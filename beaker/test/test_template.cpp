// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


// A symbol table.
Symbol_table syms;


inline Typename_type&
make_typename_type(Type_parm& p)
{
  return *new Typename_type(p);
}


inline Type_parm&
make_type_parameter(char const* n)
{
  return *new Type_parm(make_id(n));
}


inline Value_parm&
make_value_parameter(char const* n, Type& t)
{
  return *new Value_parm(make_id(n), t);
}


inline Template_decl&
make_template(Decl_list const& p, Decl& d)
{
  return *new Template_decl(p, d);
}


int
main(int argc, char* argv[])
{
  init_tokens();

  {
    Type_parm& p1 = make_type_parameter("T");
    Type_parm& p2 = make_type_parameter("U");
    Value_parm& p3 = make_value_parameter("N", make_int_type());

    Decl& var = make_variable("v1", make_typename_type(p1));
    Decl& tmp = make_template({&p1, &p2, &p3}, var);
    std::cout << tmp << '\n';
  }

  {
    Type_parm& p = make_type_parameter("T");
    Decl& var = make_variable("v2", make_typename_type(p));
    Template_decl& tmp = make_template({&p}, var);
    tmp.constraint(make_true());
    std::cout << tmp << '\n';
  }
}
