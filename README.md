# Mini-Compiler Construction (C-like Language Subset)

A modular, end-to-end compiler implementation for a restricted subset of the C programming language. This project demonstrates the complete compilation pipeline, including lexical analysis, parsing, semantic analysis, optimization, intermediate code generation, and target code generation. Developed as an Open-Ended Lab (OEL) project for the Compiler Construction course.

## 🚀 Project Overview

The compiler follows a modern compiler architecture by separating front-end and back-end responsibilities. Lexical and syntax analysis are implemented using **Flex** and **Bison**, while semantic analysis, optimization, and code generation are developed in **C++17**.

The project supports both source-file compilation and interactive compilation through a command-line interface.

## ✨ Key Features

### Lexical Analysis

* Tokenization of identifiers, keywords, operators, literals, and punctuation.
* Support for single-line and multi-line comments.
* Detection and reporting of lexical errors.

### Syntax Analysis

* Context-Free Grammar (CFG) implementation using Bison.
* Construction of an Abstract Syntax Tree (AST).
* Panic-mode error recovery for improved parsing robustness.

### Semantic Analysis

* Type checking and compatibility verification.
* Scope resolution using a stack-based symbol table.
* Function declaration and parameter validation.
* Detection of semantic errors such as undeclared variables and type mismatches.

### Abstract Syntax Tree (AST)

* Object-oriented AST design using `std::shared_ptr`.
* Hierarchical representation of program structure.
* AST visualization through a pretty-printing utility.

### Intermediate Code Generation

* Generation of Three-Address Code (TAC).
* Structured intermediate representation suitable for optimization.

### Code Optimization

* Constant Folding.
* Dead Code Elimination.
* Basic optimization passes to improve generated code quality.

### Target Code Generation

* Translation of TAC into pseudo-assembly instructions.
* Demonstrates the back-end phase of compiler design.

### Interactive Compilation Mode

* File-based compilation support.
* Interactive terminal mode for rapid testing and debugging.

---

## 🏗 Compiler Architecture

```text
Source Code
     │
     ▼
Lexical Analyzer (Flex)
     │
     ▼
Syntax Analyzer (Bison)
     │
     ▼
Abstract Syntax Tree
     │
     ▼
Semantic Analyzer
     │
     ▼
Three-Address Code (TAC)
     │
     ▼
Optimizer
     │
     ▼
Pseudo Assembly Generator
     │
     ▼
Target Output
```

## 📂 Project Structure

```text
.
├── ast.cpp / ast.h
│   └── AST node definitions and pretty printer
│
├── lexer.l
│   └── Flex lexical analyzer
│
├── parser.y
│   └── Bison grammar and parser
│
├── semantic_analyzer.cpp
│   └── Semantic analysis and type checking
│
├── symbol_table.cpp
│   └── Scope management and symbol tracking
│
├── tac.cpp / tac.h
│   └── Three-Address Code generation
│
├── optimizer.cpp
│   └── Optimization passes
│
├── codegen.cpp / codegen.h
│   └── Pseudo-assembly generation
│
├── main.cpp
│   └── Compiler driver and user interface
│
└── Makefile
    └── Build automation
```

## 🛠 Technologies Used

* C++17
* Flex
* Bison
* GNU Make
* Object-Oriented Design
* Compiler Construction Concepts

## ⚙️ Installation

### Prerequisites

```bash
g++
flex
bison
make
```

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential flex bison
```

## 🔨 Build

```bash
make
```

## ▶️ Run

```bash
./mini_compiler
```

### Available Modes

#### File Compilation

Compile a source file from the provided test cases.

```text
Example:
testcases/valid_tests.txt
```

#### Interactive Mode

Enter C-like source code directly into the terminal.

```text
Type your code
Press Ctrl+D to compile
```

## 📚 Learning Outcomes

This project demonstrates practical implementation of:

* Lexical Analysis
* Context-Free Parsing
* Abstract Syntax Trees
* Symbol Table Management
* Semantic Analysis
* Intermediate Code Generation
* Compiler Optimizations
* Target Code Generation
* Error Detection and Recovery

## 🎓 Academic Information

Developed as part of the **Compiler Construction (CSC303L)** course at the **University of Engineering and Technology (UET), Lahore**.

The project fulfills course learning outcomes related to compiler phase integration, language processing, and software system design.

## 👨‍💻 Author

**Abaidullah Bhatti**

## 📄 License

This project is intended for educational and academic purposes.
