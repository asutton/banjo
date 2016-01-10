// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


int
main(int argc, char* argv[])
{
  Context cxt;
  Builder build(cxt);

  {
    Type_parm& p1 = build.make_type_parm("T");
    Type_parm& p2 = build.make_type_parm("U");
    Value_parm& p3 = build.make_value_parm("N", build.get_int_type());

    Decl& var = build.make_variable("v1", build.get_typename_type(p1));
    Decl& tmp = build.make_template({&p1, &p2, &p3}, var);
    std::cout << tmp << '\n';
  }

  {
    Type_parm& p = build.make_type_parm("T");
    Decl& var = build.make_variable("v2", build.get_typename_type(p));
    Template_decl& tmp = build.make_template({&p}, var);
    tmp.constraint(build.get_true());
    std::cout << tmp << '\n';
  }
}
