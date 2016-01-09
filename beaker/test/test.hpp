
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include <beaker/ast.hpp>
#include <beaker/lexer.hpp>
#include <beaker/print.hpp>

#include <iostream>


using namespace lingo;
using namespace beaker;


// Create an identifier and return its symbol.
inline Name*
make_id(char const* n)
{
  Symbol const* sym = symbols.put_identifier(identifier_tok, n);
  return new Simple_id(sym);
}


inline Namespace_decl*
make_global_ns() { return new Namespace_decl(new Global_id()); }


inline Boolean_type*
make_bool_type()
{
  static Boolean_type t;
  return &t;
}


inline Integer_type*
make_int_type()
{
  static Integer_type t;
  return &t;
}


inline Boolean_expr*
make_bool(bool b)
{
  Symbol const* sym = symbols.get(b ? "true" : "false");
  return new Boolean_expr(make_bool_type(), sym);
}


inline Boolean_expr*
make_true() { return make_bool(true); }


inline Boolean_expr*
make_false() { return make_bool(false); }


inline Integer_expr*
make_integer(int n, Type*)
{
  std::stringstream ss;
  ss << n;
  Symbol const* sym = symbols.put_integer(integer_tok, ss.str(), n);
  return new Integer_expr(make_int_type(), sym);
}


inline Variable_decl*
make_variable(Decl* cxt, char const* n, Type* t)
{
  return new Variable_decl(cxt, make_id(n), t, new Default_init());
}


inline Function_type*
make_function_type(Decl_list& ps, Type* r)
{
  Type_list ts;
  for (Decl& d : ps) {
    Object_parm& p = cast<Object_parm>(d);
    ts.push_back(&p.type());
  }
  return new Function_type(ts, r);
}


inline Object_parm*
make_parameter(char const* n, Type* t)
{
  return new Object_parm(make_id(n), t, new Default_init());
}


inline Function_decl*
make_function(Decl* cxt, char const* n, Decl_list& ps, Type* r)
{
  Type* t = make_function_type(ps, r);
  return new Function_decl(cxt, make_id(n), t, ps, nullptr);
}
