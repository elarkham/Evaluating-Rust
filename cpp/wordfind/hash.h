/* See LICENSE file for copyright and license details */
#ifndef HASH_H
#define HASH_H

#include <stdlib.h>

#define K const char *
#define V size_t

class HashTable {
	private:
		struct Entry;
	public:
		HashTable();
		HashTable(size_t n);
		~HashTable();

		void update(K key, void (*func)(V *, void *), void *arg);
		struct Iterator {
			size_t index;
			HashTable *tab;
			Entry *cur;

			Iterator() {};
			K *get_key();
			V *get_value();
			bool next();
		};
		Iterator begin();
		Iterator end();
	private:
		struct Entry {
			K key;
			V value;
			Entry *next;
		};
		size_t size;
		size_t n_mulock;
		pthread_mutex_t *mulock;
		Entry **entry;

		void lock(size_t);
		Entry **lookup(K);
		void unlock(size_t);
};

#endif
