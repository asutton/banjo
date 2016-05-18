
## Smallish language tasks

- Operator names and overloading
- Re-enable overload resolution
- Class member lookup
- Base class conversions

- Constructors and destructors

- Initialization
  - Direct initialization
  - Aggregate initialization
  - Reference binding
  - Lifetime extension

- Fix the subscript operator
  - Consider variants that take a constant argument (see below)

- Fix arithmetic conversions
- Floating point types and values
- Extended integer types and values
- Character types and values
- String literal types and values
- Digit separators
- Literal suffixes

- Foreign functions
  - Allow the declaration of foreign functions

- Builtin types and functions

- Qualified names
  - Guarantee that every declaration is mapped to its enclosing
    declaration so that we can generate qualified names
  - Parse nested name specifiers
  - Support qualified name lookup

## Bigger language tasks

- Constant function arguments
- Templates and concepts
- Move semantics
- Virtual functions
- Open multimethods
- Matching (pattern, dynamic type, static type)
- Modules

- C++ interoperability
  - We probably need a C++ compiler with modules to do this
  - Import a foreign C++ module?


## C++ Generation

- Start implementing this
  - Translation should be almost direct. Banjo is very close to C++
  - Emit declarations based on order dependence


## LLVM Generation



## Framework tasks

- Cosntant folding - Write an elaboration (?) pass that actually folds 
  constant expressions in contexts where they weren't already folded.

- Use the elaboration framework for elaborators

- Cleanly separate the front-end from semantic aspects
  - Move all AST + non-FE semantics into a core library
  - Move all parse + FE semantics into an FE library

- Toolchain work
  - Options parsing
  - Task definition

- Fix source code locations
  - Minimize the size of a Location object
  - Actually use locations aggressively

- Consider refactoring the type system into two parts:
  - The syntax-side (with source locations)
  - The internal side (no source locations, all internal)

- Improve diagnostics
  - Report errors as JSON documents, format later