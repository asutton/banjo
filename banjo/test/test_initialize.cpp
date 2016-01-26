// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/initialization.hpp>

#include <iostream>
#include <iomanip>


void
test_init(Context& cxt)
{
  Builder build(cxt);

  Type& i32 = build.get_int_type();
  Type& r_i32 = build.get_reference_type(i32);

  Expr& zero = build.get_zero(i32);
  Expr& init1 = copy_initialize(cxt, i32, zero);
  Variable_decl& v1 = build.make_variable("v1", i32, init1);
  std::cout << v1 << '\n';

  Expr& r_v1 = build.make_reference(v1);
  Expr& init2 = copy_initialize(cxt, r_i32, r_v1);
  Variable_decl& v2 = build.make_variable("v2", r_i32, init2);
  std::cout << v2 << '\n';
}


int
main(int argc, char* argv[])
{
  Context c;

  test_init(c);
}
