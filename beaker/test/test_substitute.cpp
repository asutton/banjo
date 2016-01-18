// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <beaker/substitution.hpp>

#include <iostream>


template<typename T>
void
test_subst(Context& cxt, T& t, Substitution& sub)
{
  T& s = substitute(cxt, t, sub);
  std::cout << t << " ~> " << s << '\n';
}


void
test_subst_type(Context& cxt)
{
  Builder build(cxt);

  // Substitutable type and an argument.
  Decl& parm = build.make_type_parameter("T");
  Type& arg = build.get_int_type();
  Substitution sub;
  sub.put(parm, arg);

  // Substitution patterns
  Type& t0 = build.get_typename_type(parm);
  test_subst(cxt, t0, sub);

  Type& t1 = build.get_reference_type(t0);
  test_subst(cxt, t1, sub);

  Type& t2 = build.get_pointer_type(build.get_pointer_type(t0));
  test_subst(cxt, t2, sub);

  Type& t3 = build.get_function_type({&t1, &t2}, arg);
  test_subst(cxt, t3, sub);
}



int
main(int argc, char* argv[])
{
  Context cxt;
  test_subst_type(cxt);
}
