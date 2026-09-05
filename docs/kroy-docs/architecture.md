# Kryo Compiler Architecture

## Overview

Kryo is a systems programming language with memory safety guarantees. The compiler follows a multi-pass architecture designed for correctness, performance, and future extensibility including AI-assisted development.

## Design Principles

1. **Modularity**: Each compilation phase is a separate, testable component
2. **Incremental Compilation**: Future support for caching and incremental builds
3. **Error Quality**: Structured, detailed diagnostics with source locations
4. **Extensibility**: Architecture designed for multiple backends and future features
5. **AI-Ready**: Compiler produces structured data for future KryoLM training

## Compilation Pipeline

```
Source (.kyo)
    ↓
Lexer (Tokenization)
    ↓
Parser (AST Generation)
    ↓
Name Resolution (Symbol Table)
    ↓
Type Checking (Type Inference & Verification)
    ↓
Ownership/Borrow Checking (Memory Safety Analysis)
    ↓
HIR (High-Level Intermediate Representation)
    ↓
MIR (Mid-Level Intermediate Representation)
    ↓
LLVM IR Generation
    ↓
Native Machine Code (via LLVM)
```

## Component Architecture

### 1. Lexer (`compiler/lexer`)

**Responsibilities:**
- Convert source text into tokens
- Handle comments, whitespace, identifiers, keywords, literals
- Track source locations for error reporting
- Support Unicode identifiers

**Key Types:**
- `Token`: Represents a single token with kind, text, span
- `TokenKind`: Enum of all token types (keywords, operators, literals, etc.)
- `Lexer`: Main lexer struct with source and position tracking

**Output:** Stream of `Token` objects

### 2. Parser (`compiler/parser`)

**Responsibilities:**
- Build Abstract Syntax Tree (AST) from token stream
- Implement grammar according to language specification
- Recover from errors to continue parsing
- Track precedence and associativity

**Key Types:**
- `Parser`: Main parser struct
- `AST`: Root AST node containing declarations
- Decl, Expr, Stmt, Type, Pattern: AST node enums

**Output:** `AST` tree

### 3. AST (`compiler/ast`)

**Responsibilities:**
- Define all AST node types
- Provide serialization/deserialization for AI training data
- Implement visitor pattern for analysis passes

**Key Types:**
- `Module`: Top-level compilation unit
- `FunctionDecl`, `StructDecl`, `EnumDecl`, `TraitDecl`, `ImplDecl`
- `Expr`: Expressions (literals, binary ops, calls, etc.)
- `Stmt`: Statements (let, if, while, return, etc.)
- `Type`: Type representations
- `Pattern`: Pattern matching constructs

**Features:**
- Source span tracking for each node
- Optional attributes/metadata
- Serialization support

### 4. Name Resolution (`compiler/semantic`)

**Responsibilities:**
- Build symbol tables and scopes
- Resolve all identifiers to declarations
- Check for duplicate definitions
- Handle imports and module resolution (future)

**Key Types:**
- `Resolver`: Main name resolution pass
- `SymbolTable`: Hierarchical scope management
- `Symbol`: Represents a resolved identifier

**Output:** AST with resolved references

### 5. Type System (`compiler/types`)

**Responsibilities:**
- Define type representations
- Implement type inference
- Check type correctness
- Handle generic instantiation
- Support trait constraints (future)

**Key Types:**
- `Ty`: Type representation (primitives, structs, enums, references, etc.)
- `TypeChecker`: Main type checking pass
- `TypeContext`: Context for type operations
- `Substitution`: Type variable substitution for inference

**Features:**
- Monomorphization for generics
- Layout computation (size, alignment)
- Type compatibility checking

### 6. Ownership/Borrow Checker (`compiler/ownership`)

**Responsibilities:**
- Enforce ownership rules
- Check borrow validity
- Detect use-after-move, double free, data races
- Track lifetimes (future)

**Key Types:**
- `BorrowChecker`: Main analysis pass
- `BorrowRegion`: Tracking borrows and their scopes
- `OwnershipState`: Per-value ownership information
- `Lifetime`: Represent lifetime annotations

**Key Checks:**
- No mutable borrow while immutable borrow exists
- No multiple mutable borrows
- Values cannot be used after move
- References outlive their scope

### 7. HIR (`compiler/hir`)

**Responsibilities:**
- Lower AST to High-Level IR
- Desugar complex constructs
- Prepare for lower-level analysis
- Provide cleaner representation for codegen

**Key Types:**
- `HIRModule`: HIR compilation unit
- `HIRFunction`, `HIRBlock`, `HIRStatement`, `HIRExpression`
- Simplified, desugared representation

**Transformations:**
- Pattern matching desugaring
- Method resolution
- Operator overloading resolution (future)
- Macro expansion (future)

### 8. MIR (`compiler/mir`)

**Responsibilities:**
- Lower HIR to Mid-Level IR
- Convert to SSA form
- Prepare for LLVM IR generation
- Enable optimizations

**Key Types:**
- `MIRFunction`: MIR function representation
- `BasicBlock`: Control flow blocks
- `Terminator`: Block terminators (branches, returns)
- `Statement`: Instructions within blocks
- `Place`: Memory locations
- `RValue`: Computable values

**Features:**
- SSA form
- Control flow graph
- Type information preserved
- Borrow checking integration

### 9. Code Generation (`compiler/codegen`)

**Responsibilities:**
- Convert MIR to LLVM IR
- Generate native machine code via LLVM
- Handle calling conventions
- Implement runtime interfaces

**Key Types:**
- `CodeGenerator`: Main codegen driver
- `LLVMContext`: LLVM context management
- `FunctionBuilder`: LLVM IR construction

**Backend:**
- LLVM (primary)
- Architecture designed for future backends (Cranelift, etc.)

### 10. CLI (`cli`)

**Responsibilities:**
- Provide command-line interface
- Manage compilation pipeline
- Handle file I/O
- Format errors for display

**Commands:**
- `kryo build <file>`: Compile to executable
- `kryo run <file>`: Compile and execute
- `kryo check <file>`: Type check only
- `kryo fmt <file>`: Format source (future)
- `kryo test`: Run tests (future)
- `kryo clean`: Clean build artifacts

## Error Reporting

### Diagnostic System

All errors pass through a unified diagnostic system:

```rust
struct Diagnostic {
    level: DiagnosticLevel,
    message: String,
    spans: Vec<Span>,
    suggestions: Vec<Suggestion>,
    notes: Vec<String>,
    error_code: Option<ErrorCode>,
}
```

### Error Codes

Structured error codes for machine-readable output:
- `E0001`: Syntax error
- `E0002`: Undefined identifier
- `E0003`: Type mismatch
- `E0004`: Borrow conflict
- `E0005`: Use after move
- etc.

### Error Display

```text
error[E0003]: type mismatch

 --> main.kyo:5:10
  |
5 |     let x: i32 = "hello"
  |          ^^^^^   ------- expected i32, found &str
  |
help: consider changing the type
  |
5 |     let x: &str = "hello"
  |
```

## Testing Architecture

### Unit Tests

Each component has unit tests:
- Lexer tests: tokenization of various constructs
- Parser tests: AST generation for all grammar rules
- Type checker tests: type checking scenarios
- Ownership tests: borrow checking cases

### Integration Tests

End-to-end compilation tests:
- Compile `.kyo` files
- Verify executable output
- Check error messages

### Regression Tests

Maintain test suite of known issues and edge cases.

## Build System

### Build Artifacts

```
build/
├── ir/              # LLVM IR files
├── asm/             # Assembly files
├── obj/             # Object files
└── bin/             # Final executables
```

### Dependency Management

- Cargo for Rust dependencies
- LLVM linked via inkwell
- Future: package manager for Kryo libraries

## Performance Considerations

### Compilation Speed

- Incremental compilation (future)
- Parallel compilation of independent modules (future)
- Efficient data structures (Rc, Arc for sharing)

### Generated Code Performance

- Zero-cost abstractions
- Inline functions where beneficial
- Optimize via LLVM passes
- Manual control over layout and alignment

## Future Extensions

### Planned Features

1. **Package Manager**: Dependency management for Kryo projects
2. **LSP Server**: IDE integration (autocomplete, goto definition, etc.)
3. **Formatter**: Code formatting tool
4. **Macro System**: Compile-time metaprogramming
5. **Const Evaluation**: Compile-time function evaluation
6. **Async/Await**: Asynchronous programming support
7. **SIMD Intrinsics**: Vector operations
8. **Inline Assembly**: Platform-specific assembly
9. **Foreign Function Interface**: C interop
10. **Reflection**: Runtime type information (optional)

### AI/ML Integration

1. **KryoLM Training Data**: Compiler outputs structured datasets
2. **Error Explanation**: AI-powered error message improvement
3. **Code Suggestion**: Intelligent code completion
4. **Refactoring**: Automated code transformations
5. **Translation**: Bidirectional translation with C/C++/Rust

## Project Structure

```
kryo/
├── compiler/          # Core compiler library
│   ├── lexer/        # Tokenization
│   ├── parser/       # AST generation
│   ├── ast/          # AST definitions
│   ├── semantic/     # Name resolution
│   ├── types/        # Type system
│   ├── ownership/   # Borrow checking
│   ├── hir/          # High-level IR
│   ├── mir/          # Mid-level IR
│   └── codegen/      # LLVM IR generation
├── cli/              # Command-line interface
├── runtime/          # Runtime library
├── std/              # Standard library
├── tools/            # Development tools
├── tests/            # Test suite
├── examples/         # Example programs
├── docs/             # Documentation
└── build/            # Build artifacts
```

## Implementation Language

The compiler is implemented in **Rust** for:
- Memory safety in the compiler itself
- Excellent LLVM integration via inkwell
- Strong type system for complex compiler logic
- Modern tooling and ecosystem
- Performance

## Dependencies

Core dependencies:
- `inkwell`: LLVM bindings
- `clap`: CLI argument parsing
- `thiserror`: Error handling
- `anyhow`: Error propagation
- `serde`: Serialization for data exchange
- `regex`: Lexer implementation

## Development Workflow

1. Write specification for new feature
2. Implement component with tests
3. Integrate into compilation pipeline
4. Add integration tests
5. Update documentation
6. Verify compilation of example programs

## Verification

The MVP will be verified by:
1. Compiling a simple `main.kyo` program
2. Generating a working executable
3. Running the executable and checking output
4. Verifying error messages for invalid code
