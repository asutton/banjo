# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Add the core Banjo library.
add_library(banjo
  prelude.cpp
  token.cpp

  # Abstract syntax
  ast-base.cpp
  ast-name.cpp
  ast-type.cpp
  ast-expr.cpp
  ast-stmt.cpp
  ast-decl.cpp
  ast-def.cpp
  ast-req.cpp
  ast.cpp

  # # AST utility algorithms
  equivalence.cpp
  hashing.cpp

  # Node construction
  build-name.cpp
  build-type.cpp
  build-expr.cpp
  # build-stmt.cpp
  # build-decl.cpp
  builder.cpp

  # context.cpp
  # printer.cpp
  # builtin.cpp
  # intrinsic.cpp
  
  # Diagnostics
  # error.cpp

  # # Tree construction
  # type.cpp
  # declaration.cpp
  # overload.cpp
  # expression.cpp
  # expr-id.cpp
  # expr-logical.cpp
  # expr-relational.cpp
  # expr-arithmetic.cpp
  # expr-bitwise.cpp
  # expr-call.cpp


  # # Semantic rules and algorithms
  # conversion.cpp
  # initialization.cpp
  # inheritance.cpp
  # substitution.cpp
  # deduction.cpp
  # # template.cpp
  # # requirement.cpp
  # # constraint.cpp
  # # normalization.cpp
  # # satisfaction.cpp
  # # subsumption.cpp
  # inspection.cpp

  # Compile-time evaluation
  # value.cpp
  # evaluation.cpp

  # Scope, lookup, and resolution
  # scope.cpp
  # lookup.cpp
  # overload.cpp
)

target_compile_definitions(banjo PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(banjo
  PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}>"
    ${lingo_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(banjo
PUBLIC
  lingo
  ${Boost_LIBRARIES}
  ${LLVM_LIBRARIES}
)


# add_subdirectory(fe/banjo)
