/* See LICENSE file for copyright and license details
** Authors:
**     Ethan Larkham
** Description:
**  Analyze a set of files for most commonly used words that are atleast
**  6 characters long and no more than 50 letters long.
*/

use std::thread;
use std::cmp::Ordering;
use std::sync::mpsc;
use std::env;
use std::process;
use std::collections::HashMap;

const MAX_WORD : usize = 50;
const MIN_WORD : usize = 6;

fn usage() {
    eprint!("Usage: wordfind [FILE]...\n");
}

fn main() {
    let (tx, rx) = mpsc::channel();

    let args: Vec<String> = env::args().collect();

    if args.len() == 1 {
        usage();
        process::exit(-1);
    }

    for file in 1..args.len() {
        let tx = tx.clone();
        thread::spawn(move || {

            tx.send(s).unwrap();
      });
    }

    let mut word_cout = HashMap::new();
    for recv in rx {
        let word  = rx.recv().unwrap();
        match word_count.get(word) {
            Some(count) => word_count.insert(word, count + 1),
            None => word_count.insert(word, 1),
        };

    }

    //eprintln!("pi equals {}", sum);
}
