/* See LICENSE file for copyright and license details
** Authors:
**     Ethan Larkham
**     Todd Gaunt
** Description:
**     This is a sample file that when compiled to llvm should
**     demonstrate how rust vectors are implemented.
*/

extern crate time;
use time::PreciseTime;

const SIZE : usize = 128;

fn main() {
    let mut a : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];
    let mut sum : usize = 0;

    let start = PreciseTime::now();
    a[56][32] = 1.0;
    for i in a {
        for j in i {
            for _ in 0..1000 {
                sum = sum + 1 + j as usize;
            }
        }
    }
    let stop = PreciseTime::now();

    let elapsed = start.to(stop).to_string();
    println!("{}", &elapsed[2..]);
    eprintln!("{}", sum);
}
