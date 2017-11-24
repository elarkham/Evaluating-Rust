/* See LICENSE file for copyright and license details
** Authors:
**     Ethan Larkham
**     Todd Gaunt
** Description:
**  Concurrently compute an approximation of pi
*/

use std::thread;
use std::cmp::Ordering;
use std::sync::mpsc;
use std::env;
use std::process;
extern crate time;
use time::PreciseTime;

/* Number of intervals to divide the area beneath the curve in [0,1] into */
const INTERVALS : usize = 50000000;

/* Width of an interval */
const WIDTH: f64 = 1.0 / INTERVALS as f64;

fn usage() {
    eprint!("Usage: pi [NUMBER_OF_THREADS]\n");
}

fn main() {
    let (tx, rx) = mpsc::channel();

    let args: Vec<String> = env::args().collect();

    if 2 != args.len() {
        usage();
        process::exit(-1);
    }

    // Number of threads to use
    let n_thread = args[1].parse::<usize>().unwrap();

    // Number of intervals each thread will calculate
    let mut chunk = INTERVALS / n_thread;

    // Thread index less than split have one extra interval
    let mut split = INTERVALS % n_thread;

    if split == 0 {
        split = n_thread;
        chunk -= 1;
    }

    let start = PreciseTime::now();
    for i in (0..).take(n_thread) {
      let tx = tx.clone();
      thread::spawn(move || {
          // first interval to be processed
          let low : usize = match split.cmp(&i) {
              Ordering::Equal => (split * (chunk + 1)) + (i - split) * split,
              _               => i * (chunk + 1),
          };

          // first interval *not* to be processed
          let high : usize = low + chunk + 1;

          // sum for intervals being processed
          let mut s = 0.0;

          let mut x = ((low as f64) + 0.5) * WIDTH;
          for _ in low..high {
              s += 4.0 / ((x * x) + 1.0);
              x += WIDTH;
          }
          tx.send(s).unwrap();
      });
    }

    let mut sum = 0.0;
    for _ in (0..).take(n_thread) {
        sum += rx.recv().unwrap();
    }
    sum *= 1.0 / INTERVALS as f64;
    let stop = PreciseTime::now();

    println!("{}", start.to(stop));
    eprintln!("pi equals {}", sum);
}
