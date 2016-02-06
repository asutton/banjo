# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Add the core Banjo library.
add_library(banjo
  prelude.cpp
  error.cpp
  context.cpp
  # Lexical components
  token.cpp
  lexer.cpp
  # Syntactic components
  ast.cpp
  builder.cpp
  parser.cpp
  parse_id.cpp
  parse_type.cpp
  parse_expr.cpp
  parse_stmt.cpp
  parse_decl.cpp
  sema_id.cpp
  sema_type.cpp
  sema_expr.cpp
  sema_init.cpp
  sema_stmt.cpp
  sema_decl.cpp
  # Utilities
  hash.cpp
  equivalence.cpp
  # Semantics
  scope.cpp
  lookup.cpp
  overload.cpp
  conversion.cpp
  initialization.cpp
  call.cpp
  inheritance.cpp
  substitution.cpp
  deduction.cpp
  template.cpp
  normalization.cpp
  subsumption.cpp
  print.cpp
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
add_unit_test(test_print       test/test_print.cpp)
add_unit_test(test_equivalence test/test_equivalence.cpp)
add_unit_test(test_hash        test/test_hash.cpp)
add_unit_test(test_variable    test/test_variable.cpp)
add_unit_test(test_function    test/test_function.cpp)
add_unit_test(test_template    test/test_template.cpp)
add_unit_test(test_convert     test/test_convert.cpp)
add_unit_test(test_initialize  test/test_initialize.cpp)
add_unit_test(test_substitute  test/test_substitute.cpp)
add_unit_test(test_deduce      test/test_deduce.cpp)
add_unit_test(test_constraint  test/test_constraint.cpp)

# Testing tools
add_test_program(test_parse   test/test_parse.cpp)
add_test_program(test_inspect test/test_inspect.cpp)
