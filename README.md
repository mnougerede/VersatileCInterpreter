# Versatile C Interpreter

A lightweight interpreter for a subset of the C programming language, written in C++ using ANTLR for parsing. Developed as a final-year project for the Computer Science MComp degree at the University of Sussex.

---

## 🧠 Overview

This project provides a real-time, interactive environment for writing and evaluating C code. It features a custom-built interpreter capable of running both full C source files and individual statements via a REPL (Read-Eval-Print Loop). Its purpose is to support learning, rapid prototyping, and experimentation with C.

---

## 🎯 Project Objectives

- ✅ Create an interactive IDE to run C code
- ✅ Provide a REPL interface for line-by-line evaluation
- ✅ Implement support for core C features (variables, expressions, control flow, functions)
- ✅ Include basic error handling and type checking
- ⚠️ Explore support for advanced features (pointers, structs, memory management)

---

## 💡 Features

### ✅ Fully Supported
- **Primitive types:** `int`, `double`, `char`
- **Expressions:** arithmetic, logical, comparison
- **Statements:** `if`, `else`, `while`, `do-while`, `for`
- **Functions:** declaration, invocation, return values
- **Block-level scoping**
- **REPL mode** for immediate feedback

### ⚠️ Partially Supported
- Implicit type coercion between compatible types
- Basic type checking on function parameters and return values
- Graceful error handling

### ❌ Not Yet Supported
- `struct`, `union`, `enum`, `typedef`
- Pointers and pointer arithmetic
- Arrays and array indexing
- `switch` statements and `goto`
- Preprocessor directives (e.g., `#include`, `#define`)
- Standard I/O (e.g., `printf`, `scanf`)
- File I/O and dynamic memory (`malloc`, `free`)

---

## 🛠️ Technologies Used

- **C++17** – Core language for the interpreter
- **ANTLR 4** – Used to define and parse C grammar
- **GoogleTest** – Unit and integration testing
- **CMake** – Build system
- **CLion** – IDE used during development

---

## 🚀 Build & Run Instructions

### 🔧 Requirements

- A C++17-compatible compiler (e.g. `g++`, `clang++`)
- [ANTLR 4 runtime](https://www.antlr.org/)
- CMake (v3.15 or higher)

### 🔨 Build

```bash
git clone https://github.com/yourusername/versatile-c-interpreter.git
cd versatile-c-interpreter
mkdir build && cd build
cmake ..
make
```

### ▶️ Run

```bash
./VersatileCInterpreter
```

---

## 🧪 Running Tests

Tests are located in the `tests/` directory and use GoogleTest.

To run all tests:

```bash
cd build
ctest
```

Test coverage includes:
- Expression evaluation
- Type coercion and assignment
- Control flow correctness
- Function declaration/calls
- Scope resolution
- REPL interaction
- Error reporting

---

## 📜 License and Attribution

This project was created as part of a final year MComp project at the University of Sussex in 2025. It may be reused for educational purposes with proper attribution.

```
Max de la Nougerede – Versatile C Interpreter (2025)  
University of Sussex, Department of Informatics
```

---

## 🙏 Acknowledgements

- **Supervisor:** Dr Neil de Beaudrap
- **ANTLR** for the excellent parsing framework
- **GoogleTest** for simplifying the testing process
- **University of Sussex** for academic support and resources
