/* See LICENSE file for copyright and license details */
/**
 * Authors:
 *	Philip J. Hatcher
 *	Todd Gaunt
 * Description:
 *	Concurrently compute an approximation of pi.
 */

#include <errno.h>
#include <pthread.h>
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

/* Number of intervals per thread */
int chunk;
/* Processor IDs less than split have one extra interval */
int split;
/* Child threads drop values into the array */
double partial_sum[THREAD_MAX];

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
	/* First interval to be processed */
	int low;                 
	/* First interval *not* to be processed */
	int high;              
	/* Sum for intervals being processed */
	double localSum = 0.0;   
	/* Mid-point of an interval */
	double x;                
	/* Logical thread id (0..n-1) */
	long id = (long) in;

	if (id < split) {
		low = (id * (chunk + 1));
		high = low + (chunk + 1);
	} else {
		low = (split * (chunk + 1)) + ((id - split) * chunk);
		high = low + chunk;
	}
	x = (low+0.5)*WIDTH;
	for (i = low; i < high; ++i) {
		localSum += (4.0/(1.0+x*x));
		x += WIDTH;
	}
	partial_sum[id] = localSum;
	return 0;
}

	int
main(int argc, char **argv)
{
	size_t i = 0;
	size_t n = 0;
	double sum = 0.0;
	pthread_t pid[THREAD_MAX] = {0};

	if (2 != argc)
		usage();
	n = atoi(argv[1]);
	if (0 >= n) 
		EPRINT("number of threads must be greater than zero\n");
	if (THREAD_MAX < n)
		EPRINT("too many threads requested\n");
	//TODO(todd): Start timer here.
	chunk = INTERVALS / n;
	split = INTERVALS % n;
	if (0 == split) {
		split = n;
		chunk -= 1;
	}
	/* Create the worker threads */
	for (i = 0; i < n; ++i) {
		if (0 != pthread_create(&pid[i], NULL, work, (void *) i))
			EPRINT("could not create thread\n");
	}
	// wait for each thread to finish
	for (i=0; i < n; i++) {
		if (pthread_join(pid[i], NULL))
			EPRINT("could not join thread\n");
		sum += partial_sum[i];
	}
	sum *= 1.0 / INTERVALS;
	//TODO(todd): Stop timer here.
	printf("Estimation of pi is %14.12f\n", sum);
	return 0;
}
