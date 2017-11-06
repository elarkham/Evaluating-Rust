/* See LICENSE file for copyright and license details */
/**
 * Authors:
 *	Todd O. Gaunt
 * Description:
 *	Concurrently compute an approximation of pi.
 */

/*TODO(todd): Replce old C headers.
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
*/

use std::env;

const PROG_NAME: str = "pi";
/* Number of intervals to divide the area beneath the curve in [0,1] into */
const INTERVALS: usize 50000000
// width of an interval
const WIDTH: f64 (1.0/INTERVALS)
/* Number of intervals each thread will calculate */
static chunk: &'static usize = 0;
/* Processor IDs less than split have one extra interval */
static split: &'static usize = 0;

fn usage() {
	eprint!("Usage: {} [NUMBER OF THREADS]\n", PROG_NAME);
	process::exit(-1);
}

fn work(id: usize) {
	let mut low: usize = 0; // first interval to be processed
	let mut high: usize = 0; // first interval *not* to be processed
	let mut localSum: f64 = 0.0; // sum for intervals being processed

	if id < split {
		low = (id * (chunk + 1));
		high = low + (chunk + 1);
	} else {
		low = (split * (chunk + 1)) + ((id - split) * chunk);
		high = low + chunk;
	}

	let x: f64 = (low+0.5)*WIDTH;

	for i in low..high {
		localSum += (4.0/(1.0+x*x));
		x += WIDTH;
	}
	partial_sums[id] = localSum;
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut tid: Vec<JoinHandle> = Vec::new();
	let mut sum: f64 = 0;
	/* Child threads drop values of their interval calculation into the array */
	let mut partial_sums: Vec<64> = Vec::new();

	if args.len() != 2 {
		usage();
	}

	/* Number of child threads to create. Note that the final approximation
	 * of pi may be different depending on how many threads are used to do
	 * floating point addition rounding. */
	let n: usize = atoi(argv[1]);

	if n <= 0 {
		eprint!("Lees than 0 child threads requested\n");
		process::exit(-1);
	}
	chunk = INTERVALS / n;
	split = INTERVALS % n;
	if 0 == split {
		split = n;
		chunk -= 1;
	}
	for i in 0..n {
		thread::spawn(|| {
			work(partial_sums, i);
		}):
	}
	/* Wait for each thread to finish */
	for i in 0..n {
		tid[i].join();
		sum += partial_sums[i];
	}
	sum *= 1.0/INTERVALS;
	print!("Estimation of pi is {}\n", sum);
}
