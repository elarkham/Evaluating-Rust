/* See LICENSE file for copyright and license details */
#define K char *
#define T int

class HashTable {
	public:
		HashTable();
		~HashTable();

		void insert(K key, T data);
		void delete(K key);
		T find(K key);
	private:
		size_t size;
		Entry **entries;
};
