/* vim: noexpandtab nosmarttab tabstop=8 shiftwidth=8 softtabstop=8 textwidth=80
 * author: Todd Gaunt
 * summary: Analyze a set of files for most commonly used words that are at
 * least 6 characters long and no more than 50 letters long.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

#include "symtab.h"

#define PRGM_NAME "prog4"

#define DPRINT(...) fprintf(stderr, PRGM_NAME": error: "__VA_ARGS__)

#define EPRINT(...)\
	{fprintf(stderr, PRGM_NAME": fatal: "__VA_ARGS__);\
	exit(EXIT_FAILURE);}

#define MAX_ARG 32
#define MAX_WORD 50
#define MIN_WORD 6

char *getword(char *buf, int *buflen, size_t maxsize, FILE *fp);
const char *find_largest(void *wordtab, int n);
void *findwords(void *vargs);
void *updateword(void *mask, void *id);

// Aguments for findwords()
struct findwords_args {
	uintptr_t id;
	void *wordtab;
	const char *filepath;
};

// Function to pass to symtab to update the information in the hash table.
// The file bitmask is stored directly inside the void pointer, as there is a
// maximum limit to how many files this program can read.
void *
updateword(void *mask, void *id)
{
	uintptr_t tmp;
	if (NULL == mask) {
		tmp = 0;
	} else {
		tmp = (uintptr_t) mask;
	}

	tmp |= (1 << (uintptr_t) id);
	return (void *)tmp;
}

// Subroutine to scan a file pointer for a completely letter-composed word.
char *
getword(char *buf, int *buflen, size_t maxsize, FILE *fp)
{
	int c = 0;
	*buflen = 0;
	do {
		c = getc(fp);
		if (EOF == c)
			return NULL;
	} while (!isalpha(c));

	while (isalpha(c)) {
		if (maxsize >= *buflen) {
			buf[*buflen] = tolower(c);
			++(*buflen);
		}
		c = getc(fp);
	}
	buf[*buflen] = '\0';
	return buf;
}

// This function is used by worker threads to scan through a file for long
// words.
void *
findwords(void *vargs)
{
	FILE *fp;
	int buflen;
	char buf[MAX_WORD + 1];
	struct findwords_args *args = (struct findwords_args *)vargs;

	if (!(fp = fopen(args->filepath, "r"))) {
		DPRINT("Unable to open file (%s), killing thread.\n",
				args->filepath);
		return vargs;
	}

	while (getword(buf, &buflen, MAX_WORD + 1, fp)) {
		// Words less than MIN_WORD letters long don't matter!
		if (MIN_WORD > buflen) {
			continue;
		} else {
			symtabUpdate(args->wordtab,
					buf,
					updateword,
					(void *) args->id);
		}
	}

	fclose(fp);

	return vargs;
}

// Iterates through the given hash table and tries to find a word that is both
// the longest and hask the same bitmask as the integer 'n'.
const char *
find_largest(void *wordtab, int n)
{
	void *data;
	void *iter;
	const char *entry;
	const char *longest;
	uintptr_t maxmask, mask;

	// Find what the perfect bitmask is.
	maxmask = 0;
	for (int i=0; i<n; ++i) {
		maxmask |= 1 << i;
	}

	// Find the largest word with the hash-table iterator.
	iter = symtabCreateIterator(wordtab);
	entry = symtabNext(iter, &data);
	longest = NULL;
	while(entry) {
		mask = (uintptr_t) data;

		// Potential match
		if (maxmask == mask) {
			if (!longest) {
				longest = entry;
			} else {
				int len = strlen(longest) - strlen(entry);
				if (0 > len) {
					longest = entry;
				} else if (0 == len) {
					if (0 < strcmp(longest, entry))
						longest = entry;
				}
			}
		}

		entry = symtabNext(iter, &data);
	}
	symtabDeleteIterator(iter);

	return longest;
}

int
main(int argc, char **argv)
{
	size_t i;
	pthread_t pt[MAX_ARG];
	struct findwords_args ap[MAX_ARG];

	if (2 > argc) {
		EPRINT("Need at least 1 file to analyze.\n");
	} else if (MAX_ARG + 1 < argc) {
		EPRINT("Maximum file limit (" MAX_ARG ") reached.\n");
	}

	HashTable = HashTable(10000, 8, 0);

	// Dispatch 1 thread per file.
	for (size_t i = 0; i < argc - 1; ++i) {
		// Initialize the arguments.
		ap[i].id = i;
		ap[i].wordtab = wordtab;
		ap[i].filepath = argv[i+1];

		if (pthread_create(pt + i, NULL, findwords, &ap[i]))
			EPRINT("Unable to create thread (%d).\n", i);
	}
	// End all child threads.
	for (size_t i = 0; i < argc - 1; ++i) {
		if (pthread_join(pt[i], NULL))
			EPRINT("Unable to join thread (%d).\n", i);
	}

	const char *tmp = find_largest(wordtab, filecnt);

	if (tmp) {
		printf("%s\n", tmp);
	} else {
		putc('\n', stdout);
	}
}
