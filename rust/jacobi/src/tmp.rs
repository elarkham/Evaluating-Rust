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

use std::thread;
use std::sync::mpsc;
use std::cmp::max;
use std::cmp::min;
//extern crate time;
//use time::PreciseTime;

const SIZE : usize = 64;
const TEMP : f64 = 50.0;
const EPSILON : f64 = 0.1;

fn main () {
    let n_threads = 4;
    let channels = vec![];
    for i in 0..n_threads {
        channels.push(mpsc::channel());
    }
    for i in 0..n_threads {
        /* Create a copy of channels needed for each thread */
        let tx = channels[i].0.clone();
        let rx = &channels[i].1;
        let split = SIZE / n_threads;
        thread::spawn(move || {
            let mut new_grid = vec![vec![0.0; SIZE]; split];
            /* Old grid has extra row required for computation */
            let mut old_grid = vec![vec![0.0; SIZE]; split + 2];
            /* Initialize the temperatures. North, East, and West boundaries
             * are already 0.0 */
            if i == n_threads - 1 {
                /* South Boundry, only should be the final thread */
                for i in  1..SIZE {
                    old_grid[split-1][i] = 100.0;
                    new_grid[split-1][i] = 100.0;
                }
            }
            for i in 1..(split - 1) {
                /* Inner core */
                for j in 1..SIZE {
                    new_grid[i][j] = 50.0;
                    old_grid[i + 1][j] = 50.0;
                }
            }
            if i < n_threads && i > 0 {
                /* Initialize extra bounds on old_grid */
                for i in 1..SIZE {
                    old_grid[0][i] = 50.0;
                    old_grid[split + 1][i] = 50.0;
                }
            }
            /*
            /* Main computation begins here */
            loop {
                for i in 1..(split - 1) {
                    for j in 1..(split - 1) {
                        new[i][j] = (old[i-1][j] + old[i+1][j] +
                                     old[i][j+1] + old[i][j-1]) / 4.0;
                        change = (old[i][j]-new[i][j]).abs();
                        if maxerr < change {
                            maxerr =  change; 
                        }
                    }
                }
                for i in 1..(split - 1) {
                    for j in 1..(split - 1) {
                        old_grid[i][j] = new_grid[i][j];
                    }
                }
                */
                    /* Send rows of old_grid that are needed by other threads */
                    channels[n_threads - 1].0.send((1, old_grid[1].clone()));
                channels[n_threads + 1].0.send((1, old_grid[split - 1].clone()));
                /* Receive the needed rows for next iteration */
                //old_grid[0] = channels[n_threads - 1].recv.unwrap();
                //old_grid[split] = channels[n_threads + 1].recv.unwrap();
        });
    }
}

    /* Join threads here to get number of cool cells */

    /* Will print out timing info here
       println!("{}", 0);
       */
    /*
       eprintln!("There are {} cells cooler than {} degrees",
       cool_cells,
       TEMP);
       */
