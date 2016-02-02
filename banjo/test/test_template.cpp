// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/template.hpp>
#include <banjo/substitution.hpp>

#include <iostream>


void
test_basics(Context& cxt)
{
  std::cout << "--- basics ---\n";

  Builder build(cxt);
  {
    Type_parm& p1 = build.make_type_parameter("T");
    Type_parm& p2 = build.make_type_parameter("U");
    Value_parm& p3 = build.make_value_parm("N", build.get_int_type());

    Decl& var = build.make_variable("v1", build.get_typename_type(p1));
    Decl& tmp = build.make_template({&p1, &p2, &p3}, var);
    std::cout << tmp << '\n';
  }

  {
    Type_parm& p = build.make_type_parameter("T");
    Decl& var = build.make_variable("v2", build.get_typename_type(p));
    Template_decl& tmp = build.make_template({&p}, var);
    tmp.constrain(build.get_true());
    std::cout << tmp << '\n';
  }
}


void
test_specialize(Context& cxt)
{
  std::cout << "--- specialization ---\n";
  Builder build(cxt);

  // Substitutable type and an argument.
  Decl& parm = build.make_type_parameter("T");
  Type& arg = build.get_int_type();
  Term_list args {&arg};

  // Substitution patterns
  Type& t = build.get_typename_type(parm);
  Type& p_t = build.get_pointer_type(t);

  Decl& v1 = build.make_variable("v1", p_t);
  Template_decl& tv1 = build.make_template({&parm}, v1);
  std::cout << tv1 << "\n   vvvv\n";
  Decl& ts1 = specialize_template(cxt, tv1, args);
  std::cout << ts1 << '\n';
}


void
test_synthesis(Context& cxt)
{
  std::cout << "--- ordering ---\n";
  Builder build(cxt);

  Type_parm& tp1 = build.make_type_parameter("T");
  Type_parm& tp2 = build.make_type_parameter("U");
  Type& t1 = build.get_typename_type(tp1);
  Type& t2 = build.get_typename_type(tp2);
  Type& rt1 = build.get_reference_type(t1);

  Object_parm p1 = build.make_object_parm("a", t1);
  Object_parm p2 = build.make_object_parm("b", t2);
  Object_parm p3 = build.make_object_parm("c", rt1);

  Decl& f1 = build.make_function("f", {&p1, &p2}, t1);
  Template_decl& tmp1 = build.make_template({&tp1, &tp2}, f1);

  Decl& f2 = build.make_function("f", {&p3, &p2}, t1);
  Template_decl& tmp2 = build.make_template({&tp1, &tp2}, f2);

  std::cout << "tmp1\n" << tmp1 << '\n';
  std::cout << "tmp2\n" << tmp2 << '\n';
  std::cout << is_more_specialized(cxt, tmp1, tmp2) << ' '
            << is_more_specialized(cxt, tmp2, tmp1) << '\n';
}


int
main(int argc, char* argv[])
{
  Context cxt;

  test_basics(cxt);
  test_specialize(cxt);
  test_synthesis(cxt);
}
