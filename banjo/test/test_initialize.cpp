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

  Variable_decl& v1 = build.make_variable("v1", i32);
  Expr& r_v1 = build.make_reference(v1);

  Init& init1 = initialize(cxt, i32, build.make_equal_init(zero));
  std::cout << init1 << '\n';

  Init& init2 = initialize(cxt, r_i32, build.make_equal_init(r_v1));
  std::cout << init2 << '\n';
}


int
main(int argc, char* argv[])
{
  Context c;

  test_init(c);
}
