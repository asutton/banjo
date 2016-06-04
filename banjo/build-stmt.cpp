// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "build-stmt.hpp"

#include <unordered_set>


namespace banjo
{

Empty_stmt&
Stmt_builder::make_empty_statement()
{
  return empty_stmt();
}


Compound_stmt&
Stmt_builder::make_compound_statement()
{
  return compound_stmt();
}


Compound_stmt&
Stmt_builder::make_compound_statement(Stmt_list&& ss)
{
  return compound_stmt(std::move(ss));
}


Return_stmt&
Stmt_builder::make_return_statement()
{
  return return_stmt();
}


Return_value_stmt&
Stmt_builder::make_return_value_statement(Expr& e)
{
  return return_value_stmt(e);
}


Yield_stmt&
Stmt_builder::make_yield_statement()
{
  return yield_stmt();
}


Yield_value_stmt&
Stmt_builder::make_yield_value_statement(Expr& e)
{
  return yield_value_stmt(e);
}


If_then_stmt&
Stmt_builder::make_if_statement(Expr& e, Stmt& s)
{
  return if_then_stmt(e, s);
}


If_else_stmt&
Stmt_builder::make_if_statement(Expr& e, Stmt& s1, Stmt& s2)
{
  return if_else_stmt(e, s1, s2);
}


While_stmt&
Stmt_builder::make_while_statement(Expr& e, Stmt& s)
{
  return while_stmt(e, s);
}


Break_stmt&
Stmt_builder::make_break_statement()
{
  return break_stmt();
}


Continue_stmt&
Stmt_builder::make_continue_statement()
{
  return continue_stmt();
}


Expression_stmt&
Stmt_builder::make_expression_statement(Expr& e)
{
  return expression_stmt(e);
}


Declaration_stmt&
Stmt_builder::make_declaration_statement(Decl& d)
{
  return declaration_stmt(d);
}

} // namespace banjo
