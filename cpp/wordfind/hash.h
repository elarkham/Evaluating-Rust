/* See LICENSE file for copyright and license details */
#define K char *
#define V size_t

class HashTable {
	public:
		HashTable();
		HashTable(size_t n);
		~HashTable();

		void insert(const K key, V data);
		V *find(const K key);
	private:
		struct Entry {
			const K key;
			V value;
			Entry *next;
		};
		size_t size;
		size_t n_mulock;
		pthread_mutex_t *mulock;
		Entry **entry;

		void lock(size_t);
		Entry **lookup(const K);
		void unlock(size_t);
};
