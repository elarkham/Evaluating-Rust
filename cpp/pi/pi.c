/* See LICENSE file for copyright and license details */
/**
 * Authors:
 *	Philip J. Hatcher
 *	Todd O. Gaunt
 * Description:
 *	Concurrently compute an approximation of pi.
 */

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROG_NAME "pi"

#define EPRINT(...) \
	do { \
		fprintf(stderr, PROG_NAME": error: "__VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while (0);

/* How many intervals to divide [0,1] into */
#define INTERVALS 50000000
/* Width of an interval */
#define WIDTH (1.0 / INTERVALS)
/* The maximum number of child threads that can be used */
#define THREAD_MAX 24

/* Number of threads, default number is 2 */
int n_threads = 1;
/* Number of intervals per thread */
int chunk;
/* Processor IDs less than split have one extra interval */
int split;
/* Child threads drop values into the array */
double partial_sum[4];

/* Fits inside a register */
struct worker_context {
	uint32_t id;
	uint32_t n_sums;
};

typedef struct worker_context worker_context;

void
usage()
{
	fprintf(stderr, "Usage: "PROG_NAME" [NUMBER OF THREADS]\n");
	exit(EXIT_FAILURE);
}

void *
work(void *in)
{
	int i;
	int j;
	/* First interval to be processed */
	int low;                 
	/* First interval *not* to be processed */
	int high;              
	/* Sum for intervals being processed */
	double localSum = 0.0;   
	/* Mid-point of an interval */
	double x;                
	/* Retrieve the thread context passed in */
	worker_context wc = (*((worker_context *) &in));
	uint32_t id = wc.id;
	uint32_t n_sums = wc.n_sums;
	/* Beginning index to use for the partial_sum array */
	size_t index = id * 2;

	/* Calculate n_sums partial sums and store them into the array starting
	 * at the index provided by worker_context */
	for (i = index; i < index + n_sums; ++i) {
		if (i + 1 < split) {
			low = (i * (chunk + 1));
			high = low + (chunk + 1);
		} else {
			low = (split * (chunk + 1)) + ((i - split) * chunk);
			high = low + chunk;
		}

		localSum = 0.0;
		x = (low+0.5)*WIDTH;
		for (j = low; j < high; ++j) {
			localSum += (4.0/(1.0+x*x));
			x += WIDTH;
		}
		partial_sum[i] = localSum;
	}
	return 0;
}

int
main(int argc, char **argv)
{
	size_t i = 0;
	/* Number of partial sums to calculate, default is 2 */
	size_t n = 2;
	double sum = 0.0;
	pthread_t pid[THREAD_MAX] = {0};
	int16_t n_sums = n / n_threads;

	//TODO(todd): Start timer here.
	chunk = INTERVALS / n;
	split = INTERVALS % n;
	if (0 == split) {
		split = n;
		chunk -= 1;
	}
	/* Create the worker threads */
	for (i = 0; i < n_threads; ++i) {
		worker_context wc = {i, n_sums};
		// Cast magic
		long k = *((size_t *) &wc);
		if (0 != pthread_create(&pid[i], NULL, work, (void *) k))
			EPRINT("could not create thread\n");
	}
	// wait for each thread to finish
	for (i = 0; i < n_threads; i++) {
		if (pthread_join(pid[i], NULL))
			EPRINT("could not join thread\n");
		for (size_t j = i * 2; j < i * 2 + n_sums; ++j) {
			sum += partial_sum[j];
		}
	}
	sum *= 1.0 / INTERVALS;
	//TODO(todd): Stop timer here.
	printf("Estimation of pi is %14.12f\n", sum);
	return 0;
}
