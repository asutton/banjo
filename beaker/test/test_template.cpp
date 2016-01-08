// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


// A symbol table.
Symbol_table syms;


inline Type_parm*
make_type_parameter(char const* n)
{
  return new Type_parm(make_id(n), new Incomplete_init());
}


inline Value_parm*
make_value_parameter(char const* n, Type* t)
{
  return new Value_parm(make_id(n), t, new Default_init());
}


inline Template_decl*
make_template(Decl* cxt, Decl_list const& p, Decl* d)
{
  return new Template_decl(p, d);
}


int
main(int argc, char* argv[])
{
  init_tokens();

  Namespace_decl* global = make_global_ns();

  {
    Type_parm* p1 = make_type_parameter("T");
    Type_parm* p2 = make_type_parameter("U");
    Value_parm* p3 = make_value_parameter("N", make_int_type());

    Decl* var = make_variable(nullptr, "v", new Typename_type(p1));
    Decl* tmp = make_template(global, {p1, p2, p3}, var);
    std::cout << *tmp << '\n';
  }

  {
    Type_parm* p1 = make_type_parameter("T");
    Type_parm* p2 = make_type_parameter("U");
    Value_parm* p3 = make_value_parameter("N", make_int_type());
    Expr* req = make_true();

    Decl* var = make_variable(nullptr, "v", new Typename_type(p1));
    auto* tmp = make_template(global, {p1, p2, p3}, var);
    tmp->constr = req;
    std::cout << *tmp << '\n';
  }

}
