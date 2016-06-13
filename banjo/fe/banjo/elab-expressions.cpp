// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "elab-expressions.hpp"
#include "parser.hpp"

#include <banjo/ast.hpp>
#include <banjo/declaration.hpp>
#include <banjo/deduction.hpp>
#include <banjo/initialization.hpp>
#include <banjo/evaluation.hpp>
#include <banjo/debugging.hpp>


namespace banjo
{

namespace fe
{


void
Elaborate_expressions::on_return_statement(Return_value_stmt& s)
{
  s.expr_ = &get_expr(s.expression());

  // TODO: Returning a value is initialization. We need to ensure that
  // that the returned value is moved (consumed) into the declared value.
}


void
Elaborate_expressions::on_yield_statement(Yield_value_stmt& s)
{
  s.expr_ = &get_expr(s.expression());
}


void
Elaborate_expressions::on_if_statement(If_then_stmt& s)
{
  s.cond_ = &get_expr(s.condition());
}


void
Elaborate_expressions::on_if_statement(If_else_stmt& s)
{
  s.cond_ = &get_expr(s.condition());
}


void
Elaborate_expressions::on_while_statement(While_stmt& s)
{
  s.cond_ = &get_expr(s.condition());
}


void
Elaborate_expressions::on_expression_statement(Expression_stmt& s)
{
  s.expr_ = &get_expr(s.expression());
}


// -------------------------------------------------------------------------- //
// Declarations

// Perform a series of checks and transformations on the declared
// type of d to yield a complete, deduced type.
template<typename Init>
static Type&
get_complete_type(Context& cxt, Init& decl)
{
  deduce_declared_type(cxt, decl, decl.initializer());
  
  // Ensure the completeness of the type's definition. Most types are
  // always complete. Class types are complete only when they have been
  // fully elaborated. The completeness of array and tuple types depend
  // on the completeness of their members, etc.
  //
  // FIXME: Implement this.

  return decl.type();
}



void
Elaborate_expressions::on_variable_declaration(Variable_decl& d)
{
  // debug(d.initializer());
}


void
Elaborate_expressions::on_function_definition(Expression_def& d)
{
  d.expr_ = &get_expr(d.expression());
}


// -------------------------------------------------------------------------- //
// Expressions

// Parse the expression as needed, returning its fully elaborated form.
Expr&
Elaborate_expressions::get_expr(Expr& e)
{
  if (Unparsed_expr* u = as<Unparsed_expr>(&e)) {
    Save_input_location loc(cxt);
    Token_stream ts(u->tokens());
    Parser p(cxt, ts);
    return p.expression();    
  }
  return e;
}


} // namespace fe

} // namespace banjo
