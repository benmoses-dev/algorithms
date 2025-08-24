use std::io::Stdin;
use std::io::{self, Write};
use std::iter::Peekable;
use std::str::Chars;
use std::time::Instant;

fn main() {
    let stdin: Stdin = io::stdin();
    let mut buffer: String = String::new();
    let valid_choices = [1, 2, 3, 4];
    let descriptions = [
        "1: Safe",
        "2: Unsafe (Ensure text is raw ASCII)",
        "3: Pointers (Ensure text is raw ASCII)",
        "4: Benchmark (Ensure text is raw ASCII)",
    ];

    let choice: u8 = get_choice(&mut buffer, &stdin, &valid_choices, &descriptions);
    let mut text: String = get_text(&mut buffer, &stdin);
    match choice {
        1 => {
            reverse_string(&mut text);
        }
        2 => {
            reverse_string_unsafe(&mut text);
        }
        3 => {
            reverse_string_pointers(&mut text);
        }
        4 => {
            benchmark(&text);
        }
        _ => {
            println!("Please enter a valid choice!");
        }
    }
}

fn get_text(buffer: &mut String, stdin: &Stdin) -> String {
    buffer.clear();
    print!("Enter a string of text > ");
    io::stdout().flush().expect("Could not flush stdout!");
    stdin.read_line(buffer).expect("Failed to read line");
    buffer.trim_end().to_owned()
}

fn get_choice(
    buffer: &mut String,
    stdin: &Stdin,
    valid_choices: &[u8],
    descriptions: &[&str],
) -> u8 {
    loop {
        buffer.clear();
        read_choice(buffer, stdin, descriptions);
        let choice: u8 = match buffer.trim().parse() {
            Ok(value) => value,
            Err(_) => {
                println!("Failed to parse as int. Try again!\n");
                continue;
            }
        };
        if !valid_choices.contains(&choice) {
            println!("Please make a valid choice\n");
            continue;
        }
        return choice;
    }
}

fn read_choice(buffer: &mut String, stdin: &Stdin, descriptions: &[&str]) -> () {
    println!("Enter your choice of algorithm:");
    for description in descriptions {
        println!("{description}");
    }
    print!("Choice > ");
    io::stdout().flush().expect("Could not flush stdout!");
    stdin.read_line(buffer).expect("Error reading line!");
}

fn reverse_string(buffer: &mut String) {
    let now = Instant::now();

    // Create iterator over the String
    let mut buf_iter: Peekable<Chars<'_>> = buffer.chars().peekable();
    let punctuation: [char; 7] = [' ', '.', ',', '!', '?', ';', ':'];
    let mut peeked: &char;
    let mut next: char;
    let mut text: String = String::new();

    loop {
        peeked = match buf_iter.peek() {
            Some(val) => val,
            None => {
                break;
            }
        };
        if punctuation.contains(peeked) {
            // Add the punctuation mark to the result
            next = buf_iter.next().unwrap();
            text.push(next);
            continue;
        }
        // We are at the start of the next word
        let mut temp: String = String::new();
        loop {
            peeked = match buf_iter.peek() {
                Some(val) => val,
                None => {
                    break;
                }
            };
            if punctuation.contains(peeked) {
                break;
            }
            next = buf_iter.next().unwrap();
            temp.push(next);
        }
        temp = temp.chars().rev().collect();
        text.push_str(&temp);
    }

    let elapsed = now.elapsed();

    println!("\nYour text is > {text}");
    println!(
        "The safe algorithm took {} seconds",
        elapsed.as_secs_f64()
    );
}

fn reverse_string_unsafe(buffer: &mut String) {
    let now = Instant::now();

    // Convert to byte array and create indexes
    let bytes: &mut [u8] = unsafe { buffer.as_bytes_mut() };
    let mut head: usize = 0;
    let mut tail: usize = 0;
    let punctuation: [u8; 7] = [b' ', b'.', b',', b'!', b'?', b';', b':'];

    while head < bytes.len() {
        // Find the next word
        while head < bytes.len() && !punctuation.contains(&bytes[head]) {
            head += 1;
        }
        // End of word
        let mut end: usize = head.saturating_sub(1);
        while head < bytes.len() && punctuation.contains(&bytes[head]) {
            head += 1;
        }
        // Reverse the word
        while tail < end {
            let temp: u8 = bytes[tail];
            bytes[tail] = bytes[end];
            bytes[end] = temp;
            tail += 1;
            end -= 1;
        }
        // Set tail to the beginning of next word
        tail = head;
    }

    let elapsed = now.elapsed();

    println!("\nYour text is > {buffer}");
    println!(
        "The unsafe algorithm took {} seconds",
        elapsed.as_secs_f64()
    );
}

fn reverse_string_pointers(buffer: &mut String) {
    let now = Instant::now();

    // Convert to byte array and create pointers
    let bytes: &mut [u8] = unsafe { buffer.as_bytes_mut() };
    let mut head: *mut u8 = bytes.as_mut_ptr();
    let mut tail: *mut u8 = bytes.as_mut_ptr();
    let last: *mut u8 = &mut bytes[bytes.len() - 1];
    let punctuation: [u8; 7] = [b' ', b'.', b',', b'!', b'?', b';', b':'];

    while head <= last {
        // Find the next word
        while head <= last && !punctuation.contains(unsafe { &*head }) {
            head = unsafe { head.offset(1) };
        }
        // End of word
        let mut end: *mut u8 = unsafe { head.offset(-1) };
        // Go to beginning of next word
        while head <= last && punctuation.contains(unsafe { &*head }) {
            head = unsafe { head.offset(1) };
        }
        // Reverse the word
        while tail < end {
            let temp: u8 = unsafe { *tail };
            unsafe { *tail = *end };
            unsafe { *end = temp };
            tail = unsafe { tail.offset(1) };
            end = unsafe { end.offset(-1) };
        }
        // Set tail to the beginning of next word
        tail = head;
    }

    let elapsed = now.elapsed();

    println!("\nYour text is > {buffer}");
    println!(
        "The (very) unsafe pointer algorithm took {} seconds",
        elapsed.as_secs_f64()
    );
}

fn benchmark(buffer: &String) {
    let mut one: String = buffer.to_owned();
    let mut two: String = buffer.to_owned();
    let mut three: String = buffer.to_owned();

    for _ in 1..=6 {
        reverse_string(&mut one);
        one = buffer.to_owned();
    }

    for _ in 1..=6 {
        reverse_string_unsafe(&mut two);
        two = buffer.to_owned();
    }

    for _ in 1..=6 {
        reverse_string_pointers(&mut three);
        three = buffer.to_owned();
    }
}
