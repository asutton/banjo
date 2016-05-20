// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>
#include <banjo/declaration.hpp>


namespace banjo
{

namespace fe
{

// Create a saved scope for the function body and add the parameters 
// to it. This will allow parameter names to be looked up during
// subsequent elaboration. Note that name hiding will also be diagnosed
// later.
static void
declare_parms(Context& cxt, Term& t, Decl_list& parms)
{
  Enter_scope scope(cxt, cxt.saved_scope(t));
  for (Decl& p : parms)
    declare(cxt, p);
}


// Build and declare the function or method. A method is a non-static
// function declared in class scope.
Decl&
Parser::start_function_declaration(Name& n, Decl_list& p, Type& t)
{
  Decl& decl = parsing_nonstatic_member()
    ? cxt.make_method_declaration(n, p, t)
    : cxt.make_function_declaration(n, p, t);
  decl.spec_ = take_decl_specs();
  declare(cxt, decl);
  return decl;
}


// Declare parameters and update the definition.
Decl&
Parser::finish_function_declaration(Decl& decl, Expr& expr)
{
  Function_decl& fn = cast<Function_decl>(decl);
  declare_parms(cxt, expr, fn.parameters());
  fn.def_ = &cxt.make_function_definition(expr);
  return decl;
}


// Declare parameters and update the definition.
Decl&
Parser::finish_function_declaration(Decl& decl, Stmt& stmt)
{
  Function_decl& fn = cast<Function_decl>(decl);
  declare_parms(cxt, stmt, fn.parameters());
  fn.def_ = &cxt.make_function_definition(stmt);
  return decl;
}


// TODO: Declare parameters as we do above.
Decl&
Parser::on_coroutine_declaration(Name& n, Decl_list& p, Type& t, Stmt& s)
{
  Decl& d = build.make_coroutine_declaration(n, p, t, s);
  declare(cxt, current_scope(), d);
  return d;
}


// In the first pass, just create the parameter. It is declared into the
// function body.
Decl&
Parser::on_function_parameter(Name& n, Type& t)
{
  Decl& d = build.make_object_parm(n, t);
  d.spec_ = take_decl_specs();
  return d;
}


} // namespace fe

} // namespace banjo
