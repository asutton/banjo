// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/normalization.hpp>

#include <iostream>


void
test_normalize(Context& cxt)
{
  Builder build(cxt);

  Type& b = build.get_bool_type();
  Expr& t = build.get_true();
  Expr& f = build.get_false();
  Expr& e1 = build.make_not(b, f);
  Expr& e2 = build.make_and(b, t, e1);
  Expr& e3 = build.make_and(b, e2, t);
  std::cout << e3 << '\n';

  Cons& c = normalize(cxt, e3);
  std::cout << c << '\n';
}



int
main(int argc, char* argv[])
{
  Context cxt;
  test_normalize(cxt);
}
