// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/normalization.hpp>

#include <iostream>


Concept_decl&
make_concept_1(Context& cxt)
{
  Builder build(cxt);
  Type_parm& p = build.make_type_parameter("T");
  Expr& e = build.get_true();
  Concept_decl& c = build.make_concept("C1", {&p}, e);
  return c;
}


void
test_normalize(Context& cxt)
{
  Builder build(cxt);

  Concept_decl& c = make_concept_1(cxt);
  Type_parm& p1 = build.make_type_parameter("T");

  Type& b = build.get_bool_type();
  Type& t1 = build.get_typename_type(p1);

  // Build a test expression.
  Expr& t = build.get_true();
  Expr& f = build.get_false();
  Expr& c1 = build.make_check(c, {&t1});
  Expr& e1 = build.make_not(b, f);
  Expr& e2 = build.make_and(b, t, e1);
  Expr& e3 = build.make_and(b, e2, c1);
  std::cout << e3 << '\n';

  // Normalize it.
  Cons& nc1 = normalize(cxt, e3);
  std::cout << nc1 << '\n';
}



int
main(int argc, char* argv[])
{
  Context cxt;
  test_normalize(cxt);
}
