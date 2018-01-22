/* See LICENSE file for copyright and license details */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#include "hash.h"

#define K char *
#define V size_t

static void
eprint(char *fmt, ...)
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

HashTable::Entry **
HashTable::lookup(K key)
{
	HashTable::Entry **ep;
	size_t index = hash(key) % this->size;
	
	this->lock(index);
	ep = &this->entry[index];
	while (*ep) {
		if (key == (*ep)->key)
			return ep;
		ep = &(*ep)->next;
	}
	this->unlock(index);
	return ep;
}

void
HashTable::insert(K key, V value)
{
	HashTable::Entry **epp = this->lookup(key);

	if (!*epp) {
		*epp = (HashTable::Entry *)calloc(1, sizeof(**epp));
	}
	(*epp)->key = key;
	(*epp)->value = value;
}

V
HashTable::find(K key)
{
	Entry **epp = this->lookup(key);

	return (*epp)->value;
}

int 
main(int argc, char **argv)
{
	return 0;
}
