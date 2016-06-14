# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Construct the front-end parser library.
add_library(banjo-fe
  context.cpp
  token.cpp
  lexer.cpp
  printer.cpp
  parser.cpp
  parse-tu.cpp
  parse-id.cpp
  parse-type.cpp
  parse-expr.cpp
  parse-stmt.cpp
  parse-decl.cpp
  parse-function.cpp
  parse-class.cpp
  # parse-req.cpp
  sema-tu.cpp
  sema-id.cpp
  sema-type.cpp
  sema-expr.cpp
  sema-init.cpp
  sema-stmt.cpp
  sema-decl.cpp
  sema-function.cpp
  sema-class.cpp
  elaboration.cpp
  elab-declarations.cpp
  # elab-classes.cpp
  elab-expressions.cpp
)
target_compile_definitions(banjo-fe PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(banjo-fe PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}>"
    ${lingo_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(banjo-fe PUBLIC
  banjo
  lingo
  ${Boost_LIBRARIES}
  ${LLVM_LIBRARIES}
)


# TODO: Consider moving these into a tools directory.

# The compiler is the main driver for compilation.
add_executable(banjo-compile compiler.cpp)
target_link_libraries(banjo-compile banjo banjo-llvm banjo-fe)

# A simple expression calculator.
add_executable(banjo-calc calc.cpp)
target_link_libraries(banjo-calc banjo banjo-fe)
