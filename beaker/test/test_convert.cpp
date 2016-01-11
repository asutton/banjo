// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <beaker/convert.hpp>

#include <iostream>
#include <iomanip>


using namespace lingo;
using namespace beaker;


Context* cxt;


bool
test_similarity(Type const& a, Type const& b)
{
  bool x = is_similar(a, b);
  std::cout << a << " ~ " << b << " == " << std::boolalpha << x << '\n';
  return x;
}


void
test_signature(Type const& t)
{
  Qualifier_list sig = get_qualification_signature(t);
  std::cout << t << " : " << '[';
  for (auto iter = sig.begin(); iter != sig.end(); ++iter) {
    int cv = *iter;
    if (cv & const_qual)
      std::cout << 'c';
    if (cv & volatile_qual)
      std::cout << 'v';
    if (cv == 0)
      std::cout << '0';
    if (std::next(iter) != sig.end())
      std::cout << ',';
  }
  std::cout << ']' << '\n';
}


void
test_similar()
{
  Builder build(*cxt);

  Type& b = build.get_bool_type();
  Type& cb = build.get_const_type(b);
  Type& p_b = build.get_pointer_type(b); // bool*
  Type& p_cb = build.get_pointer_type(cb); // bool const*
  Type& p_p_cb = build.get_pointer_type(p_cb); // bool const* *
  Type& cp_p_cb = build.get_const_type(p_p_cb); // bool const* *const

  test_similarity(b, b);
  test_similarity(b, cb);
  test_similarity(b, p_cb);
  test_similarity(p_b, p_cb);
  test_similarity(p_p_cb, cp_p_cb);
  test_similarity(p_p_cb, p_cb);

  test_signature(b);
  test_signature(cb);
  test_signature(p_b);
  test_signature(p_cb);
  test_signature(cp_p_cb);
}


void
test_convert()
{
  Builder build(*cxt);

  Type& b = build.get_bool_type();
  Type& z = build.get_int_type();
  Type& cz = build.get_const_type(z);

  Variable_decl& v1 = build.make_variable("v1", b);

  // object-to-value
  Reference_expr e1 = build.make_reference(v1);
  Expr& c1 = convert_to_type(e1, b);
  std::cout << c1 << '\n';

  // no conversion
  Boolean_expr e2 = build.get_true();
  Expr& c2 = convert_to_type(e2, b);
  std::cout << c2 << '\n';

  // bool-to-int
  Expr& c3 = convert_to_type(e2, z);
  std::cout << c3 << '\n';

  // int-to-bool
  Integer_expr e3 = build.get_int(0);
  Expr& c4 = convert_to_type(e3, b);
  std::cout << c4 << '\n';

  // bool& ~> int
  Expr& c5 = convert_to_type(e1, z);
  std::cout << c5 << '\n';

  // int ~> int const
  Expr& c6 = convert_to_type(e3, cz);
  std::cout << c6 << '\n';

  // bool& ~> int const
  Expr& c7 = convert_to_type(e1, cz);
  std::cout << c7 << '\n';

  // int const -> int
  Expr& e4 = build.get_integer(cz, 1);
  Expr& c8 = convert_to_type(e4, z);
  std::cout << c8 << '\n';
}


int
main(int argc, char* argv[])
{
  Context c;
  cxt = &c;

  test_similar();
  test_convert();
}
