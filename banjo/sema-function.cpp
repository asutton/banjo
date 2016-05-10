// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{


// Returns true if it looks like we're declaring a non-static member
// of a user-defined type.
//
// FIXME: This appears in sema-decl.cpp also.
inline static bool
declaring_member(Parser& p)
{
  bool in_udt = is<Class_decl>(p.cxt.immediate_context());
  bool is_static = p.decl_specs() & static_spec;
  return in_udt && !is_static;
}


Decl&
Parser::on_function_declaration(Name& n, Decl_list& p, Type& t, Expr& e)
{
  Decl* ret;
  if (declaring_member(*this))
    ret = &cxt.make_method_declaration(n, p, t, e);
  else
    ret = &cxt.make_function_declaration(n, p, t, e);
  ret->spec_ = take_decl_specs();
  declare(cxt, *ret);
  return *ret;
}


Decl&
Parser::on_function_declaration(Name& n, Decl_list& p, Type& t, Stmt& s)
{
  Decl* ret;
  if (declaring_member(*this))
    ret = &cxt.make_method_declaration(n, p, t, s);
  else
    ret = &cxt.make_function_declaration(n, p, t, s);
  ret->spec_ = take_decl_specs();
  declare(cxt, *ret);
  return *ret;
}


// In the first pass, just create the parameter. We'll declare it
// during elaboration of the function's definition.
Decl&
Parser::on_function_parameter(Name& n, Type& t)
{
  Decl& d = build.make_object_parm(n, t);
  d.spec_ = take_decl_specs();
  return d;
}


// FIXME: These should go away.

Def&
Parser::on_function_definition(Decl& d, Stmt& s)
{
  // Def& def = build.make_function_definition(s);
  // return define_function(d, def);
  lingo_unreachable();
}


Def&
Parser::on_deleted_definition(Decl& d)
{
  // Def& def = build.make_deleted_definition();
  // return define_entity(d, def);
  lingo_unreachable();
}


Def&
Parser::on_defaulted_definition(Decl& d)
{
  // Def& def = build.make_defaulted_definition();
  // return define_function(d, def);
  lingo_unreachable();
}


Decl&
Parser::on_coroutine_declaration(Name& n,Decl_list& p, Type& t, Stmt& s)
{
  Decl& d = build.make_coroutine_declaration(n,p,t,s);
  declare(cxt, current_scope(), d);
  return d;
}


} // namespace banjo
