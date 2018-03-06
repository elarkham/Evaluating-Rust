/* See LICENSE file for copyright and license details
** Authors:
**     Ethan Larkham
** Description:
**  Analyze a set of files for most commonly used words that are atleast
**  6 characters long and no more than 50 letters long.
*/
extern crate time;

use time::PreciseTime;
use std::thread;
use std::cmp::Ordering;
use std::sync::mpsc;
use std::env;
use std::process;
use std::collections::HashMap;
use std::io::{BufRead, BufReader};
use std::ascii::AsciiExt;
use std::fs::File;

const MAX_WORD : usize = 50;
const MIN_WORD : usize = 6;

fn usage() {
    eprint!("Usage: wordfind [FILE]...\n");
}

fn main() {
    let (tx, rx) = mpsc::channel();

    let args: Vec<String> = env::args().collect();
    let file_count = args.len() - 1;

    if file_count <= 0 {
        usage();
        process::exit(-1);
    }
    let start_time = PreciseTime::now();

    // Iterate over all words in each file
    let mut all = 0;
    for i in 1..args.len() {
        let tx = tx.clone();
        let file_name = args[i].clone();
        all = all | (1 << (i - 1));
        //println!("File : {} : {}", i, file_name);
        thread::spawn(move || {
            let id = 1 << (i - 1);
            let file = BufReader::new(File::open(file_name).unwrap());
            file.lines()
                .flat_map(|line| {
                    line.unwrap()
                        .split(|c: char| !c.is_alphanumeric())
                        .map(|string| string.to_string())
                        .map(|string| string.to_lowercase())
                        .collect::<Vec<String>>()
                })
                .filter(|word| {
                    let len = word.len();
                    (len < MAX_WORD) && (len > MIN_WORD)
                })
                .filter(|word| {
                    //for c in word.chars() {
                    //    if !c.is_ascii() || !c.is_alphanumeric() {
                    //        return false;
                    //    }
                    //}
                    return true;
                })
                .for_each(|word| {
                    tx.send((word, id)).unwrap();
                });
            tx.send(("".to_string(), -1)).unwrap();
        });
    }

    // Receive words collected from threads and insert into hash table
    let mut table = HashMap::new();
    let mut active_threads = file_count;
    for recv in &rx {
        let (word, id)  = recv;
        if id < 0 {
            active_threads = active_threads - 1;
            if active_threads <= 0 {
                break;
            }
        }
        let get : Option<i32> = table.get(&word).cloned();
        let mask : i32 = match get {
            Some(mask) => mask.clone(),
            None => 0,
        };

        table.insert(word.clone(), (id.clone() | mask));
    }

    // Find most common word
    let mut max : String = "".to_string();
    for (&ref word, &mask) in table.iter() {
        if (mask == all) && (word.len() > max.len()){
            max = word.clone();
        }
    }
    let stop_time = PreciseTime::now();
    let elapsed = start_time.to(stop_time).to_string();
    println!("{}", &elapsed[2..]);

    eprintln!("{}", max);
}
