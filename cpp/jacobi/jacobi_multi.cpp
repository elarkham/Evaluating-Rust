/* See LICENSE file for copyright and license details */
/**
 * Authors:
 *	Philip J. Hatcher
 *	Ethan Larkham
 *
 * Description:
 *  This serial program implements the Jacobi algorithm to find the
 *  steady-state temperature distribution on an insulated
 *  two-dimensional plate, given constant boundary conditions.
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

#include <vector>
#include <iostream>

std::string const PROG_NAME = "jacobi";

#define EPRINT(x) \
	do { \
		std::cerr << PROG_NAME << ": error: " << x << std::endl; \
		exit(EXIT_FAILURE); \
	} while (0)

#define SIZE    2560
#define TEMP    50.0
#define EPSILON 0.1

size_t n_threads;
double split;

double new_p[SIZE][SIZE];
double old_p[SIZE][SIZE];

void* work(void* in)
{
	size_t id  = (size_t)in;
  size_t beg = id * split;
  size_t end = beg + split;

  if (id == 0) {
    beg += 1;
  }
  if (id == (n_threads - 1)) {
    end -= 1;
  }

  for (size_t i = beg; i < end; i++) {
    for (size_t j = 1; j < SIZE; j++) {
      new_p[i][j] = ( old_p[i-1][j]
                    + old_p[i+1][j]
                    + old_p[i][j+1]
                    + old_p[i][j-1] ) / 4.0;
    }
  }

	return 0;
}

void usage()
{
	std::cerr << "Usage: " << PROG_NAME << " [NUMBER OF THREADS]"
		<< std::endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	struct timespec start, stop;

  double maxerr;
	double change;
	size_t i,j;
	int cool_cells;

  /* Get thread number from first argument */
  if (argc !=2) {
    usage();
  }
  n_threads = atoi(argv[1]);
  if (n_threads <= 0)
    EPRINT("Less than 0 child threads requested");
  split = SIZE / n_threads;

  /* Array for storing thread IDs */
	clock_gettime(CLOCK_MONOTONIC, &start);
  std::vector<pthread_t> ptid;
  ptid.reserve(n_threads);

  for (i = 0; i < n_threads; i++) {
    if (pthread_create(&ptid[i], NULL, work, (void *) i) != 0)
      EPRINT("Could not create thread");
  }

  /* north, east, and west boundaries */
	for (i = 0; i < SIZE; i++)
  {
		old_p[0][i] = new_p[0][i] = 0.0;
		old_p[i][0] = new_p[i][0] = 0.0;
		old_p[i][SIZE-1] = new_p[i][SIZE-1] = 0.0;
	}

  /* south boundary */
	for (i = 1; i < SIZE-1; i++)
	{
    old_p[SIZE-1][i] = new_p[SIZE-1][i] = 100.0;
	}

  /* inner core */
	for (i = 1; i < SIZE-1; i++) {
    for (j = 1; j < SIZE-1; j++) {
			new_p[i][j] = 50.0;
			old_p[i][j] = 50.0;
		}
	}

  /* Compute steady-state temperatures */
	do {
    /* delegate work to threads */
    for(i = 0; i < n_threads; i++) {
	    if(pthread_create(&ptid[i], NULL, work, (void *) i) != 0)
        EPRINT("Could not create thread");
    }
    for (size_t i = 0; i < n_threads; ++i) {
      if (pthread_join(ptid[i], NULL))
        EPRINT("Could not join thread");
    }

    /* update maxerr */
		maxerr = 0.0;
		for (i = 1; i < SIZE-1; i++) {
			for (j = 1; j < SIZE-1; j++)
      {
				change = fabs(old_p[i][j] - new_p[i][j]);
				if (maxerr < change){
          maxerr = change;
        }
			}
    }

    /* transfer new to old */
		for (i = 1; i < SIZE-1; i++) {
			for (j = 1; j < SIZE-1; j++)
      {
				old_p[i][j] = new_p[i][j];
			}
    }
	} while (maxerr > EPSILON);

	cool_cells = 0;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			if (new_p[i][j] < TEMP) cool_cells++;

  /* end timer */
	clock_gettime(CLOCK_MONOTONIC, &stop);
	std::cout << (double) (stop.tv_nsec - start.tv_nsec) / 1000000000 +
			(double) (stop.tv_sec - start.tv_sec) << std::endl;

	fprintf(stderr,
      "There are %d cells cooler than %5.2f degrees\n", cool_cells, TEMP);

	return 0;
}
