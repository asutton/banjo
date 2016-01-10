// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <beaker/convert.hpp>

#include <iostream>


using namespace lingo;
using namespace beaker;


int
main(int argc, char* argv[])
{
  Context cxt;
  Builder build(cxt);

  Type& b = build.get_bool_type();
  Type& z = build.get_int_type();
  Type& cz = build.get_const_type(z);

  Variable_decl& v1 = build.make_variable("v1", b);

  // object-to-value
  Reference_expr e1 = build.make_reference(v1);
  Expr& c1 = convert(e1, b);
  std::cout << c1 << '\n';

  // no conversion
  Boolean_expr e2 = build.get_true();
  Expr& c2 = convert(e2, b);
  std::cout << c2 << '\n';

  // bool-to-int
  Expr& c3 = convert(e2, z);
  std::cout << c3 << '\n';

  // int-to-bool
  Integer_expr e3 = build.get_int(0);
  Expr& c4 = convert(e3, b);
  std::cout << c4 << '\n';

  // object-to-value, bool-to-int
  Expr& c5 = convert(e1, z);
  std::cout << c5 << '\n';

  Expr& c6 = convert(e3, cz);
  std::cout << c6 << '\n';

  Expr& c7 = convert(e1, cz);
  std::cout << c7 << '\n';

}
