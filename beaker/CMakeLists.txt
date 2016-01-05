# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Add the core Beaker library.
add_library(beaker
  prelude.cpp
  lexer.cpp
  parser.cpp
  ast.cpp
  name.cpp
  type.cpp
  expr.cpp
  init.cpp
  decl.cpp
  stmt.cpp
  translation.cpp
  hash.cpp
  equivalence.cpp
  print.cpp
)
target_compile_definitions(beaker PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(beaker
  PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}>"
    ${lingo_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(beaker
PUBLIC
  lingo
  ${Boost_LIBRARIES}
  ${LLVM_LIBRARIES}
)

# The compiler is the main driver for compilation.
add_executable(beaker-compile main.cpp)
target_link_libraries(beaker-compile beaker)


macro(add_run_test target)
  add_executable(${target} ${ARGN})
  target_link_libraries(${target} beaker)
endmacro()


# Unit tests
# TODO: Move these into a different directory.
add_run_test(test_print       test_print.cpp)
add_run_test(test_equivalence test_equivalence.cpp)
add_run_test(test_hash        test_hash.cpp)
add_run_test(test_variable    test_variable.cpp)
