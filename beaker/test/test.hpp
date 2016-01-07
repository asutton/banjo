
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include <beaker/ast.hpp>
#include <beaker/lexer.hpp>
#include <beaker/print.hpp>


using namespace lingo;
using namespace beaker;


// A symbol table. This must be defined in each unit test (until
// we have a library that exports it).
extern Symbol_table syms;


// Create an identifier and return its symbol.
inline Name*
make_id(char const* n)
{
  Symbol* sym = syms.put_identifier(identifier_tok, n);
  return new Simple_id(sym);
}


inline Namespace_decl*
make_global_ns() { return new Namespace_decl(new Global_id()); }


inline Boolean_type*
make_bool_type() { return new Boolean_type(); }


inline Integer_type*
make_int_type() { return new Integer_type(); }


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
  Init* i = new Incomplete_init();
  return new Function_decl(cxt, make_id(n), t, ps, i);
}
