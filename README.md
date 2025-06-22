# Monkey Interpreter in C++

This project is a C++ implementation of the [Monkey programming language](https://interpreterbook.com/), inspired by Thorsten Ball's book *"Writing An Interpreter In Go"*. It features a full interpreter pipeline from lexing to evaluation, designed to be modular and educational.

## Overview

Monkey is a small, expressive programming language designed for learning interpreter design. This implementation covers:

- **Lexical analysis** (lexer)
- **Parsing** into an Abstract Syntax Tree (AST)
- **Evaluation** using a tree-walking interpreter
- **REPL** (Read-Eval-Print Loop) for interactive use

Example usage:
```monkey
let add = fn(x, y) { x + y; };
add(3, 4);  // -> 7
```

## 🗂Project Structure

├── main.cpp               # Entry point for launching the REPL
├── repl/                  # REPL loop and error printing
├── lexer/                 # Token stream generator
├── parser/                # AST builder from tokens
├── ast/                   # AST node definitions
├── object/                # Object system for evaluated values
├── environment/           # Variable scope and bindings
├── evaluator/             # Core interpreter logic (tree-walking evaluator)
└── token/                 # Token definitions and keyword mapping

## Build & Run

### Compile Manually

```bash
g++ -std=c++17 main.cpp \
    repl/repl.cpp \
    lexer/lexer.cpp \
    parser/parser.cpp \
    evaluator/evaluator.cpp \
    -o monkey
```

### Run the Interpreter

```bash
./monkey
```

## Features Implemented

- Variables with **let**
- **Integer** and **boolean** literals
- Arithmetic operations: **+**, **-**, *, **/**
- Comparison: **==**, **!=**, **<**, **>**
- **Prefix** operators: **-**, **!**
- **Conditional** statements: if / else
- **Functions** and first-class closures
- Return statements
- Nested scopes
