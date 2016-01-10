// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;


Namespace_decl& global = make_global_ns();

void
test_names()
{
  std::cout << "--- names ---\n";
  Name& n1 = make_id("N1");
  Name& n2 = make_id("N2");

  Namespace_decl ns1(n1);
  Namespace_decl ns2(n2);
  ns1.context(global);
  ns2.context(ns1);

  // TODO: How do I automate these tests?
  Qualified_id qn1(global, n1);
  std::cout << qn1 << '\n'; // ::N1

  Qualified_id qn2(ns1, n2);
  std::cout << qn2 << '\n'; // N1::N2

  std::cout << ns2.qualified_id() << '\n'; // N1::N2
  std::cout << ns2.fully_qualified_id() << '\n'; // N1::N2
}


void
test_types()
{
  std::cout << "--- types ---\n";
  Void_type t1;
  std::cout << t1 << '\n'; // void

  Boolean_type t2;
  std::cout << t2 << '\n'; // bool

  Integer_type t3;
  std::cout << t3 << '\n'; // int

  Float_type t4;
  std::cout << t4 << '\n'; // double

  Function_type t5({&t2, &t3}, t2);
  std::cout << t5 << '\n'; // (bool, int32) -> bool

  Pointer_type t6(t2);
  std::cout << t6 << '\n'; // bool*;

  Reference_type t7(t5);
  std::cout << t7 << '\n'; // ((bool, int32) -> bool)&;

  Function_type t8({}, t1);
  std::cout << t8 << '\n'; // () -> void

  Sequence_type t9(*new Pointer_type(t8));
  std::cout << t9 << '\n'; // (() -> void)*[]
}


void
test_expressions()
{
  std::cout << "--- expressions ---\n";
  Expr& e1 = make_true();
  Expr& e2 = make_false();
  std::cout << e1 << '\n';
  std::cout << e2 << '\n';

  Expr& e3 = *new And_expr(make_bool_type(), e1, e2);
  Expr& e4 = *new Not_expr(make_bool_type(), e3);
  std::cout << e4 << '\n';
}


int
main(int argc, char* argv[])
{
  init_tokens();

  test_names();
  test_types();
  test_expressions();
}
