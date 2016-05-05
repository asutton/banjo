// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Supers
Decl&
Parser::on_super_declaration(Name& n, Type& t)
{
  Decl& d = cxt.make_super_declaration(t);
  declare(cxt, d);
  return d;
}


// Returns true if it looks like we're declaring a non-static member
// of a user-defined type.
inline static bool
declaring_member(Parser& p)
{
  bool in_udt = is<Type_decl>(p.cxt.immediate_context());
  bool is_static = p.decl_specs() & static_spec;
  return in_udt && !is_static;
}


// -------------------------------------------------------------------------- //
// Variables

Decl&
Parser::on_variable_declaration(Name& n, Type& t)
{
  Decl* ret;
  if (declaring_member(*this))
    ret = &cxt.make_field_declaration(n, t);
  else
    ret = &cxt.make_variable_declaration(n, t);
  ret->spec_ = take_decl_specs();

  declare(cxt, *ret);
  return *ret;
}


// FIXME: There's a lot of duplication hereabouts. Can we reduce it?
Decl&
Parser::on_variable_declaration(Name& n, Type& t, Expr& e)
{
  Decl* ret;
  if (declaring_member(*this))
    ret = &cxt.make_field_declaration(n, t, e);
  else
    ret = &cxt.make_variable_declaration(n, t, e);
  ret->spec_ = take_decl_specs();
  declare(cxt, *ret);
  return *ret;
}




// -------------------------------------------------------------------------- //
// Functions

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


// -------------------------------------------------------------------------- //
// Types

Decl&
Parser::on_class_declaration(Name& n, Type& t, Stmt& s)
{
  Decl& d = build.make_class_declaration(n, t, s);
  declare(cxt, current_scope(), d);
  return d;
}

// -------------------------------------------------------------------------- //
// Coroutine
Decl&
Parser::on_coroutine_declaration(Name& n,Decl_list& p, Type& t, Stmt& s)
{
  Decl& d = build.make_coroutine_declaration(n,p,t,s);
  declare(cxt, current_scope(), d);
  return d;
}

// -------------------------------------------------------------------------- //
// Templates

Decl&
Parser::on_type_template_parameter(Name& n)
{
  Decl& parm = build.make_type_parameter(n);
  declare(cxt, current_scope(), parm);
  return parm;
}


Decl&
Parser::on_type_template_parameter(Name& n, Type& t)
{
  Decl& parm = build.make_type_parameter(n, t);
  declare(cxt, current_scope(), parm);
  return parm;
}


// -------------------------------------------------------------------------- //
// Concepts

static inline void
define_concept(Decl& decl, Def& def)
{
  Concept_decl& con = cast<Concept_decl>(decl);
  con.def = &def;
}


Decl&
Parser::on_concept_declaration(Token, Name& n, Decl_list& ps)
{
  Decl& decl = build.make_concept(n, ps);
  declare(cxt, current_scope(), decl);
  return decl;
}


Def&
Parser::on_concept_definition(Decl& decl, Expr& e)
{
  Def& def = build.make_expression_definition(e);
  define_concept(decl, def);
  return def;
}


Def&
Parser::on_concept_definition(Decl& decl, Req_list& ds)
{
  Def& def = build.make_concept_definition(ds);
  define_concept(decl, def);
  return def;
}


} // namespace banjo
