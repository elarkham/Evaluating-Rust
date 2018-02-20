/* See LICENSE file for copyright and license details
 ** Authors:
 **     Todd Gaunt
 ** Description:
 **	This program implements the Jacobi algorithm concurrently to find the
 **	steady-state temperature distribution on an insulated
 **	two-dimensional plate, given constant boundary conditions.
 **
 **	This version was created in November 2017, to compare to a C++ version
 **	Using Pthreads.
 */

extern crate time;
extern crate crossbeam;

use time::PreciseTime;
use std::env;
use std::process;

const SIZE : usize = 2560;
const TEMP : f64 = 50.0;
const EPSILON : f64 = 0.1;

fn usage() {
    eprint!("Usage: jacobi [NUMBER_OF_THREADS]\n");
}

fn main () {
    let args: Vec<String> = env::args().collect();

    if 2 != args.len() {
        usage();
        process::exit(-1);
    }

    // Number of threads to use
    let n_threads = args[1].parse::<usize>().unwrap();
    let start = PreciseTime::now();
    let split = SIZE / n_threads;
    let mut new : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];
    let mut old : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];

    /* Initialize the temperatures */
    for i in 1..SIZE {
        /* Inner core */
        for j in 1..SIZE - 1 {
            old[i][j] = 50.0;
            new[i][j] = 50.0;
        }
    }
    for i in  1..SIZE - 1 {
        /* South Boundry */
        old[SIZE - 1][i] = 100.0;
        new[SIZE - 1][i] = 100.0;
    }

    loop {
        let mut maxerr = 0.0;
        /* Scope guarentees children die before references become stale */
        crossbeam::scope(|scope| {
            let mut children = vec![];
            let mut slice = new.as_mut_slice();
            for id in 0..n_threads {
                let old = &old;
                /* The braces around slice ar a hack to get Rust to not
                 * reborrow slice */
                let (rows, rest) = {slice}.split_at_mut(split);
                children.push(scope.spawn(move || {
                    let mut beg = id * split;
                    let mut end = beg + split;
                    if id == 0 {
                        beg += 1;
                    }
                    if id == n_threads - 1 {
                        end -= 1;
                    }
                    let mut maxerr = 0.0;
                    for i in beg..end {
                        for j in 1..SIZE - 1 {
                            rows[i - id * split][j] = (old[i - 1][j]
                                                       + old[i + 1][j]
                                                       + old[i][j + 1]
                                                       + old[i][j - 1]) / 4.0;
                            let change = (old[i][j] - rows[i - id * split][j]).abs();
                            if maxerr < change {
                                maxerr =  change;
                            }
                        }
                    }
                    return maxerr;
                }));
                slice = rest;
            }
            for child in children {
                let tmp = child.join();
                if maxerr < tmp {
                    maxerr = tmp;
                }
            }
        });
        if EPSILON >= maxerr {
            break;
        }
        /* Print-out for debugging */
        /*
           for i in 0..SIZE {
           for j in 0..SIZE {
           print!("{} ", new[i][j]);
           }
           println!();
           }
           */
        for i in 1..SIZE - 1 {
            for j in 1..SIZE - 1 {
                old[i][j] = new[i][j];
            }
        }
    }

    let mut cool_cells = 0;
    for i in 0..SIZE {
        for j in 0..SIZE {
            if new[i][j] < TEMP {
                cool_cells += 1
            }
        }
    }
    let stop = PreciseTime::now();

    let elapsed = start.to(stop).to_string();
    println!("{}", &elapsed[2..]);
    eprintln!("There are {} cells cooler than {} degrees",
              cool_cells,
              TEMP);
}
