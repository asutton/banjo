// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/template.hpp>
#include <banjo/substitution.hpp>

#include <iostream>


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


int
main(int argc, char* argv[])
{
  Context cxt;
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
    tmp.constraint(build.get_true());
    std::cout << tmp << '\n';
  }

  test_specialize(cxt);
}
