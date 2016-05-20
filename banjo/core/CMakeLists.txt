# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Add the core Banjo library.
add_library(banjo-core
  prelude.cpp
  error.cpp
  token.cpp

  ast.cpp
  ast-base.cpp
  ast-name.cpp
  ast-type.cpp
  ast-expr.cpp
  ast-stmt.cpp
  ast-decl.cpp
  ast-def.cpp
  ast-req.cpp
  operator.cpp
  qualifier.cpp
  specifier.cpp
  value.cpp

  context.cpp
  builder.cpp
  printer.cpp
  builtin.cpp
  intrinsic.cpp

  # Tree construction
  type.cpp
  declaration.cpp
  overload.cpp
  expression.cpp
  expr-id.cpp
  expr-logical.cpp
  expr-relational.cpp
  expr-arithmetic.cpp
  expr-bitwise.cpp
  expr-call.cpp  
  call.cpp

  # Semantic rules and algorithms
  equivalence.cpp
  hash.cpp
  conversion.cpp
  initialization.cpp
  inheritance.cpp
  substitution.cpp
  deduction.cpp
  evaluation.cpp
  # template.cpp
  # requirement.cpp
  # constraint.cpp
  # normalization.cpp
  # satisfaction.cpp
  # subsumption.cpp
  inspection.cpp

  scope.cpp
  lookup.cpp)
target_compile_definitions(banjo-core PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(banjo-core
  PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}>"
    ${lingo_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(banjo-core
PUBLIC
  lingo
  ${Boost_LIBRARIES}
  ${LLVM_LIBRARIES}
)

