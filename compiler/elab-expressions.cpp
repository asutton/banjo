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


void
Elaborate_expressions::on_variable_initializer(Expression_def& d)
{
  d.expr_ = &get_expr(d.expression());
  debug(d.expression());
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
