// leftpad.cpp
// The infamous leftpad function, rewritten in Rust.
// Author: Dénes Fintha
// Year: 2022
// -------------------------------------------------------------------------- //

// Implementation

enum LeftpadFiller {
    Character(char),
    String(String),
    Number(i64),
}

fn leftpad(string: &str, length: isize, fill: LeftpadFiller) -> String {
    let old_length = string.len() as isize;
    let new_length = if length < old_length {
        old_length
    } else {
        length
    };
    let pad_length = new_length - old_length;

    let mut result = String::with_capacity(new_length as usize);

    let fill = match fill {
        LeftpadFiller::Character(c) => c,
        LeftpadFiller::String(s) => s.chars().nth(0).unwrap_or(' '),
        LeftpadFiller::Number(n) => n.to_string().chars().nth(0).unwrap_or(' '),
    };

    for _ in 0..pad_length {
        result.push(fill);
    }
    result.push_str(string);
    result
}

// Demonstration

macro_rules! test {
    ($desc:expr, $string:expr, $len:expr, $fill:expr, $expected:expr) => {
        println!("Testing if {}", $desc);
        let subject = leftpad($string, $len, $fill);
        println!("  Expected: '{}'", $expected);
        println!("       Got: '{}'", subject);
        let verdict = if subject == String::from($expected) {
            "Passed"
        } else {
            "Failed"
        };
        println!("    Result: {}\n", verdict);
    };
}

fn main() {
    test!(
        "padding an empty string to a length to 0 results in an empty string",
        "",
        0,
        LeftpadFiller::Character(' '),
        ""
    );
    test!(
        "padding to a shorter length results in the same string",
        "foo",
        2,
        LeftpadFiller::Character(' '),
        "foo"
    );
    test!(
        "padding to a negative length results in the same string",
        "foo",
        -2,
        LeftpadFiller::Character(' '),
        "foo"
    );
    test!(
        "padding a non-empty string to its length results in the same string",
        "foo",
        3,
        LeftpadFiller::Character(' '),
        "foo"
    );
    test!(
        "padding to a longer string with a single character fills to the left",
        "foo",
        4,
        LeftpadFiller::Character('_'),
        "_foo"
    );
    test!(
        "padding to a longer string with a number fills with its first digit",
        "foo",
        4,
        LeftpadFiller::Number(12),
        "1foo"
    );
    test!(
        "padding to a longer string with a negative number fills with -",
        "foo",
        4,
        LeftpadFiller::Number(-12),
        "-foo"
    );
    test!(
        "padding to a longer string with a string fills with its first char",
        "foo",
        4,
        LeftpadFiller::String("abc".to_string()),
        "afoo"
    );
}
