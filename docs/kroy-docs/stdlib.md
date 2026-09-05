# Kryo Standard Library Documentation

## Overview

The Kryo standard library provides essential data structures, algorithms, and utilities for common programming tasks. This document covers the core modules and their key functions.

## Core Types

### Option<T>

Represents an optional value that can be either `Some(T)` or `None`.

```kryo
enum Option<T> {
    Some(T),
    None,
}
```

**Usage:**
```kryo
fn divide(a: i32, b: i32) -> Option<i32> {
    if b == 0 {
        return Option::None;
    }
    return Option::Some(a / b);
}
```

**Methods:**
- `is_some()`: Returns true if the value is `Some`
- `is_none()`: Returns true if the value is `None`
- `unwrap()`: Returns the contained value or panics if `None`
- `unwrap_or(default)`: Returns the contained value or the default

### Result<T, E>

Represents a value that can be either success (`Ok(T)`) or error (`Err(E)`).

```kryo
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

**Usage:**
```kryo
fn parse_int(s: String) -> Result<i32, String> {
    // Parse logic here
    return Result::Ok(42);
}
```

**Methods:**
- `is_ok()`: Returns true if the value is `Ok`
- `is_err()`: Returns true if the value is `Err`
- `unwrap()`: Returns the contained value or panics if `Err`
- `unwrap_or(default)`: Returns the contained value or the default

## Data Structures

### String

A growable, heap-allocated string type.

```kryo
struct String {
    // Internal implementation
}
```

**Methods:**
- `new()`: Creates a new empty string
- `from(s: &str)`: Creates a string from a string slice
- `push(c: char)`: Appends a character
- `push_str(s: &str)`: Appends a string slice
- `len()`: Returns the length in bytes
- `is_empty()`: Returns true if the string is empty
- `clone()`: Creates a deep copy

**Usage:**
```kryo
let mut s: String = String::new();
s.push('H');
s.push_str("ello");
```

### Vec<T>

A growable array type.

```kryo
struct Vec<T> {
    // Internal implementation
}
```

**Methods:**
- `new()`: Creates a new empty vector
- `push(value: T)`: Appends an element
- `pop() -> Option<T>`: Removes and returns the last element
- `len() -> usize`: Returns the number of elements
- `is_empty() -> bool`: Returns true if the vector is empty
- `get(index: usize) -> Option<&T>`: Returns a reference to the element at index
- `clone()`: Creates a deep copy

**Usage:**
```kryo
let mut v: Vec<i32> = Vec::new();
v.push(10);
v.push(20);
let value = v.get(0);
```

### HashMap<K, V>

A hash map implementation for key-value storage.

```kryo
struct HashMap<K, V> {
    // Internal implementation
}
```

**Methods:**
- `new()`: Creates a new empty hash map
- `insert(key: K, value: V)`: Inserts a key-value pair
- `get(key: &K) -> Option<&V>`: Returns a reference to the value for the key
- `remove(key: &K) -> Option<V>`: Removes and returns the value for the key
- `len() -> usize`: Returns the number of entries
- `is_empty() -> bool`: Returns true if the map is empty

**Usage:**
```kryo
let mut map: HashMap<String, i32> = HashMap::new();
map.insert("one", 1);
map.insert("two", 2);
let value = map.get("one");
```

## Memory Domain Types

### Stack<T>

Stack-allocated memory with automatic lifetime management.

```kryo
struct Stack<T> {
    // Internal implementation
}
```

**Methods:**
- `new(value: T) -> Stack<T>`: Creates a new stack-allocated value
- `get() -> T`: Returns the value
- `set(value: T)`: Sets the value

**Usage:**
```kryo
let stack_data: Stack<i32> = Stack::new(42);
let result = stack_data.get();
```

### Heap<T>

Heap-allocated memory with explicit ownership.

```kryo
struct Heap<T> {
    // Internal implementation
}
```

**Methods:**
- `new(value: T) -> Heap<T>`: Allocates a new value on the heap
- `get() -> T`: Returns the value
- `borrow() -> &T`: Returns an immutable reference
- `borrow_mut() -> &mut T`: Returns a mutable reference

**Usage:**
```kryo
let heap_data: Heap<i32> = Heap::new(100);
let value = heap_data.get();
```

### Persistent<T>

File-backed persistent memory.

```kryo
struct Persistent<T> {
    // Internal implementation
}
```

**Methods:**
- `open(path: String) -> Persistent<T>`: Opens a file-backed value
- `read() -> T`: Reads the value
- `write(value: T)`: Writes the value

**Usage:**
```kryo
let persistent_data: Persistent<i32> = Persistent::open("data.bin");
let value = persistent_data.read();
```

### GPU<T>

GPU memory with explicit transfer semantics.

```kryo
struct GPU<T> {
    // Internal implementation
}
```

**Methods:**
- `allocate(size: usize) -> GPU<T>`: Allocates GPU memory
- `write(value: T)`: Writes a value to GPU memory
- `read() -> T`: Reads a value from GPU memory

**Usage:**
```kryo
let gpu_data: GPU<i32> = GPU::allocate(1024);
gpu_data.write(42);
let result = gpu_data.read();
```

### Shared<T>

Inter-process shared memory.

```kryo
struct Shared<T> {
    // Internal implementation
}
```

**Methods:**
- `open(region: String) -> Shared<T>`: Opens a shared memory region
- `get() -> T`: Reads the value
- `set(value: T)`: Sets the value
- `increment()`: Atomically increments the value

**Usage:**
```kryo
let shared_data: Shared<i32> = Shared::open("shared_mem");
shared_data.increment();
let result = shared_data.get();
```

### Distributed<T>

Network-accessible distributed memory.

```kryo
struct Distributed<T> {
    // Internal implementation
}
```

**Methods:**
- `connect(address: String) -> Distributed<T>`: Connects to a distributed memory node
- `read() -> T`: Reads the value
- `write(value: T)`: Writes the value

**Usage:**
```kryo
let dist_data: Distributed<i32> = Distributed::connect("cluster.node1");
let value = dist_data.read();
```

## Capacity-Aware Types

### Array<T, N>

Fixed-size array with compile-time size.

```kryo
type Array<T, N> = [T; N];
```

**Usage:**
```kryo
let arr: [i32; 5] = [1, 2, 3, 4, 5];
let first = arr[0];
```

### BoundedVec<T, C>

Vector with maximum capacity C.

```kryo
struct BoundedVec<T, C> {
    // Internal implementation
}
```

**Methods:**
- `new() -> BoundedVec<T, C>`: Creates a new bounded vector
- `push(value: T)`: Appends an element (fails if at capacity)
- `len() -> usize`: Returns the number of elements
- `capacity() -> usize`: Returns the maximum capacity

**Usage:**
```kryo
let vec: BoundedVec<i32, 100> = BoundedVec::new();
vec.push(10);
vec.push(20);
```

### Bounded<T, MIN, MAX>

Integer with guaranteed range [MIN, MAX].

```kryo
struct Bounded<T, MIN, MAX> {
    // Internal implementation
}
```

**Methods:**
- `new(value: T) -> Bounded<T, MIN, MAX>`: Creates a bounded integer
- `get() -> T`: Returns the value
- `add(other: Bounded<T, MIN, MAX>) -> Bounded<T, MIN, MAX>`: Safe addition

**Usage:**
```kryo
let x: Bounded<i32, 0, 100> = Bounded::new(50);
let y: Bounded<i32, 0, 100> = Bounded::new(25);
let result = x + y; // Cannot overflow
```

## Resource Types

### Resource<Kind, State, Payload>

Resource with automatic lifecycle tracking.

```kryo
struct Resource<Kind, State, Payload> {
    // Internal implementation
}
```

**Lifecycle States:**
- `Open`: Resource is open and available
- `InUse`: Resource is currently being used
- `Closed`: Resource is closed and unavailable

**Usage:**
```kryo
let file: File<Open> = File::open("test.txt");
let content = file.read();
file.close(); // Must close to prevent resource leak
```

## I/O Functions

### Console Output

- `print(value)`: Print value without newline
- `println(value)`: Print value with newline

**Usage:**
```kryo
print("Hello, ");
println("World!");
```

### Console Input

- `read_line() -> String`: Read a line from stdin
- `read_int() -> i32`: Read an integer from stdin
- `read_float() -> f64`: Read a float from stdin

**Usage:**
```kryo
print("Enter your name: ");
let name = read_line();
println("Hello, ");
println(name);
```

### Debugging

- `assert(condition)`: Assert that condition is true
- `panic(message)`: Panic with message

**Usage:**
```kryo
assert(x > 0);
if error {
    panic("Something went wrong!");
}
```

### Unsafe Operations

- `unsafe { ... }`: Execute unsafe code block
- `ptr::read<T>(ptr: *const T) -> T`: Read from raw pointer
- `ptr::write<T>(ptr: *mut T, value: T)`: Write to raw pointer

## FFI

### extern "C"

Foreign Function Interface for C interoperability.

```kryo
extern "C" {
    fn puts(s: *const u8) -> i32;
    fn malloc(size: usize) -> *mut u8;
    fn free(ptr: *mut u8);
}
```

**Usage:**
```kryo
unsafe {
    let msg: *const u8 = "Hello from Kryo!";
    puts(msg);
}
```

## Notes

- All standard library types are designed for zero-cost abstractions
- Memory domain types provide compile-time verification of memory safety
- Capacity-aware types prevent overflow and bounds errors
- Resource types prevent resource leaks through lifecycle tracking
- FFI requires explicit unsafe blocks for safety

## Future Enhancements

- More comprehensive standard library coverage
- Async/await support
- Iterator patterns
- Concurrency primitives
- File I/O utilities
- Networking primitives
- Cryptographic primitives
