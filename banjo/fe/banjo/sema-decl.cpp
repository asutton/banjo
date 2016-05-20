// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>
#include <banjo/declaration.hpp>


namespace banjo
{

namespace fe
{

// -------------------------------------------------------------------------- //
// Variables

Decl&
Parser::on_variable_declaration(Name& n, Type& t)
{
  Decl* ret;
  if (parsing_nonstatic_member())
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
  if (parsing_nonstatic_member())
    ret = &cxt.make_field_declaration(n, t, e);
  else
    ret = &cxt.make_variable_declaration(n, t, e);
  ret->spec_ = take_decl_specs();
  declare(cxt, *ret);
  return *ret;
}


// -------------------------------------------------------------------------- //
// Constants


Decl&
Parser::on_constant_declaration(Name& n, Type& t, Expr& e)
{
  // A constant declaration has static storage.
  //
  // TODO: Make this implicit in its construction or not?
  decl_specs() |= static_spec;

  // Build the declaration.
  Decl& decl = cxt.make_constant_declaration(n, t, e);
  decl.spec_ = take_decl_specs();

  declare(cxt, decl);
  return decl;
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


} // namespace fe

} // namespace banjo
