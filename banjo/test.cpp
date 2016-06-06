// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

// A dumb test stub for AST experiments.

#include <iostream>

#include "ast.hpp"
#include "debugging.hpp"

#include <lingo/io.hpp>


using namespace banjo;


Allocator alloc;


template<typename T>
T& f(T& t)
{
  return t.clone(alloc);
}


int main()
{
  lingo::init_colors();

  Type_list parms { &Boolean_type::make(alloc) };
  auto& f = Function_type::make(alloc, parms, Void_type::make(alloc));

  debug(f);
}
