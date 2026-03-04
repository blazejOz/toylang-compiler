# llvm-cpp-compiler
[![CI](https://github.com/blazejOz/llvm-cpp-compiler/actions/workflows/pipeline.yml/badge.svg)](https://github.com/blazejOz/llvm-cpp-compiler/actions)

A small learning project: a simple C++ compiler frontend aimed at learning LLVM by building a minimal, opinionated compiler.

Status
------

In progress — currently provides an OOP skeleton for the frontend and a working translation for a `print` statement to the IR. The project is educational and evolving.

Goals
-----

- Learn the LLVM toolchain and IR generation.
- Implement a small language frontend (lexer, parser, AST).
- Generate LLVM IR and explore code generation optimizations.

Quick build & run
---------------

Requires CMake and a C++ toolchain. From the repository root:

```bash
cmake -B build
cmake --build build
./build/compiler
```

Binaries and tests are produced in the `build` folder (for example, `build/compiler` and `build/unit_tests`).

Project layout
--------------

- Source:
  - [src/main.cpp](src/main.cpp)
  - [src/frontend/Lexer.cpp](src/frontend/Lexer.cpp)
  - [src/frontend/Parser.cpp](src/frontend/Parser.cpp)
  - [src/frontend/AST.cpp](src/frontend/AST.cpp)
  - [src/codegen/IR_Generator.cpp](src/codegen/IR_Generator.cpp)
- Public headers: [include/](include/)
  - [include/AST.hpp](include/AST.hpp)
  - [include/Lexer.hpp](include/Lexer.hpp)
  - [include/Parser.hpp](include/Parser.hpp)
  - [include/IR_Generator.hpp](include/IR_Generator.hpp)
- Tests: [tests/](tests/) (Catch2 based unit tests)

Current features
----------------

- Object-oriented skeleton for the compiler frontend.
- Basic lexer and parser scaffolding.
- Translation of a `print` statement into the project's IR generator.

Notes
-----

This repository is intended as a learning exercise. Expect frequent changes and refactors as ideas are explored.

License
-------

See the project `LICENSE` file.
