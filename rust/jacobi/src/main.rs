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
//extern crate time;
//use time::PreciseTime;
extern crate crossbeam;

const SIZE : usize = 1280;
const TEMP : f64 = 50.0;
const EPSILON : f64 = 0.1;

fn main () {

    let mut children = vec![];
    let split = SIZE / 4;
    for i in 0..4 {
        children.push(thread::spawn(move || {
            let mut new : Vec<Vec<f64>> = vec![vec![0.0; split]; split];
            let mut old : Vec<Vec<f64>> = vec![vec![0.0; split + 2]; split];

            /* Initialize the temperatures */
            for i in  1..(split - 1) {
                /* South Boundry */
                old[split-1][i] = 100.0;
                new[split-1][i] = 100.0;
            }
            for i in 1..(split - 1) {
                /* Inner core */
                for j in 1..(split - 1) {
                    old[i][j] = 50.0;
                    new[i][j] = 50.0;
                }
            }
            loop {
                let mut maxerr = 0.0;
                for i in 1..split - 1 {
                    for j in 1..split - 1{
                        new[i][j] = (old[i-1][j] + old[i+1][j] +
                                     old[i][j+1] + old[i][j-1]) / 4.0;
                        let change = (old[i][j]-new[i][j]).abs();
                        if maxerr < change {
                            maxerr =  change; 
                        }
                    }
                }
                for i in 1..split - 1 {
                    for j in 1..split - 1 {
                        old[i][j] = new[i][j];
                    }
                }
                if EPSILON >= maxerr {
                    break;
                }
            }
        }));
    }
    for child in children {
        child.join();
        println!("JOIN!");
    }
    /*
       println!("There are {} cells cooler than {} degrees",
       cool_cells,
       TEMP);
       */
}
