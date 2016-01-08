// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;


Namespace_decl* global;

void
test_names()
{
  std::cout << "--- names ---\n";
  Name* n1 = make_id("N1");
  Name* n2 = make_id("N2");

  auto* ns1 = new Namespace_decl(global, n1);
  auto* ns2 = new Namespace_decl(ns1, n2);

  // TODO: How do I automate these tests?
  Name* qn1 = new Qualified_id(global, n1);
  std::cout << *qn1 << '\n'; // ::N1

  Name* qn2 = new Qualified_id(ns1, n2);
  std::cout << *qn2 << '\n'; // N1::N2

  std::cout << ns2->qualified_id() << '\n'; // N1::N2
  std::cout << ns2->fully_qualified_id() << '\n'; // N1::N2
}


void
test_types()
{
  std::cout << "--- types ---\n";
  Type* t1 = new Void_type();
  std::cout << *t1 << '\n'; // void

  Type* t2 = new Boolean_type();
  std::cout << *t2 << '\n'; // bool

  Type* t3 = new Integer_type();
  std::cout << *t3 << '\n'; // bool

  Type* t4 = new Float_type();
  std::cout << *t4 << '\n'; // bool
}


void
test_expressions()
{
  std::cout << "--- expressions ---\n";
  Expr* e1 = make_true();
  Expr* e2 = make_false();
  std::cout << *e1 << '\n';
  std::cout << *e2 << '\n';

  Expr* e3 = new And_expr(make_bool_type(), e1, e2);
  Expr* e4 = new Not_expr(make_bool_type(), e3);
  std::cout << *e4 << '\n';
}


int
main(int argc, char* argv[])
{
  init_tokens();

  global = make_global_ns();

  test_names();
  test_types();
  test_expressions();
}
