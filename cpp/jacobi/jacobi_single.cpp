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

#define SIZE    1024
#define TEMP    50.0
#define EPSILON 0.1

double new_p[SIZE][SIZE];
double old_p[SIZE][SIZE];

int main()
{
	struct timespec start, stop;
	double maxerr;
	double change;
	int   cool_cells;
	int   i,j;

	clock_gettime(CLOCK_MONOTONIC, &start);

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
		maxerr = 0.0;
		for (i = 1; i < SIZE-1; i++)
			for (j = 1; j < SIZE-1; j++)
			{
				new_p[i][j] = (old_p[i-1][j] + old_p[i+1][j] +
						old_p[i][j+1] + old_p[i][j-1]) / 4.0;
				change = fabs(old_p[i][j] - new_p[i][j]);
				if (maxerr < change) maxerr =  change;
			}
		for (i = 1; i < SIZE-1; i++)
			for (j = 1; j < SIZE-1; j++)
			{
				old_p[i][j] = new_p[i][j];
			}
	} while (maxerr > EPSILON);

	cool_cells = 0;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			if (new_p[i][j] < TEMP) cool_cells++;

	clock_gettime(CLOCK_MONOTONIC, &stop);
	std::cout << (double) (stop.tv_nsec - start.tv_nsec) / 1000000000 +
		(double) (stop.tv_sec - start.tv_sec) << std::endl;

	fprintf (stderr, "There are %d cells cooler than %5.2f degrees\n",
			cool_cells, TEMP);

	return 0;
}
