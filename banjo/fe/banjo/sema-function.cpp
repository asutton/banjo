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
Parser::start_function_declaration(Name& n, Decl_list& ps, Type& t)
{
  // Form the (initial) function type.
  Type_list ts;
  for (Decl& p : ps)
    ts.push_back(cast<Typed_decl>(p).type());
  Type& ft = cxt.get_function_type(std::move(ts), t);

  // Generate the declaration.
  Decl* d;
  if (parsing_nonstatic_member())
    d = &cxt.make_method_declaration(n, ft, ps);
  else
    d = &cxt.make_function_declaration(n, ft, ps);
  d->spec_ = take_decl_specs();
  declare(cxt, *d);
  return *d;
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


// In the first pass, just create the parameter. It is declared into the
// function body.
Decl&
Parser::on_function_parameter(Name& n, Type& t)
{
  Decl& d = build.make_variable_parameter(n, t);
  d.spec_ = take_decl_specs();
  return d;
}


} // namespace fe

} // namespace banjo
