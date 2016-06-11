// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"

#include <unordered_set>


namespace banjo
{

Empty_stmt&
Builder::make_empty_statement()
{
  return Empty_stmt::make(alloc_);
}


Compound_stmt&
Builder::make_compound_statement()
{
  return Compound_stmt::make(alloc_);
}


Compound_stmt&
Builder::make_compound_statement(Stmt_list const& ss)
{
  return Compound_stmt::make(alloc_, ss);
}


Compound_stmt&
Builder::make_compound_statement(Stmt_list&& ss)
{
  return Compound_stmt::make(alloc_, std::move(ss));
}


Return_stmt&
Builder::make_return_statement()
{
  return Return_stmt::make(alloc_);
}


Return_value_stmt&
Builder::make_return_statement(Expr& e)
{
  return Return_value_stmt::make(alloc_, e);
}


Yield_stmt&
Builder::make_yield_statement()
{
  return Yield_stmt::make(alloc_);
}


Yield_value_stmt&
Builder::make_yield_statement(Expr& e)
{
  return Yield_value_stmt::make(alloc_, e);
}


If_then_stmt&
Builder::make_if_statement(Expr& e, Stmt& s)
{
  return If_then_stmt::make(alloc_, e, s);
}


If_else_stmt&
Builder::make_if_statement(Expr& e, Stmt& s1, Stmt& s2)
{
  return If_else_stmt::make(alloc_, e, s1, s2);
}


While_stmt&
Builder::make_while_statement(Expr& e, Stmt& s)
{
  return While_stmt::make(alloc_, e, s);
}


Break_stmt&
Builder::make_break_statement()
{
  return Break_stmt::make(alloc_);
}


Continue_stmt&
Builder::make_continue_statement()
{
  return Continue_stmt::make(alloc_);
}


Expression_stmt&
Builder::make_expression_statement(Expr& e)
{
  return Expression_stmt::make(alloc_, e);
}


Declaration_stmt&
Builder::make_declaration_statement(Decl& d)
{
  return Declaration_stmt::make(alloc_, d);
}

} // namespace banjo
