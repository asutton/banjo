// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/deduction.hpp>

#include <iostream>


void
test_deduction(Type& p, Type& a)
{
  Substitution sub;
  deduce_from_type(p, a, sub);

  std::cout << p << " vs " << a << " => ";
  std::cout << '{';
  for (auto iter = sub.begin(); iter != sub.end(); ++iter) {
    std::cout << *iter->first << " ~> " << *iter->second;
    if (std::next(iter) != sub.end())
      std::cout << ", ";
  }
  std::cout << '}' << '\n';
}


void
test_deduce_from_type(Context& cxt)
{
  Builder build(cxt);

  // Substitutable type and an argument.
  Decl& parm = build.make_type_parameter("T");
  Type& arg = build.get_int_type();

  // Tests
  Type& t0 = build.get_typename_type(parm); // T
  Type& a0 = arg;                           // int
  test_deduction(t0, a0);                   // T ~> int

  Type& t1 = build.get_reference_type(t0); // T&
  Type& a1 = build.get_reference_type(a0); // int&
  test_deduction(t1, a1);                  // T ~> int

  // FIXME: Test more stuff.
}




int
main(int argc, char* argv[])
{
  Context cxt;
  test_deduce_from_type(cxt);
}
