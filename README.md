# **Yox47**

## **Description**

**Yox47** is a compact, statically‑typed, block‑structured programming language, written  in C++. It covers each stage of the toolchain — from tokenization and abstract syntax trees to semantic validation and optimized assembly emission. Yox47’s type system includes primitive types (`int`, `float`, `char`, `bool`), heap‑allocated `box[T]` arrays, and explicit function signatures. Control flow constructs comprise `if/else`, `for`, `while`, `break`, and `choose/case`.

## Supported Features

- **Types:**
  - `int`, `float`, `char`, `bool`
  - `box[T]` – heap-allocated arrays of type `T`

- **Functions:**
  - Explicit type declarations for parameters and return types
  - Support for return values
  - Nested function calls

- **Control Flow:**
  - `if`, `else`, `while`, `for`, `break`
  - `choose/case` – similar to `switch`, with multiple case options

- **Expressions:**
  - Arithmetic: `+`, `-`, `*`, `/`, `%`
  - Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
  - Logical: `&&`, `||`, `!`
  - Assignment: `=`

- **Scoping & Blocks:**
  - Local variable scoping using `{}` blocks

- **Memory:**
  - Allocation via `box[T]`
  - No garbage collection — manual memory handling where required

- **Code Generation:**
  - Emits optimized assembly (`out.asm`)
  - Separation of runtime and compile-time code

- **Toolchain:**
  - Full compilation pipeline: lexer → parser → AST → semantic analysis → code generation

## Not Yet Supported / Known Limitations

- No support for:
  - Structs or object-oriented features
  - Inheritance or polymorphism
  - Generics beyond `box[T]`
  - Exception handling (`try/catch`)
  - Lambda/anonymous functions
  - Importing external modules/files
  - Input/output operations (e.g., user input, file writing)
  - Strings as a native, first-class type
  - Standard libraries

- Statically linked only — no dynamic linking
- No support for multithreading or concurrency primitives

## Syntax Overview

| Concept                   | Syntax Example                                      |
|---------------------------|------------------------------------------------------|
| **Variable declaration**  | `int x = 5;`                                         |
| **Function declaration**  | `int add(int a, int b) { return a + b; }`           |
| **Function return**       | `return x * 2;`                                      |
| **Heap array**            | `box<int> arr = box_make(10);`                      |
| **Array access**          | `int y = arr[2];`                                    |
| **Array assign**          | `arr[3] = 42;`                                       |
| **Array length**          | `int len = box_length(arr);`                        |
| **If/else**               | `if (x > 0) { ... } else { ... }`                   |
| **While loop**            | `while (x < 10) { x = x + 1; }`                     |
| **For loop**              | `for (int i = 0; i < 10; i = i + 1) { ... }`        |
| **Choose/case**           | `choose(x) { case 1: ...; case 2: ...; }`           |
| **Break statement**       | `break;`                                             |
| **Function call**         | `int y = add(3, 4);`                                |
| **Logical expression**    | `if (a > 0 && b < 5) { ... }`                       |
| **Arithmetic expression** | `int z = x * (y + 2);`                              |
| **Comparison**            | `if (a != b) { ... }`                               |
| **Assignment**            | `x = x + 1;`                                        |
| **Block scoping**         | `{ int temp = 3; ... }`                             |
| **Empty block**           | `{}`                                                |
| **Comment (single-line)** | `// this is a comment`                              |
| **Comment (multi-line)**  | `/* start ... end */`                               |
