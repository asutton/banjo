# Copyright (c) 2015-2016 Andrew Sutton
# All rights reserved

# Construct the backend LLVM generator.
add_library(banjo-llvm
  generator.cpp
)
target_compile_definitions(banjo-llvm PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(banjo-llvm
  PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}>"
    ${lingo_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(banjo-llvm
PUBLIC
  banjo
  lingo
  ${Boost_LIBRARIES}
  ${LLVM_LIBRARIES}
)

