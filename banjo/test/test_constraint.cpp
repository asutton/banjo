// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/normalization.hpp>
#include <banjo/subsumption.hpp>

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
test_canonical(Context& cxt)
{
  Builder build(cxt);

  auto norm = [&cxt](Expr& e) ->Cons& { return normalize(cxt, e); };

  Type& b = build.get_bool_type();

  Expr& t = build.get_true();
  lingo_assert(&norm(t) == &norm(t));

  Expr& f = build.get_false();
  lingo_assert(&norm(f) == &norm(f));

  Expr& e1 = build.make_not(b, f);
  lingo_assert(&norm(e1) == &norm(e1));

  // TODO: Check canonicalization of more constraints.

  // Expr& e2 = build.make_and(b, t, e1);
  // Expr& e3 = build.make_and(b, e2, c1);

}


void
test_subsume_1(Context& cxt)
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
  Expr& e4 = build.make_or(b, f, t);

  // Build some constraints.
  Cons& cons1 = normalize(cxt, e3);
  std::cout << cons1 << '\n';

  Cons& cons2 = normalize(cxt, e4);
  std::cout << cons2 << '\n';

  bool b1 = subsumes(cxt, cons1, cons2);
  std::cout << cons1 << " ~< " << cons2 << " == " << b1 << '\n';

  bool b2 = subsumes(cxt, cons2, cons1);
  std::cout << cons2 << " ~< " << cons1 << " == " << b2 << '\n';
}


// This is GCC's bug 6756.
void
test_subsume_2(Context& cxt)
{
  Builder build(cxt);

  Type& b = build.get_bool_type();

  Type_parm& p1 = build.make_type_parameter("T");
  Expr& e0 = build.get_int(0); // Not a valid constraint
  Expr& e1 = build.get_int(1); // Not a valid constraint
  Expr& e2 = build.get_int(2); // Not a valid constraint
  Expr& e3 = build.make_and(b, e1, e2);
  Expr& e4 = build.make_or(b, e0, e3);
  Concept_decl& c1 = build.make_concept("C1", {&p1}, e4);

  Type_parm& p2 = build.make_type_parameter("U");
  Type& u = build.get_typename_type(p2);
  Expr& e5 = build.make_check(c1, {&u});
  Expr& e6 = build.make_and(
    b, e5, build.make_and(
      b, e5, build.make_and(
        b, e5, build.make_and(
          b, e5, build.make_and(
            b, e5, build.make_and(
              b, e5, e5
            )
          )
        )
      )
    )
  );
  Concept_decl& c2 = build.make_concept("C2", {&p2}, e6);

  // Build a fake requires clause.
  Type_parm& p3 = build.make_type_parameter("V");
  Type& v = build.get_typename_type(p3);
  Expr& e7 = build.make_check(c2, {&v});
  Expr& e8 = build.make_and(b, e7, e7);

  // We'll use this as the basis for the comparison.
  Expr& e9 = build.get_true();

  Cons& con1 = normalize(cxt, e8);
  Cons& con2 = normalize(cxt, e9);

  subsumes(cxt, con1, con2);
  subsumes(cxt, con2, con1);
}


int
main(int argc, char* argv[])
{
  Context cxt;
  test_canonical(cxt);
  // test_subsume_1(cxt);
  test_subsume_2(cxt);
}
