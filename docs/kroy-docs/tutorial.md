# Kryo Programming Tutorial

## Introduction

Welcome to the Kryo programming language tutorial! This guide will teach you the basics of Kryo, from simple programs to advanced features like memory domains and capacity-aware types.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Your First Program](#your-first-program)
3. [Variables and Types](#variables-and-types)
4. [Functions](#functions)
5. [Control Flow](#control-flow)
6. [Structs and Enums](#structs-and-enums)
7. [Memory Domains](#memory-domains)
8. [Capacity-Aware Types](#capacity-aware-types)
9. [Error Handling](#error-handling)
10. [FFI with C](#ffi-with-c)

## Getting Started

### Installation

First, ensure you have Rust and LLVM 16 installed on your system.

### Building the Compiler

```bash
cd kryo
cargo build --release
```

The compiled compiler will be available at `target/release/kryo.exe`.

### Running Your First Program

Create a file named `hello.kyo`:

```kryo
fn main() -> i32 {
    return 0;
}
```

Compile and run:

```bash
kryo build hello.kyo
```

## Your First Program

Let's start with a simple program that does arithmetic:

```kryo
fn main() -> i32 {
    let x: i32 = 10;
    let y: i32 = 20;
    let sum: i32 = x + y;
    return sum;
}
```

**Explanation:**
- `fn main() -> i32`: Defines the entry point function that returns an integer
- `let x: i32 = 10`: Declares a variable `x` of type `i32` with value 10
- `let y: i32 = 20`: Declares a variable `y` of type `i32` with value 20
- `let sum: i32 = x + y`: Adds the two numbers
- `return sum`: Returns the result

## Variables and Types

### Basic Types

Kryo has several primitive types:

```kryo
fn types_example() -> i32 {
    let integer: i32 = 42;
    let floating: f64 = 3.14;
    let boolean: bool = true;
    let character: char = 'A';
    return integer;
}
```

### Mutable Variables

Use `mut` to make a variable mutable:

```kryo
fn mutable_example() -> i32 {
    let mut x: i32 = 10;
    x = 20; // Can be reassigned
    return x;
}
```

### Type Inference

Kryo can infer types in many cases:

```kryo
fn inference_example() -> i32 {
    let x = 10; // Type inferred as i32
    let y = 3.14; // Type inferred as f64
    return x;
}
```

## Functions

### Function Definition

```kryo
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn main() -> i32 {
    let result = add(10, 20);
    return result;
}
```

### Functions Without Return Value

Use `()` for functions that don't return a value:

```kryo
fn print_hello() -> () {
    // Print logic here
}

fn main() -> i32 {
    print_hello();
    return 0;
}
```

## Control Flow

### If Statements

```kryo
fn if_example(x: i32) -> i32 {
    if x > 10 {
        return 1;
    } else {
        return 0;
    }
}
```

### While Loops

```kryo
fn while_example() -> i32 {
    let mut sum: i32 = 0;
    let mut i: i32 = 0;
    while i < 10 {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}
```

### Break and Continue

```kryo
fn loop_control() -> i32 {
    let mut sum: i32 = 0;
    let mut i: i32 = 0;
    while i < 10 {
        if i == 5 {
            i = i + 1;
            continue; // Skip 5
        }
        if i == 8 {
            break; // Stop at 8
        }
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}
```

## Structs and Enums

### Structs

```kryo
struct Point {
    x: f64,
    y: f64,
}

fn struct_example() -> f64 {
    let p: Point = Point { x: 3.0, y: 4.0 };
    let distance = p.x + p.y;
    return distance;
}
```

### Enums

```kryo
enum Option<T> {
    Some(T),
    None,
}

fn enum_example() -> i32 {
    let maybe_value: Option<i32> = Option::Some(42);
    match maybe_value {
        Option::Some(value) => return value,
        Option::None => return 0,
    }
}
```

## Memory Domains

Kryo provides explicit memory domains for different use cases:

### Stack Memory

```kryo
fn stack_example() -> i32 {
    let stack_data: Stack<i32> = Stack::new(42);
    let result = stack_data.get();
    return result;
}
```

### Heap Memory

```kryo
fn heap_example() -> i32 {
    let heap_data: Heap<i32> = Heap::new(100);
    let value = heap_data.get();
    return value;
}
```

### Persistent Memory

```kryo
fn persistent_example() -> i32 {
    let persistent_data: Persistent<i32> = Persistent::open("data.bin");
    let value = persistent_data.read();
    return value;
}
```

### GPU Memory

```kryo
fn gpu_example() -> i32 {
    let gpu_data: GPU<i32> = GPU::allocate(1024);
    gpu_data.write(42);
    let result = gpu_data.read();
    return result;
}
```

### Shared Memory

```kryo
fn shared_example() -> i32 {
    let shared_data: Shared<i32> = Shared::open("shared_mem");
    shared_data.increment();
    let result = shared_data.get();
    return result;
}
```

### Distributed Memory

```kryo
fn distributed_example() -> i32 {
    let dist_data: Distributed<i32> = Distributed::connect("cluster.node1");
    let value = dist_data.read();
    return value;
}
```

## Capacity-Aware Types

### Arrays

```kryo
fn array_example() -> i32 {
    let arr: [i32; 5] = [1, 2, 3, 4, 5];
    let first = arr[0];
    return first;
}
```

### Bounded Vectors

```kryo
fn boundedvec_example() -> i32 {
    let vec: BoundedVec<i32, 100> = BoundedVec::new();
    vec.push(10);
    vec.push(20);
    let len = vec.len();
    return len;
}
```

### Bounded Integers

```kryo
fn bounded_example() -> i32 {
    let x: Bounded<i32, 0, 100> = Bounded::new(50);
    let y: Bounded<i32, 0, 100> = Bounded::new(25);
    let result = x + y; // Cannot overflow
    return result.get();
}
```

## Error Handling

### Option Type

```kryo
fn safe_divide(a: i32, b: i32) -> Option<i32> {
    if b == 0 {
        return Option::None;
    }
    return Option::Some(a / b);
}

fn handle_option() -> i32 {
    let result = safe_divide(100, 5);
    match result {
        Option::Some(value) => return value,
        Option::None => return 0,
    }
}
```

### Result Type

```kryo
fn parse_number(s: String) -> Result<i32, String> {
    // Parse logic here
    return Result::Ok(42);
}

fn handle_result() -> i32 {
    let result = parse_number("42");
    match result {
        Result::Ok(value) => return value,
        Result::Err(_) => return 0,
    }
}
```

## FFI with C

### Calling C Functions

```kryo
extern "C" {
    fn puts(s: *const u8) -> i32;
}

unsafe fn ffi_example() -> i32 {
    let msg: *const u8 = "Hello from Kryo!";
    return puts(msg);
}
```

### Unsafe Block Requirement

Calls to `extern "C"` functions must be inside an `unsafe` block:

```kryo
extern "C" {
    fn abs(x: i32) -> i32;
}

fn safe_ffi_call() -> i32 {
    unsafe {
        let x: i32 = -42;
        let result = abs(x);
        return result;
    }
}
```

## Best Practices

1. **Use memory domains appropriately**: Choose the right memory domain for your use case
2. **Prefer capacity-aware types**: Use bounded types when possible to prevent overflow
3. **Handle errors explicitly**: Use `Option` and `Result` for error handling
4. **Keep unsafe blocks minimal**: Only use `unsafe` when absolutely necessary
5. **Document your code**: Use comments to explain complex logic

## Next Steps

- Read the [Language Specification](language-spec.md) for complete syntax details
- Check the [Standard Library Documentation](stdlib.md) for available functions
- Explore the [examples](../examples/) directory for more sample programs
- Review the [Architecture Documentation](architecture.md) for compiler internals

## Common Pitfalls

1. **Forgetting `unsafe` for FFI**: Always wrap `extern "C"` calls in `unsafe` blocks
2. **Ignoring capacity limits**: Be aware of capacity constraints in bounded types
3. **Memory domain mismatches**: Ensure you're using the correct memory domain for your use case
4. **Not handling errors**: Always handle `Option` and `Result` types appropriately
5. **Forgetting to close resources**: Always close resources to prevent leaks

## Resources

- [GitHub Repository](https://github.com/kryo-lang/kryo)
- [Language Specification](language-spec.md)
- [Standard Library](stdlib.md)
- [Architecture](architecture.md)
