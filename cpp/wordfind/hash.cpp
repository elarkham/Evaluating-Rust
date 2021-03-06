/* See LICENSE file for copyright and license details */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#include "hash.h"

#define LOCKCOUNT 8
#define K const char *
#define V size_t

static void
eprint(const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "HashTable : error: ");

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}

// Compute a hash for the symbol table.
static unsigned int
hash(K key)
{
	const unsigned int p = 16777619;
	unsigned int hash = 2166136261u;
	while (*key) {
		hash = (hash ^ *key) * p;
		key += 1;
	}
	hash += hash << 13;
	hash ^= hash >> 7;
	hash += hash << 3;
	hash ^= hash >> 17;
	hash += hash << 5;
	return hash;
}

HashTable::HashTable(size_t n)
{
	this->entry = (HashTable::Entry **)calloc(n, sizeof(*this->entry));
	this->size = n;
	this->mulock = (pthread_mutex_t *)malloc(sizeof(*this->mulock) * LOCKCOUNT);
	for (size_t i = 0; i < LOCKCOUNT; ++i)
		if (pthread_mutex_init(&this->mulock[i], NULL))
			eprint("Unable to initialize mutex.\n");
}

HashTable::~HashTable()
{
	size_t i;
	HashTable::Entry *ep;

	for (i = 0; i < this->size; ++i) {
		ep = this->entry[i];
		while (ep) {
			HashTable::Entry *garbage = ep;
			ep = ep->next;
			free(garbage);
		}
	}
	free(this->entry);
	free(this->mulock);
}


// Lock a portion of the table
void
HashTable::lock(size_t index)
{
	if (pthread_mutex_lock(&this->mulock[index]))
		eprint("Unable to lock mutex, halting.\n");
}

// Unlock a portion of the table.
void
HashTable::unlock(size_t index)
{
	if (pthread_mutex_unlock(&this->mulock[index]))
		eprint("Unable to unlock mutex, halting.\n");
}

void
HashTable::update(K key, void (*func)(V *, void *), void * arg)
{
	HashTable::Entry **epp;
	size_t n = hash(key) % this->size;
	
	this->lock(n % LOCKCOUNT);
	epp = &this->entry[n];
	while (*epp) {
		if (0 == strcmp(key, (*epp)->key))
			break;
		epp = &(*epp)->next;
	}

	if (!*epp) {
		*epp = (HashTable::Entry *)calloc(1, sizeof(**epp));
	}
	(*epp)->key = strdup(key);
	func(&(*epp)->value, arg);
	this->unlock(n % LOCKCOUNT);
}

HashTable::Iterator
HashTable::begin()
{
	size_t i;
	HashTable::Iterator iter;
	HashTable::Entry *ep = NULL;

	for (i = 0; i < this->size; ++i) {
		ep = this->entry[i];
		if (ep)
			break;
	}
	iter.tab = this;
	iter.index = i;
	iter.cur = ep;

	return iter;
}

bool
HashTable::Iterator::next()
{
	if (NULL == this->cur)
		return false;
	/* Attempt to traverse the bucket list */
	this->cur = this->cur->next;
	if (this->cur)
		return true;
	++this->index;
	while (this->index < this->tab->size) {
		this->cur = this->tab->entry[this->index];
		if (this->cur)
			return true;
		++this->index;
	}
	this->cur = NULL;
	return false;
}

K *
HashTable::Iterator::get_key()
{
	return &this->cur->key;
}

V *
HashTable::Iterator::get_value()
{
	return &this->cur->value;
}
