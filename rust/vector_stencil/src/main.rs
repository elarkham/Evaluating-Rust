/* See LICENSE file for copyright and license details
** Authors:
**     Ethan Larkham
**     Todd Gaunt
** Description:
**     This is a sample file that when compiled to llvm should
**     demonstrate how rust vectors are implemented.
*/

// extern crate time;
// use time::PreciseTime;

const SIZE : usize = 128;

fn main() {
    let mut a : Vec<Vec<f64>> = vec![vec![0.0; SIZE]; SIZE];
    let mut sum : f64 = 0.0;

    // let start = PreciseTime::now();
    a[56][32] = 1.0;
    for i in 1..127 {
        for j in 1..127 {
            for _ in 0..10000 {
                sum = sum + a[i][j + 1] + a[i][j - 1] + a[i + 1][j] + a[i - 1][j];
                //println!("{}", sum);
            }
        }
    }
    // let stop = PreciseTime::now();

    // let elapsed = start.to(stop).to_string();
    // println!("{}", &elapsed[2..]);
    eprintln!("{}", sum);
}
