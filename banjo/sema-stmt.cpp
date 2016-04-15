// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast-stmt.hpp"

namespace banjo
{


Stmt&
Parser::on_translation_statement(Stmt_list&& ss)
{
  return build.make_translation_statement(std::move(ss));
}


Stmt&
Parser::on_compound_statement(Stmt_list&& ss)
{
  return build.make_compound_statement(std::move(ss));
}


Stmt&
Parser::on_member_statement(Stmt_list&& ss)
{
  return build.make_member_statement(std::move(ss));
}


Return_stmt&
Parser::on_return_statement(Token, Expr& e)
{
  // FIXME: Verify that the return statement matches the
  // declared type of the function.
  return build.make_return_statement(e);
}


Break_stmt&
Parser::on_break_statement()
{
  return build.make_break_statement();
}


Continue_stmt&
Parser::on_continue_statement()
{
  return build.make_continue_statement();
}


Declaration_stmt&
Parser::on_declaration_statement(Decl& d)
{
  return build.make_declaration_statement(d);
}


Expression_stmt&
Parser::on_expression_statement(Expr& d)
{
  return build.make_expression_statement(d);
}


Stmt&
Parser::on_unparsed_statement(Token_seq&& toks)
{
  return *new Unparsed_stmt(std::move(toks));
}


} // namespace banjo
