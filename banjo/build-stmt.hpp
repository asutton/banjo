// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILD_STMT_HPP
#define BANJO_BUILD_STMT_HPP

#include "build-core.hpp"


namespace banjo
{

// The statement builder creates statement nodes.
struct Stmt_builder : Builder_base
{
  using Builder_base::Builder_base;

  Empty_stmt&        make_empty_statement();
  Compound_stmt&     make_compound_statement();
  Compound_stmt&     make_compound_statement(Stmt_list&&);
  Return_stmt&       make_return_statement();
  Return_value_stmt& make_return_value_statement(Expr&);
  Yield_stmt&        make_yield_statement();
  Yield_value_stmt&  make_yield_value_statement(Expr&);
  If_then_stmt&      make_if_statement(Expr&, Stmt&);
  If_else_stmt&      make_if_statement(Expr&, Stmt&, Stmt&);
  While_stmt&        make_while_statement(Expr&, Stmt&);
  Break_stmt&        make_break_statement();
  Continue_stmt&     make_continue_statement();
  Expression_stmt&   make_expression_statement(Expr&);
  Declaration_stmt&  make_declaration_statement(Decl&);

  Basic_factory<Empty_stmt> empty_stmt;
  Basic_factory<Compound_stmt> compound_stmt;
  Basic_factory<Return_stmt> return_stmt;
  Basic_factory<Return_value_stmt> return_value_stmt;
  Basic_factory<Yield_stmt> yield_stmt;
  Basic_factory<Yield_value_stmt> yield_value_stmt;
  Basic_factory<If_then_stmt> if_then_stmt;
  Basic_factory<If_else_stmt> if_else_stmt;
  Basic_factory<While_stmt> while_stmt;
  Basic_factory<Break_stmt> break_stmt;
  Basic_factory<Continue_stmt> continue_stmt;
  Basic_factory<Expression_stmt> expression_stmt;
  Basic_factory<Declaration_stmt> declaration_stmt;
};


} // namespace banjo


#endif
