/* vim: noexpandtab nosmarttab tabstop=8 shiftwidth=8 softtabstop=8 textwidth=80
 * author: Todd Gaunt
 * summary: Analyze a set of files for most commonly used words that are at
 * least 6 characters long and no more than 50 letters long.
 */

#include <ctype.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

#define PRGM_NAME "prog4"

#define MAX_ARG 32
#define MAX_WORD 50
#define MIN_WORD 6

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

char *getword(char *buf, int *buflen, size_t maxsize, FILE *fp);
const char *find_largest(HashTable *wordtab, size_t n);
void *findwords(void *vargs);

/* Aguments for findwords() */
struct findwords_args {
	uintptr_t id;
	HashTable wordtab;
	const char *filepath;
};

/* Scan a file pointer for a completely letter-composed word */
char *
getword(char *buf, size_t *buflen, size_t maxsize, FILE *fp)
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
		eprint("Unable to open file (%s), killing thread.\n",
				args->filepath);
		return vargs;
	}

	while (getword(buf, &buflen, MAX_WORD + 1, fp)) {
		if (MIN_WORD > buflen) {
			/* Words shorter than MIN_WORD long don't matter! */
			continue;
		} else {
			/* The files the word appears in is kept track of
			 * through a bitmask, since the number of files this
			 * program processes is limited */
			*(args->wordtab.find(buf)) |= (1 << args->id);
		}
	}

	fclose(fp);

	return vargs;
}

// Iterates through the given hash table and tries to find a word that is both
// the longest and hask the same bitmask as the integer 'n'.
const char *
find_largest(HashTable *wordtab, int n)
{
	void *data;
	void *iter;
	const char *entry;
	const char *longest = "";
	uintptr_t maxmask, mask;

	// Find what the perfect bitmask is.
	maxmask = 0;
	for (int i=0; i<n; ++i) {
		maxmask |= 1 << i;
	}
#if 0
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
#endif

	return longest;
}

int
main(int argc, char **argv)
{
	size_t i;
	pthread_t pt[MAX_ARG];
	struct findwords_args ap[MAX_ARG];
	size_t filecnt = argc - 1;

	if (1 > filecnt) {
		eprint("Need at least 1 file to analyze.\n");
	} else if (MAX_ARG < filecnt) {
		eprint("Maximum file limit (%zu) reached.\n", MAX_ARG);
	}

	HashTable wordtab = HashTable(1000);

	/* Dispatch 1 thread per file */
	for (size_t i = 0; i < filecnt; ++i) {
		/* Initialize the arguments */
		ap[i].id = i;
		ap[i].wordtab = wordtab;
		ap[i].filepath = argv[i+1];

		if (pthread_create(pt + i, NULL, findwords, &ap[i]))
			eprint("Unable to create thread (%d).\n", i);
	}
	/* End all child threads */
	for (size_t i = 0; i < filecnt; ++i) {
		if (pthread_join(pt[i], NULL))
			eprint("Unable to join thread (%d).\n", i);
	}

	const char *tmp = find_largest(&wordtab, filecnt);

	if (tmp) {
		printf("%s\n", tmp);
	} else {
		putc('\n', stdout);
	}
}
