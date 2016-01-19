// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/conversion.hpp>

#include <iostream>
#include <iomanip>


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
test_similar(Context& cxt)
{
  Builder build(cxt);

  std::cout << "--- qualifier similarity --" << '\n';

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
test_standard_conversions(Context& cxt)
{
  Builder build(cxt);

  std::cout << "--- standard conversions --" << '\n';

  Type& b = build.get_bool_type();
  Type& z = build.get_int_type();
  Type& cz = build.get_const_type(z);

  Variable_decl& v1 = build.make_variable("v1", b);

  // FIXME: How should we go about testing conversion sequences?

  // bool& ~> bool
  Reference_expr e1 = build.make_reference(v1);
  Expr& c1 = standard_conversion(e1, b);
  std::cout << c1 << '\n';
  Conversion_seq s1 = get_conversion_sequence(c1);
  assert(s1.kind() == std_conv_seq);

  // no conversion
  Boolean_expr e2 = build.get_true();
  Expr& c2 = standard_conversion(e2, b);
  std::cout << c2 << '\n';
  Conversion_seq s2 = get_conversion_sequence(c1);
  assert(s2.kind() == std_conv_seq);

  // bool-to-int
  Expr& c3 = standard_conversion(e2, z);
  std::cout << c3 << '\n';

  // int-to-bool
  Integer_expr e3 = build.get_int(0);
  Expr& c4 = standard_conversion(e3, b);
  std::cout << c4 << '\n';

  // bool& ~> int
  Expr& c5 = standard_conversion(e1, z);
  std::cout << c5 << '\n';

  // int ~> int const
  Expr& c6 = standard_conversion(e3, cz);
  std::cout << c6 << '\n';

  // bool& ~> int const
  Expr& c7 = standard_conversion(e1, cz);
  std::cout << c7 << '\n';

  // int const -> int
  Expr& e4 = build.get_integer(cz, 1);
  Expr& c8 = standard_conversion(e4, z);
  std::cout << c8 << '\n';
}


void
test_arithmetic_conversions(Context& cxt)
{
  Builder build(cxt);

  std::cout << "--- arithmetic conversions --" << '\n';

  Type& i16 = build.get_integer_type(true, 16);
  Type& i32 = build.get_integer_type(true, 32);
  Type& u32 = build.get_integer_type(false, 32);

  Expr& z16 = build.get_integer(i16, 0);
  Expr& z32 = build.get_integer(i32, 1);
  Expr& n32 = build.get_integer(u32, 1);

  Expr_pair p1 = arithmetic_conversion(z16, z32);
  std::cout << p1.first << " ## " << p1.second << '\n';

  Expr_pair p2 = arithmetic_conversion(n32, z32);
  std::cout << p2.first << " ## " << p2.second << '\n';

  // TODO: Fully exhaust all of the different testing rules.
}


int
main(int argc, char* argv[])
{
  Context cxt;

  test_similar(cxt);
  test_standard_conversions(cxt);
  test_arithmetic_conversions(cxt);
}
