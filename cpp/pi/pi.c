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
#include <time.h>
#include <unistd.h>

#define PROG_NAME "pi"

#define EPRINT(...) \
	do { \
		fprintf(stderr, PROG_NAME ": error: " __VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while (0);

/* Number of intervals to divide the area beneath the curve in [0,1] into */
#define INTERVALS 50000000

// width of an interval
#define WIDTH (1.0/INTERVALS)

/* Number of intervals each thread will calculate */
size_t chunk;

/* Processor IDs less than split have one extra interval */
size_t split;

/* Child threads drop values of their interval calculation into the array */
double *partial_sums;

void
usage()
{
	fprintf(stderr, "Usage: " PROG_NAME " [NUMBER OF THREADS]\n");
	exit(EXIT_FAILURE);
}

void *
work(void *in)
{
	size_t i;
	size_t low;                 // first interval to be processed
	size_t high;                // first interval *not* to be processed
	double localSum = 0.0;   // sum for intervals being processed
	double x;                // mid-point of an interval
	size_t id = (size_t)in;

	if (id < split) {
		low = (id * (chunk + 1));
		high = low + (chunk + 1);
	} else {
		low = (split * (chunk + 1)) + ((id - split) * chunk);
		high = low + chunk;
	}
	x = (low+0.5)*WIDTH;
	for (i = low; i < high; i++) {
		localSum += (4.0/(1.0+x*x));
		x += WIDTH;
	}
	partial_sums[id] = localSum;
	return 0;
}

int
main(int argc, char *argv[])
{
	size_t i;
	/* Number of child threads to create. Note that the final approximation
	 * of pi may be different depending on how many threads are used to do
	 * floating point addition rounding. */
	size_t n; 
	/* Array for storing thread IDs */
	pthread_t *ptid;
	/* Final summed value of all work done between threads */
	double sum = 0.0;
	struct timespec start, stop;

	if (argc !=2)
		usage();
	n = atoi(argv[1]);
	if (n <= 0)
		EPRINT("Less than 0 child threads requested\n")
	/* Allocate according to the number of threads requested */
	ptid = (pthread_t *)malloc(sizeof(*ptid) * n);
	partial_sums = (double *)malloc(sizeof(*partial_sums) * n);
	chunk = INTERVALS / n;
	split = INTERVALS % n;
	if (0 == split) {
		split = n;
		chunk -= 1;
	}
	/* Start the timer */
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (i = 0; i < n; ++i) {
		if (pthread_create(&ptid[i], NULL, work, (void *) i) != 0)
			EPRINT("Could not create thread\n");
	}
	/* Wait for each thread to finish */
	for (i = 0; i < n; ++i)
	{
		if (pthread_join(ptid[i], NULL))
		{
			EPRINT("Could not join thread\n");
		}
		sum += partial_sums[i];
	}
	sum *= 1.0 / INTERVALS;
	/* End the timer */
	clock_gettime(CLOCK_MONOTONIC, &stop);
	/* Print out total runtime of algorithm */
	printf("%f\n", (double) (stop.tv_nsec - start.tv_nsec) / 1000000000 +
			(double) (stop.tv_sec - start.tv_sec));
	//printf ("Estimation of pi is %14.12f\n", sum);
	return 0;
}
