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

use time::PreciseTime;

const SIZE : usize = 1280;
const TEMP : f64 = 50.0;
const EPSILON : f64 = 0.1;

fn main () {
    // Number of threads to use
    let start = PreciseTime::now();
    let mut new : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];
    let mut old : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];

    /* Initialize the temperatures */
    for i in 1..SIZE - 1 {
        /* Inner core */
        for j in 1..SIZE - 1 {
            new[i][j] = 50.0;
            old[i][j] = 50.0;
        }
    }

    for i in  1..SIZE - 1 {
        /* South Boundry */
        old[SIZE - 1][i] = 100.0;
        new[SIZE - 1][i] = 100.0;
    }

	loop {               /* Compute steady-state temperatures */
		let mut maxerr = 0.0;
        for i in 1..SIZE - 1 {
            for j in 1..SIZE - 1{
				new[i][j] = (old[i-1][j] + old[i+1][j] +
						old[i][j+1] + old[i][j-1]) / 4.0;
				let change = (old[i][j]-new[i][j]).abs();
				if maxerr < change {
                    maxerr = change;
                }
			}
        }
        for i in 1..SIZE - 1{
            for j in 1..SIZE - 1 {
				old[i][j] = new[i][j];
			}
        }
        if maxerr < EPSILON {
            break;
        }
	}

	let mut cool_cells = 0;
	for i in 0..SIZE {
        for j in 0..SIZE {
			if new[i][j] < TEMP {
                cool_cells += 1;
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
