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

//extern crate time;

use std::thread;
//use time::PreciseTime;

const SIZE : usize = 1280;
const TEMP : f64 = 50.0;
const EPSILON : f64 = 0.1;

fn main () {
    let n_threads = 4;
    let mut children = vec![];
    let split = SIZE / n_threads;
    for i in 0..n_threads {
        children.push(thread::spawn(move || {
            let mut new : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; split];
            let mut old : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; split + 1];

            /* Initialize the temperatures */
            for i in  1..(SIZE - 1) {
                /* South Boundry */
                old[split-1][i] = 100.0;
                new[split-1][i] = 100.0;
            }
            for i in 1..(split - 1) {
                /* Inner core */
                for j in 1..(SIZE - 1) {
                    old[i][j] = 50.0;
                    new[i][j] = 50.0;
                }
            }
            loop {
                let mut maxerr = 0.0;
                for i in 1..split - 1 {
                    for j in 1..SIZE - 1{
                        new[i][j] = (old[i-1][j] + old[i+1][j] +
                                     old[i][j+1] + old[i][j-1]) / 4.0;
                        let change = (old[i][j]-new[i][j]).abs();
                        if maxerr < change {
                            maxerr =  change; 
                        }
                    }
                }
                for i in 1..split - 1 {
                    for j in 1..SIZE - 1 {
                        old[i][j] = new[i][j];
                    }
                }
                if EPSILON >= maxerr {
                    break;
                }
            }
            return new;
        }));
    }
    let mut cool_cells = 0;
    for child in children {
        let new = child.join().unwrap();
        for i in 0..split {
            for j in 0..SIZE {
                if (new[i][j] < TEMP) {
                    cool_cells += 1;
                }
            }
        }
        println!("JOIN!");
    }
    println!("There are {} cells cooler than {} degrees",
             cool_cells,
             TEMP);
}
