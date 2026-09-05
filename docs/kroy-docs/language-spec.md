# Kryo Language Specification

## Overview

Kryo is a systems programming language designed for low-level control with memory safety guarantees. It combines the explicitness of C, the safety of Rust, and the simplicity of Zig, while introducing innovative features that differentiate it from existing systems languages.

## Key Innovations

### 1. Explicit Memory Domains
Kryo introduces explicit memory domains with static verification, going beyond traditional memory safety:

- **Stack<T>**: Automatic stack memory with known lifetime
- **Heap<T>**: Manual heap memory with explicit ownership
- **Persistent<T>**: Memory-mapped files and persistent storage
- **GPU<T>**: GPU memory with explicit transfer semantics
- **Shared<T>**: Inter-process shared memory
- **Distributed<T>**: Network-accessible distributed memory

Each domain has specific operations verified at compile-time, preventing invalid memory access across domains.

### 2. Capacity-Aware Types
Kryo provides types with explicit capacity bounds and bounded arithmetic:

- **Array<T, N>**: Fixed-size arrays with compile-time size
- **BoundedVec<T, C>**: Vectors with maximum capacity C
- **Bounded<T, MIN, MAX>**: Integers with guaranteed range
- Overflow is impossible by type system construction
- Compile-time verification of capacity constraints

### 3. Resource Types
Kryo introduces resource types with automatic lifecycle tracking:

- **Resource<Kind, State, Payload>**: Generic resource with state machine
- States: Open, InUse, Closed
- Compile-time verification of resource lifecycle
- Prevents resource leaks through type system

### 4. Compile-Time Contracts
Kryo supports preconditions and postconditions on functions:

- **requires**: Preconditions that must hold before function execution
- **ensures**: Postconditions that must hold after function execution
- Verified at compile-time through type system

## Documentation

- [Tutorial](tutorial.md) - Learn Kryo programming step by step
- [Standard Library](stdlib.md) - Complete standard library reference
- [Best Practices](best-practices.md) - Guidelines for writing better Kryo code

## File Extension

Kryo source files use the `.kyo` extension.

## Lexical Structure

### Identifiers

Identifiers start with a letter or underscore, followed by letters, digits, or underscores. Unicode identifiers are supported.

```
identifier ::= [a-zA-Z_][a-zA-Z0-9_]*
```

### Keywords

Reserved keywords:
```
fn, let, mut, if, else, while, for, return, break, continue
match, case, struct, enum, trait, impl, type, const, static
unsafe, where, use, mod, pub, crate, true, false, as, in
Stack, Heap, Persistent, GPU, Shared, Distributed
Bounded, BoundedVec, Resource, requires, ensures
extern
```

### Types

#### Primitive Types
- `bool`: Boolean values (`true`, `false`)
- `i8`, `i16`, `i32`, `i64`, `i128`: Signed integers
- `u8`, `u16`, `u32`, `u64`, `u128`: Unsigned integers
- `f32`, `f64`: Floating-point numbers
- `char`: Unicode character
- `()`: Unit type (void)

#### Memory Domain Types
- `Stack<T>`: Stack-allocated memory with automatic lifetime
- `Heap<T>`: Heap-allocated memory with explicit ownership
- `Persistent<T>`: File-backed persistent memory
- `GPU<T>`: GPU memory with explicit transfer semantics
- `Shared<T>`: Inter-process shared memory
- `Distributed<T>`: Network-accessible distributed memory

#### Capacity-Aware Types
- `Array<T, N>`: Fixed-size array with compile-time size N
- `BoundedVec<T, C>`: Vector with maximum capacity C
- `Bounded<T, MIN, MAX>`: Integer with guaranteed range [MIN, MAX]

#### Advanced Types
- `&T`: Immutable reference
- `&mut T`: Mutable reference
- `*T`: Raw pointer
- `fn(A, B) -> C`: Function type
- `Option<T>`: Optional value (`Some(T)` or `None`)
- `Result<T, E>`: Result type (`Ok(T)` or `Err(E)`)
ref, self, Self, super, crate
```

### Literals

#### Integer Literals
```
decimal ::= [0-9]+
hex ::= 0x[0-9a-fA-F]+
binary ::= 0b[01]+
octal ::= 0o[0-7]+
```

#### Float Literals
```
float ::= [0-9]+.[0-9]+([eE][+-]?[0-9]+)?
```

#### String Literals
```
string ::= "([^"\\]|\\.)*"
```

#### Character Literals
```
char ::= '([^'\\]|\\.)'
```

#### Boolean Literals
```
true, false
```

### Operators

#### Arithmetic
```
+, -, *, /, %
```

#### Comparison
```
==, !=, <, >, <=, >=
```

#### Logical
```
&&, ||, !
```

#### Bitwise
```
&, |, ^, <<, >>
```

#### Assignment
```
=, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
```

#### Other
```
., ->, ::, ?, =>, :, ;, ,, (, ), [, ], {, }, @, #
```

### Comments

```
// Single line comment
/* Multi-line comment */
/// Documentation comment
//! Module documentation comment
```

## Grammar

### Module Structure

```
module ::= {item}
item ::= 
    | function_decl
    | struct_decl
    | enum_decl
    | trait_decl
    | impl_decl
    | type_alias
    | const_decl
    | static_decl
    | use_decl
    | mod_decl
```

### Functions

```
function_decl ::= 
    "fn" identifier generics? "(" param_list ")" "->" type block
    | "unsafe" "fn" identifier generics? "(" param_list ")" "->" type block
    | "const" "fn" identifier generics? "(" param_list ")" "->" type block
    | "extern" "fn" identifier generics? "(" param_list ")" "->" type ";"

param_list ::= [param ["," param]*]
param ::= identifier ":" type
generics ::= "<" [type_param ["," type_param]*] ">"
type_param ::= identifier [":" type_bound]
type_bound ::= trait_name | trait_name "+" trait_bound
```

### Statements

```
statement ::= 
    | ";"
    | let_stmt
    | expr_stmt
    | block_stmt
    | if_stmt
    | while_stmt
    | for_stmt
    | match_stmt
    | return_stmt
    | break_stmt
    | continue_stmt
```

#### Let Statement
```
let_stmt ::= "let" ["mut"] identifier [":" type] "=" expr ";"
```

#### Expression Statement
```
expr_stmt ::= expr ";"
```

#### Block Statement
```
block_stmt ::= "{" {statement} [expr] "}"
```

#### If Statement
```
if_stmt ::= "if" expr block ["else" block | "else" "if" expr block]
```

#### While Statement
```
while_stmt ::= "while" expr block
```

#### For Statement
```
for_stmt ::= "for" identifier "in" expr block
```

#### Match Statement
```
match_stmt ::= "match" expr "{" {match_arm} "}"
match_arm ::= pattern "=>" expr [","] [";"]
```

#### Return Statement
```
return_stmt ::= "return" [expr] ";"
```

#### Break/Continue
```
break_stmt ::= "break" ";"
continue_stmt ::= "continue" ";"
```

### Expressions

```
expr ::= 
    | literal
    | identifier
    | unary_expr
    | binary_expr
    | call_expr
    | field_expr
    | index_expr
    | if_expr
    | block_expr
    | unsafe_expr
    | cast_expr
    | reference_expr
    | array_expr
    | tuple_expr
    | struct_expr
    | paren_expr
```

#### Literals
```
literal ::= integer | float | string | char | bool
```

#### Unary Expressions
```
unary_expr ::= ("-" | "!" | "*" | "&" | "&mut") expr
```

#### Binary Expressions
```
binary_expr ::= expr operator expr
```

#### Call Expressions
```
call_expr ::= expr "(" [arg_list] ")"
arg_list ::= expr ["," expr]*
```

#### Field Access
```
field_expr ::= expr "." identifier
```

#### Index Expression
```
index_expr ::= expr "[" expr "]"
```

#### If Expression
```
if_expr ::= "if" expr block ["else" block]
```

#### Unsafe Block
```
unsafe_expr ::= "unsafe" block
```

#### Cast Expression
```
cast_expr ::= expr "as" type
```

#### Reference Expressions
```
reference_expr ::= "&" expr | "&mut" expr
```

#### Array Expression
```
array_expr ::= "[" [expr ["," expr]*] "]"
```

#### Tuple Expression
```
tuple_expr ::= "(" [expr ["," expr]*] ")"
```

#### Struct Expression
```
struct_expr ::= identifier "{" [field_init ["," field_init]*] "}"
field_init ::= identifier ":" expr
```

### Types

```
type ::= 
    | primitive_type
    | identifier
    | tuple_type
    | array_type
    | reference_type
    | pointer_type
    | function_type
    | trait_type
    | qualified_type
```

#### Primitive Types
```
primitive_type ::= 
    | "bool"
    | "i8" | "i16" | "i32" | "i64" | "i128"
    | "u8" | "u16" | "u32" | "u64" | "u128"
    | "f32" | "f64"
    | "usize" | "isize"
    | "char"
```

#### Tuple Type
```
tuple_type ::= "(" [type ["," type]*] ")"
```

#### Array Type
```
array_type ::= "[" type ";" expr "]"
```

#### Reference Type
```
reference_type ::= "&" ["mut"] type
```

#### Pointer Type
```
pointer_type = "*" ["const" | "mut"] type
```

#### Function Type
```
function_type = "fn" "(" [type ["," type]*] ")" "->" type
```

#### Qualified Type
```
qualified_type ::= identifier "::" type
```

### Structs

```
struct_decl ::= 
    "struct" identifier generics? "{" [field ["," field]*] "}"
    | "struct" identifier generics? "(" [type ["," type]*] ")" ";"
    | "struct" identifier generics? ";"

field ::= identifier ":" type
```

### Enums

```
enum_decl ::= "enum" identifier generics? "{" [enum_variant ["," enum_variant]*] "}"
enum_variant ::= identifier ["(" [type ["," type]*] ")"] | identifier "{" [field ["," field]*] "}"
```

### Traits

```
trait_decl ::= "trait" identifier generics? [":" trait_bound] "{" [trait_item]* "}"
trait_item ::= function_decl
```

### Impls

```
impl_decl ::= 
    "impl" generics? identifier "for" identifier "{" [impl_item]* "}"
    | "impl" generics? identifier "{" [impl_item]* "}"

impl_item ::= function_decl
```

### Patterns

```
pattern ::= 
    | literal
    | identifier
    | wildcard
    | ref_pattern
    | mut_pattern
    | struct_pattern
    | tuple_pattern
    | enum_pattern
    | range_pattern

wildcard ::= "_"
ref_pattern ::= "ref" identifier
mut_pattern ::= "mut" identifier
struct_pattern ::= identifier "{" [pattern ["," pattern]*] "}"
tuple_pattern ::= "(" [pattern ["," pattern]*] ")"
enum_pattern ::= identifier "(" [pattern ["," pattern]*] ")"
range_pattern ::= pattern "..=" pattern
```

## Type System

### Primitive Types

| Type | Size | Description |
|------|------|-------------|
| `bool` | 1 byte | Boolean value |
| `i8` | 1 byte | Signed 8-bit integer |
| `i16` | 2 bytes | Signed 16-bit integer |
| `i32` | 4 bytes | Signed 32-bit integer |
| `i64` | 8 bytes | Signed 64-bit integer |
| `i128` | 16 bytes | Signed 128-bit integer |
| `u8` | 1 byte | Unsigned 8-bit integer |
| `u16` | 2 bytes | Unsigned 16-bit integer |
| `u32` | 4 bytes | Unsigned 32-bit integer |
| `u64` | 8 bytes | Unsigned 64-bit integer |
| `u128` | 16 bytes | Unsigned 128-bit integer |
| `f32` | 4 bytes | 32-bit floating point |
| `f64` | 8 bytes | 64-bit floating point |
| `usize` | platform | Unsigned pointer-sized integer |
| `isize` | platform | Signed pointer-sized integer |
| `char` | 4 bytes | Unicode scalar value |

## Memory Domains

### Stack Memory
```kryo
let x: Stack<i32> = Stack::new(42);
let y: Stack<Vec<f64>> = Stack::new(Vec::new());
```
- Automatic lifetime based on scope
- Zero-cost allocation
- No manual deallocation required

### Heap Memory
```kryo
let ptr: Heap<i32> = Heap::alloc(1024);
*ptr = 42;
Heap::free(ptr);
```
- Manual allocation with ownership
- Explicit deallocation
- Borrow checker ensures safety

### Persistent Memory
```kryo
let data: Persistent<u8> = Persistent::mmap("data.bin", 4096);
data[0] = 0xFF;
// Data persists across program runs
```
- Memory-mapped files
- Persistence guarantees
- Transaction support (future)

### GPU Memory
```kryo
let gpu_buffer: GPU<f32> = GPU::alloc(1024);
let cpu_data: Stack<Vec<f32>> = vec![1.0, 2.0, 3.0];
gpu_buffer.copy_from(&cpu_data)?;
let result: Stack<Vec<f32>> = gpu_buffer.copy_to()?;
```
- Explicit GPU memory management
- Verified transfer operations
- Kernel execution context

### Shared Memory
```kryo
let shared: Shared<i32> = Shared::create(4096);
*shared = 42;
// Accessible from multiple processes
```
- Inter-process communication
- Synchronization primitives
- Atomic operations support

### Distributed Memory
```kryo
let remote: Distributed<String> = Distributed::connect("node1:8080");
remote.set("key", "value")?;
let value: String = remote.get("key")?;
```
- Network-accessible memory
- Consistency models
- Fault tolerance

### Domain Transfers
```kryo
// Explicit conversion between domains
let cpu_data: Stack<Vec<f32>> = vec![1.0, 2.0];
let gpu_data: GPU<f32> = cpu_data.to_gpu()?;
let result: Stack<Vec<f32>> = gpu_data.to_cpu()?;

// Checked conversion with error handling
let safe: Bounded<i32, 0, 100> = regular_i32.to_bounded()?;
```

## Capacity-Aware Types

### Fixed-Size Arrays
```kryo
let arr: Array<i32, 10> = Array::new();
arr[0] = 1;
arr[9] = 10;
// arr[10] = 11; // COMPILE ERROR: out of bounds
```
- Compile-time bounds checking
- Zero runtime overhead
- Optimized layout

### Bounded Vectors
```kryo
let vec: BoundedVec<i32, 100> = BoundedVec::new();
vec.push(1)?;
vec.push(2)?;
// vec.push(101)?; // RUNTIME ERROR: capacity exceeded
```
- Maximum capacity verified at runtime
- No heap allocation after capacity set
- Predictable memory usage

### Bounded Integers
```kryo
let counter: Bounded<i32, 0, 100> = Bounded::new(50);
let increment: Bounded<i32, 0, 100> = Bounded::new(1);
let result: Bounded<i32, 0, 200> = counter.add(increment); // OK

let overflow = counter.add(Bounded::new(60)); // COMPILE ERROR: could exceed 100
```
- Mathematical range guarantees
- No overflow possible
- Compiler-proven bounds

### Capacity Operations
```kryo
// Safe arithmetic with bounds propagation
let a: Bounded<i32, 0, 50> = Bounded::new(25);
let b: Bounded<i32, 0, 50> = Bounded::new(25);
let sum: Bounded<i32, 0, 100> = a.add(b)?; // Result range computed

// Checked operations with explicit error handling
let c: Bounded<i32, 0, 50> = Bounded::new(40);
let d: Bounded<i32, 0, 50> = Bounded::new(40);
let checked: Result<Bounded<i32, 0, 100>> = c.checked_add(d);

// Saturating operations
let saturated: Bounded<i32, 0, 100> = a.saturating_add(Bounded::new(100)); // Returns 100
```

### Compound Types

#### Arrays
```
let arr: [i32; 5] = [1, 2, 3, 4, 5];
```

#### Tuples
```
let pair: (i32, f64) = (42, 3.14);
```

#### Slices (future)
```
let slice: &[i32] = &arr[1..3];
```

### Reference Types

#### Immutable Reference
```
let x: i32 = 10;
let ref: &i32 = &x;
```

#### Mutable Reference
```
let mut x: i32 = 10;
let mref: &mut i32 = &mut x;
```

### Pointer Types

#### Const Pointer
```
let ptr: *const i32 = &x as *const i32;
```

#### Mutable Pointer
```
let mut ptr: *mut i32 = &mut x as *mut i32;
```

## Ownership and Borrowing

### Ownership Rules

1. Each value has an owner
2. A value can have only one owner at a time
3. When the owner goes out of scope, the value is dropped
4. Moving a value transfers ownership

### Borrowing Rules

1. You can have multiple immutable references OR
2. One mutable reference
3. References must always be valid

### Examples

```kryo
fn main() -> i32 {
    let x: i32 = 10;
    let y: i32 = x;  // Copy, x is still valid
    
    let s: String = "hello".to_string();
    let t: String = s;  // Move, s is no longer valid
    
    let ref: &i32 = &x;
    let ref2: &i32 = &x;  // OK: multiple immutable refs
    
    let mut z: i32 = 20;
    let mref: &mut i32 = &mut z;
    // let mref2: &mut i32 = &mut z;  // ERROR: multiple mutable refs
    
    return 0;
}
```

## Unsafe Code

Unsafe blocks allow operations that bypass safety checks:

```kryo
unsafe {
    let ptr: *mut u8 = allocate(1024);
    *ptr = 42;
    deallocate(ptr);
}
```

Unsafe operations require explicit `unsafe` keyword.

## Generic Programming

### Generic Functions

```kryo
fn max<T>(a: T, b: T) -> T {
    if a > b { a } else { b }
}
```

### Generic Structs

```kryo
struct Container<T> {
    value: T,
}
```

### Trait Bounds

```kryo
fn process<T: Printable>(item: T) {
    item.print();
}
```

## Traits

### Defining Traits

```kryo
trait Printable {
    fn print(&self);
}
```

### Implementing Traits

```kryo
impl Printable for i32 {
    fn print(&self) {
        println!("{}", self);
    }
}
```

### Trait Objects (future)

```kryo
let obj: &dyn Printable = &42;
```

## Error Handling

### Result Type

```kryo
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

### Option Type

```kryo
enum Option<T> {
    Some(T),
    None,
}
```

### Error Propagation (future)

```kryo
fn read_file() -> Result<String, Error> {
    let content = try!(std::fs::read("file.txt"));
    Ok(content)
}
```

## Pattern Matching

```kryo
fn describe(value: Option<i32>) -> &str {
    match value {
        Some(x) if x > 10 => "large",
        Some(_) => "small",
        None => "none",
    }
}
```

## Constants and Statics

### Constants

```kryo
const MAX_SIZE: usize = 1024;
```

### Static Variables

```kryo
static COUNTER: i32 = 0;
static mut MUT_COUNTER: i32 = 0;
```

## Attributes

Attributes provide metadata:

```kryo
#[repr(C)]
struct Packet {
    id: u32,
    size: u16,
}

#[inline]
fn fast_function() -> i32 {
    42
}
```

## Standard Library (Future)

The standard library will provide:
- Collections (Vec, HashMap, etc.)
- I/O operations
- String manipulation
- Concurrency primitives
- Memory allocation
- FFI bindings

## ABI and FFI

### External Functions

```kryo
extern "C" {
    fn printf(format: *const u8, ...) -> i32;
}
```

### Calling C

```kryo
extern "C" fn kryo_function(x: i32) -> i32 {
    x * 2
}
```

## Semantics

### Evaluation Order

Expressions are evaluated left-to-right, with operators respecting precedence.

### Memory Layout

- Structs have predictable layout (C-compatible with `#[repr(C)]`)
- Arrays are contiguous in memory
- Tuples have layout compatible with C structs
- Enums use tagged union representation

### Calling Convention

Default platform calling convention, overridable with `extern "abi"`.

## Operator Precedence (Highest to Lowest)

1. Paths, field access, array indexing
2. Unary operators
3. As, multiplication, division, remainder
4. Addition, subtraction
5. Shift operators
6. Comparison operators
7. Logical AND
8. Logical OR
9. Range, assignment

## Examples

### Hello World

```kryo
fn main() -> i32 {
    println!("Hello, World!");
    return 0;
}
```

### Factorial

```kryo
fn factorial(n: i32) -> i32 {
    if n <= 1 {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() -> i32 {
    let result: i32 = factorial(5);
    println!("factorial(5) = {}", result);
    return 0;
}
```

### Struct with Methods

```kryo
struct Point {
    x: f64,
    y: f64,
}

impl Point {
    fn new(x: f64, y: f64) -> Point {
        Point { x, y }
    }
    
    fn distance(&self, other: &Point) -> f64 {
        let dx: f64 = self.x - other.x;
        let dy: f64 = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }
}

fn main() -> i32 {
    let p1: Point = Point::new(0.0, 0.0);
    let p2: Point = Point::new(3.0, 4.0);
    let dist: f64 = p1.distance(&p2);
    println!("distance = {}", dist);
    return 0;
}
```

### Generic Container

```kryo
struct Container<T> {
    value: T,
}

impl<T> Container<T> {
    fn new(value: T) -> Container<T> {
        Container { value }
    }
    
    fn get(&self) -> &T {
        &self.value
    }
}

fn main() -> i32 {
    let c: Container<i32> = Container::new(42);
    println!("value = {}", c.get());
    return 0;
}
```

## MVP Grammar (Simplified)

For the MVP, the grammar is simplified to:

```
program ::= {function_decl}

function_decl ::= "fn" "main" "(" ")" "->" "i32" block

block ::= "{" {statement} expr "}"

statement ::= 
    | "let" identifier ":" type "=" expr ";"
    | expr ";"

expr ::= 
    | literal
    | identifier
    | binary_expr
    | call_expr

binary_expr ::= expr ("+" | "-" | "*" | "/") expr

literal ::= integer | float

type ::= "i32" | "f64" | "bool"
```

This simplified grammar allows compilation of basic arithmetic functions for the MVP.
