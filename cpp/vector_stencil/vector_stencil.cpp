/* See LICENSE file for copyright and license details */
/**
 * Authors:
 *	Philip J. Hatcher
 *	Todd O. Gaunt
 * Description:
 *	This is the counterpart program to the rust "vector" program. This
 *	portion tests C++ 2d arrays compared to rust's vec! macros. This
 *	comparsion is done to attempt to explain jacobi's performance in
 *	Rust being poorer than C++.
 */

#include <time.h>

#include <stdio.h>
#include <iostream>

size_t const SIZE = 128;

int
main()
{
	size_t i, j ,k;
	double a[SIZE][SIZE] = {{0.0}};
	double sum = 0;
	struct timespec start, stop;

	/* Start the timer. Note this is wallclock time, not CPU time. */
	clock_gettime(CLOCK_MONOTONIC, &start);
	a[56][32] = 1.0;
	for (i = 0; i < 1000; ++i) {
		for (j = 1; j < SIZE - 1; ++j) {
			for (k = 1; k < SIZE - 1; ++k) {
					sum += a[j][k - 1]
						+ a[j][k + 1]
						+ a[j - 1][k]
						+ a[j + 1][k];
			}
		}
	}
	/* End the timer */
	clock_gettime(CLOCK_MONOTONIC, &stop);
	/* Print out total runtime of algorithm */
	std::cout << (double) (stop.tv_nsec - start.tv_nsec) / 1000000000 +
			(double) (stop.tv_sec - start.tv_sec) << std::endl;
	std::cerr << sum << std::endl;
}
