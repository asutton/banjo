# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Add the core Banjo library.
add_library(banjo
  prelude.cpp
  error.cpp
  context.cpp

  # TODO: Factor this out to support multiple front ends.
  # Lexical and syntactic components
  token.cpp
  lexer.cpp
  parser.cpp
  parse-tu.cpp
  parse-id.cpp
  parse-type.cpp
  parse-expr.cpp
  parse-stmt.cpp
  parse-decl.cpp
  parse-function.cpp
  parse-class.cpp
  parse-req.cpp
  sema-tu.cpp
  sema-id.cpp
  sema-type.cpp
  sema-expr.cpp
  sema-init.cpp
  sema-stmt.cpp
  sema-decl.cpp
  sema-function.cpp
  sema-class.cpp
  sema-req.cpp
  elaboration.cpp
  elab-declarations.cpp
  elab-overloads.cpp
  elab-classes.cpp
  elab-expressions.cpp
  printer.cpp

  # Core facilities
  builder.cpp
  ast.cpp
  ast-base.cpp
  ast-name.cpp
  ast-type.cpp
  ast-expr.cpp
  ast-stmt.cpp
  ast-decl.cpp
  ast-def.cpp
  ast-req.cpp
  ast-hash.cpp
  ast-eq.cpp
  value.cpp

  operator.cpp
  qualifier.cpp
  specifier.cpp
  scope.cpp
  lookup.cpp
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
  conversion.cpp
  initialization.cpp
  call.cpp
  inheritance.cpp
  # template.cpp
  # substitution.cpp
  # deduction.cpp
  # requirement.cpp
  # constraint.cpp
  # normalization.cpp
  # satisfaction.cpp
  # subsumption.cpp
  evaluation.cpp
  inspection.cpp

  # Code generation
  gen/cxx/generator.cpp
  gen/llvm/generator.cpp
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

# The compiler is the main driver for compilation.
add_executable(banjo-compile main.cpp)
target_link_libraries(banjo-compile banjo)

# A simple expression calculator.
add_executable(banjo-calc calc.cpp)
target_link_libraries(banjo-calc banjo)


# Add an executable test program.
macro(add_test_program target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} banjo)
endmacro()


# Add a test program and register it with the test suite.
macro(add_unit_test target)
  add_test_program(${target} ${ARGN})
  add_test(${target} ${target})
endmacro()


# Unit tests
#
# FIXME: Conditionally the test suite using an option.

# add_unit_test(test_print       test/test_print.cpp)
# add_unit_test(test_equivalence test/test_equivalence.cpp)
# add_unit_test(test_hash        test/test_hash.cpp)
# add_unit_test(test_variable    test/test_variable.cpp)
# add_unit_test(test_function    test/test_function.cpp)
# add_unit_test(test_template    test/test_template.cpp)
# add_unit_test(test_convert     test/test_convert.cpp)
# add_unit_test(test_initialize  test/test_initialize.cpp)
# add_unit_test(test_substitute  test/test_substitute.cpp)
# add_unit_test(test_deduce      test/test_deduce.cpp)
# add_unit_test(test_constraint  test/test_constraint.cpp)

# Testing tools
# add_test_program(test_parse   test/test_parse.cpp)
# add_test_program(test_inspect test/test_inspect.cpp)
