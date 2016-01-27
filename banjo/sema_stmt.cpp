// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"


namespace banjo
{

// FIXME: Use the builder.
Compound_stmt&
Parser::on_compound_statement(Stmt_list const& ss)
{
  return *new Compound_stmt(ss);
}


// FIXME: Use the builder.
Declaration_stmt&
Parser::on_declaration_statement(Decl& d)
{
  return *new Declaration_stmt(d);
}


// FIXME: Use the builder.
Expression_stmt&
Parser::on_expression_statement(Expr& d)
{
  return *new Expression_stmt(d);
}


} // namespace banjo
