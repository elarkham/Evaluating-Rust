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
use std::sync::mpsc;
use std::sync::mpsc::{Sender, Receiver};
//use time::PreciseTime;

const SIZE : usize = 1280;
const TEMP : f64 = 50.0;
const EPSILON : f64 = 0.1;

fn main () {
    let n_threads = 4;
    let mut children = vec![];
    let split = SIZE / n_threads;
    let mut channels : Vec<
        (Sender<(i32, Vec<f64>)>,
        Receiver<(i32, Vec<f64>)>)> = vec![];
    for i in 0..n_threads {
        channels.push(mpsc::channel());
    }
    for i in 0..n_threads {
        let tx = channels[i + 1].0.clone();
        children.push(thread::spawn(move || {
            let mut new : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; split];
            let mut old : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; split + 2];

            /* The first thread started contains the northern boundry */
            if 0 == i {
                for i in 0..split {
                    new[0][i] = 0.0;
                }
                for i in 1..split {
                    /* Inner core */
                    for j in 1..(SIZE - 1) {
                        new[i][j] = 50.0;
                    }
                }
            }
            /* Only the last thread started contains the south boundry */
            if n_threads == i {
                /* Initialize the temperatures */
                for i in  1..(SIZE - 1) {
                    /* South Boundry */
                    new[split-1][i] = 100.0;
                }
                for i in 0..(split - 1) {
                    /* Inner core */
                    for j in 1..(SIZE - 1) {
                        new[i][j] = 50.0;
                    }
                }
            } else {
                for i in 0..split {
                    /* Inner core */
                    for j in 1..(SIZE - 1) {
                        new[i][j] = 50.0;
                    }
                }
            }
            loop {
                for i in 1..split - 1 {
                    for j in 1..SIZE - 1 {
                        old[i + 1][j] = new[i][j];
                    }
                }
                let mut maxerr = 0.0;
                for i in 1..split - 1 {
                    for j in 1..SIZE - 1{
                        new[i][j] = (old[i][j]
                                     + old[i + 2][j]
                                     + old[i + 1][j + 1]
                                     + old[i + 1][j - 1]) / 4.0;
                        let change = (old[i + 1][j] - new[i][j]).abs();
                        if maxerr < change {
                            maxerr =  change; 
                        }
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
        let chunk = child.join().unwrap();
        for i in 0..split {
            for j in 0..SIZE {
                if chunk[i][j] < TEMP {
                    cool_cells += 1;
                }
            }
        }
        eprintln!("JOIN!");
    }
    eprintln!("There are {} cells cooler than {} degrees",
             cool_cells,
             TEMP);
}
