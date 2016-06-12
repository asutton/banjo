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
  Def& d = cxt.make_variable_initializer();
  return on_variable_declaration(n, t, d);
}


Decl&
Parser::on_variable_declaration(Name& n, Type& t, Expr& e)
{
  Def& d = cxt.make_variable_initializer(e);
  return on_variable_declaration(n, t, d);
}


Decl&
Parser::on_variable_declaration(Name& n, Type& t, Def& d)
{
  Decl* ret;
  if (parsing_nonstatic_member())
    ret = &cxt.make_field_declaration(n, t, d);
  else
    ret = &cxt.make_variable_declaration(n, t, d);
  ret->spec_ = take_decl_specs();
  declare(cxt, *ret);
  return *ret;
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
  lingo_unreachable();
  // Decl& parm = build.make_type_parameter(n, t);
  // declare(cxt, current_scope(), parm);
  // return parm;
}


// -------------------------------------------------------------------------- //
// Concepts

Decl&
Parser::on_concept_declaration(Token, Name& n, Decl_list& ps)
{
  lingo_unreachable();
  // Decl& decl = build.make_concept(n, ps);
  // declare(cxt, current_scope(), decl);
  // return decl;
}


Def&
Parser::on_concept_definition(Decl& decl, Expr& e)
{
  lingo_unreachable();
  // Def& def = build.make_expression_definition(e);
  // define_concept(decl, def);
  // return def;
}


Def&
Parser::on_concept_definition(Decl& decl, Req_list& ds)
{
  lingo_unreachable();
  // Def& def = build.make_concept_definition(ds);
  // define_concept(decl, def);
  // return def;
}


} // namespace fe

} // namespace banjo
