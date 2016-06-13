// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_FE_ELAB_EXPRESSIONS_HPP
#define BANJO_FE_ELAB_EXPRESSIONS_HPP

#include "elaboration.hpp"


namespace banjo
{

namespace fe
{

struct Parser;
struct Context;


// Recursively analyze unparsed expressions in the contexts in which
// they appear, and ensure that they are fully typed.
struct Elaborate_expressions : Basic_elaborator
{
  using Basic_elaborator::Basic_elaborator;

  // Bring hidden names into scope.
  using Basic_elaborator::on_return_statement;
  using Basic_elaborator::on_yield_statement;

  void on_return_statement(Return_value_stmt&);
  void on_yield_statement(Yield_value_stmt&);
  void on_if_statement(If_then_stmt&);
  void on_if_statement(If_else_stmt&);
  void on_while_statement(While_stmt&);
  void on_expression_statement(Expression_stmt&);

  void on_variable_declaration(Variable_decl&);
  void on_function_definition(Expression_def&);

  Expr& get_expr(Expr&);
};


} // namespace fe

} // nammespace banjo


#endif
