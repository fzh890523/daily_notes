fn main() {
    println!("Hello, world!");
}

/// This module contains tests
mod test {
    // ...
}

fn main() {
    println!("Hello, world!");
}

fn print_sum(a: i8, b: i8) {
    println!("sum is {}", a + b);
}


fn plus_one(a :i32) -> i32 {
    return a + 1;
}

fn plus_two(a: i32) -> i32 {
    return a + 2;
}

let b = plus_one;
let c = b(5);

let b: fn(i32) -> i32 = plus_one;
let c = b(5);
