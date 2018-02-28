/* See LICENSE file for copyright and license details
 ** Authors:
 **     Todd Gaunt
 ** Description:
 **     This rust program solves a linear system of equations.
 **
 **	This version was created in 2018, to compare to a C++ version
 **	Using Pthreads.
 */

extern crate rand;

use rand::Rng;

const SIZE : usize = 512;
const EPSILON : f64 = 1.0e-20;

/* cook up a system where ans[i] will be i */
fn initialize_system(a : &mut Vec<Vec<f64>>, marked : &mut Vec<i32>) {
    let mut rng = rand::thread_rng();

    for i in 0..SIZE {
        marked[i] = 0;
        a[i][SIZE] = 0.0;
        for j in 0..SIZE {
            a[i][j] = rng.gen();
            a[i][SIZE] += j as f64 * a[i][j];
        }
    }
}

/* reduce the matrix using partial pivoting */
fn gaussian_elimination(a : &mut Vec<Vec<f64>>, marked : &mut Vec<i32>, pivot : &mut Vec<i32>) -> bool
{
    let mut picked : usize = 0;
    let mut tmp : f64;

    for i in 0..SIZE - 1 {
        tmp = 0.0;
        for j in 0..SIZE {
            if 0 == marked[j] && a[j][i].abs() > tmp {
                tmp = a[j][i].abs();
                picked = j;
            }
        }
        marked[picked] = 1;    /* Mark pivot row */
        pivot[picked] = i as i32;     /* Remember permuted position */

        if a[picked][i].abs() < EPSILON {
            println!("Exits on iteration {}", i);
            return false;
        }

        for j in 0..SIZE {
            if 0 == marked[j] {
                tmp = a[j][i] / a[picked][i];
                for k in i..SIZE + 1 {
                    a[j][k] = a[j][k] - a[picked][k] * tmp;
                }
            }
        }
    }
    for i in 0..SIZE {
        if 0 == marked[i] {
            pivot[i] = (SIZE - 1) as i32;
        }
    }
    return true;
}

/* solve the (logical) upper triangular matrix */
fn back_substitution(a : &mut Vec<Vec<f64>>, pivot : &mut Vec<i32>)
{
    for i in (0..SIZE).rev() {
        let mut j = 0;
        loop {
            if i == pivot[j] as usize {
                break;
            }
            j = j + 1;
        }
        let coeff = a[j][SIZE] / a[j][i];
        for j in 0..SIZE {
            if pivot[j] < i as i32 {
                a[j][SIZE] -= coeff * a[j][i];
            }
        }
    }
}

/* ans[i] should be (roughly) i */
/* to get ans I still need to divide by the non-zero column's coefficient */
/* might end up with -0.0 so I must special-case that */
fn check_results()
{
    // for i in 0..SIZE {
    //     char buf1[100], buf2[100];

    //     let mut ans = a[i][SIZE]/a[i][pivot[i]];
    //     if ((pivot[i] == 0) && (ans < 0.0))
    //         ans = -ans;
    //     sprintf(buf1, "%10.6f", (double) pivot[i]);
    //     sprintf(buf2, "%10.6f", ans);
    //     /* printf("%s == %s?\n", buf1, buf2); */
    //     assert (strcmp(buf1, buf2) == 0);
    // }

    println!("Done.");
}

fn main() {
    let mut a : Vec<Vec<f64>> = vec![vec![0.0; SIZE + 1]; SIZE];
    let mut pivot : Vec<i32> = vec![0; SIZE];
    let mut marked : Vec<i32> = vec![0; SIZE];

    initialize_system(&mut a, &mut marked);
    let solution = gaussian_elimination(&mut a, &mut marked, &mut pivot);
    if solution {
        back_substitution(&mut a, &mut pivot);
        check_results();
    } else {
        println!("No solution?");
    }
}
