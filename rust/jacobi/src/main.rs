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
        let mut children = vec![];
        for id in 0..n_threads {
            let old = old.clone();
            let mut new = new.clone();
            children.push(thread::spawn(move || {
                let mut beg = id * split;
                let mut end = beg + split;
                if id == 0 {
                    beg += 1;
                } else if id == n_threads - 1 {
                    end -= 1;
                }
                for i in beg..end {
                    for j in 1..SIZE - 1 {
                        new[i][j] = (old[i - 1][j]
                                     + old[i + 1][j]
                                     + old[i][j + 1]
                                     + old[i][j - 1]) / 4.0;
                    }
                }
                return new.clone();
            }));
        }
        let mut maxerr = 0.0;
        let mut i = 0;
        for child in children {
            let rows = child.join().unwrap();
            let mut beg = i * split;
            let mut end = beg + split;
            if i == 0 {
                beg += 1;
            } else if i == n_threads - 1 {
                end -= 1;
            }
            for j in beg..end {
                for k in 1..SIZE - 1 {
                    new[j][k] = rows[j][k];
                }
            }
            i += 1;
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
                let change = (old[i][j] - new[i][j]).abs();
                if maxerr < change {
                    maxerr =  change; 
                }
            }
        }
        if EPSILON >= maxerr {
            break;
        }
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
    eprintln!("There are {} cells cooler than {} degrees",
              cool_cells,
              TEMP);
}
