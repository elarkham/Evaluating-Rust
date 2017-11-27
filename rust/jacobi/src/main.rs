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
    let mut new_grid : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];
    let mut old_grid : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];

    /* Initialize the temperatures */
    /* Commented out since rust requires zero initialization anyways */
    for i in 0..SIZE {
        /* North, East, and West boundaries */
        old_grid[0][i] = 0.0;
        new_grid[0][i] = 0.0;
        old_grid[i][0] = 0.0;
        new_grid[i][0] = 0.0;
        old_grid[i][SIZE-1] = 0.0;
        new_grid[i][SIZE-1] = 0.0;
    }
    for i in  1..(SIZE - 1) {
        /* South Boundry */
        old_grid[SIZE-1][i] = 100.0;
        new_grid[SIZE-1][i] = 100.0;
    }
    for i in 1..(SIZE - 1) {
        /* Inner core */
        for j in 1..(SIZE - 1) {
            new_grid[i][j] = 50.0;
        }
    }

    loop {
        for i in 1..(SIZE - 1) {
            for j in 1..(SIZE - 1) {
                old_grid[i][j] = new_grid[i][j];
            }
        }
        let mut maxerr = 0.0;
        crossbeam::scope(|scope| {
        let mut children = vec![];
        for (count, new_row) in new_grid.iter_mut().skip(1).take(SIZE - 2).enumerate() {
            let i = count + 1;
            let old_ref = &old_grid;
            children.push(scope.spawn(move || {
                let mut change = 0.0;
                for j in 1..(SIZE - 1) {
                    new_row[j] = (old_ref[i-1][j] + old_ref[i+1][j] +
                                 old_ref[i][j+1] + old_ref[i][j-1]) / 4.0;
                    let tmp = (old_ref[i][j]-new_row[j]).abs();
                    if change < tmp {
                        change = tmp; 
                    }
                }
                return change;
            }));
        }
        for child in children {
            let change = child.join();
            if maxerr < change {
                maxerr = change; 
            }
        }
        });
        //children.clear();
        if maxerr <= EPSILON {
            break;
        }
    }

    let mut cool_cells : usize = 0;
    for i in 0..SIZE {
        for j in 0..SIZE {
            if new_grid[i][j] < TEMP {
                cool_cells += 1;
            }
        }
    }

    /* Will print out timing info here
    println!("{}", 0);
    */
    println!("There are {} cells cooler than {} degrees",
              cool_cells,
              TEMP);
}
