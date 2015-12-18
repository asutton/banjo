# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Add the core Beaker library.
add_library(beaker
  prelude.cpp
  lexer.cpp
)
target_compile_definitions(beaker PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(
  beaker
    PUBLIC
      "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}>"
      ${lingo_SOURCE_DIR}
      ${Boost_INCLUDE_DIRS}
      ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(
  beaker
    PUBLIC
      lingo
      ${Boost_LIBRARIES}
      ${LLVM_LIBRARIES}
)

# The compiler is the main driver for compilation.
# It takes a set of input files and produces linked
# outputs (programs, libraries, archives).
# add_executable(beaker-compile driver.cpp compiler.cpp)
# target_link_libraries(beaker-compile beaker)
