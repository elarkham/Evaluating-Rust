/* See LICENSE file for copyright and license details
** Author: Ethan Larkham
** Description:
**  Concurrently compute an approximation of pi
*/

use std::thread;
use std::cmp::Ordering;
use std::sync::mpsc;

/* Number of intervals to divide the area beneath the curve in [0,1] into */
const INTERVALS : usize = 50000000;

/* Width of an interval */
const WIDTH: f64 = 1.0 / INTERVALS;

/* Number of threads being used*/
const THREAD: usize = 4;

fn main() {
    let (tx, rx) = mpsc::channel();

    // Number of intervals each thread will calculate
    let mut chunk = INTERVALS / THREADS;

    // Thread index less than split have one extra interval
    let mut split = INTERVALS % THREADS;

    if split == 0 {
        split = THREADS;
        chunk -= 1;
    }

    for i in (0..).take(THREADS) {
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
    for _ in (0..).take(THREADS) {
        sum += rx.recv().unwrap();
    }
    sum *= 1.0 / INTERVALS as f64;

    println!("pi equals {}", sum);
}
