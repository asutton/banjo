// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>
#include <banjo/debugging.hpp>


namespace banjo
{

namespace fe
{

// Build an empty compound statement.
Stmt&
Parser::start_compound_statement()
{
  return cxt.make_compound_statement();
}


// Assign the set of statements to the compound statement.
Stmt&
Parser::finish_compound_statement(Stmt& s, Stmt_list&& ss)
{
  Compound_stmt& cs = cast<Compound_stmt>(s);
  cs.stmts_ = std::move(ss);
  return cs;
}


Stmt&
Parser::on_empty_statement()
{
  return cxt.make_empty_statement();
}


Stmt&
Parser::on_return_statement(Token)
{
  return cxt.make_return_statement();
}


Stmt&
Parser::on_return_statement(Token, Expr& e)
{
  return cxt.make_return_statement(e);
}


Stmt&
Parser::on_yield_statement(Token)
{
  return cxt.make_yield_statement();
}


Stmt&
Parser::on_yield_statement(Token, Expr& e)
{
 return cxt.make_yield_statement(e);
}


Stmt&
Parser::on_if_statement(Expr& e, Stmt& s)
{
  return cxt.make_if_statement(e, s);
}


Stmt&
Parser::on_if_statement(Expr& e, Stmt& s1, Stmt& s2)
{
  return cxt.make_if_statement(e, s1, s2);
}


Stmt&
Parser::on_while_statement(Expr& e, Stmt& s)
{
  return cxt.make_while_statement(e, s);
}


Stmt&
Parser::on_break_statement()
{
  return cxt.make_break_statement();
}


Stmt&
Parser::on_continue_statement()
{
  return cxt.make_continue_statement();
}


Stmt&
Parser::on_declaration_statement(Decl& d)
{
  return cxt.make_declaration_statement(d);
}


Stmt&
Parser::on_expression_statement(Expr& d)
{
  return cxt.make_expression_statement(d);
}


Stmt&
Parser::on_unparsed_statement(Token_seq&& toks)
{
  // FIXME: Use the builder.
  return *new Unparsed_stmt(std::move(toks));
}


// NOTE: This is probably the most interesting function in the parser
// because it provides the framework for making multiple passes over
// a sequence of statements and declarations. Note that this modifies
// the statement list in place.
void
Parser::on_statement_seq(Stmt_list& ss)
{
  // // Second pass: Resolve declared types.
  // elaborate_declarations(ss);

  // elaborate_partials(ss);
  // elaborate_overloads(ss);

  // // Third pass: Resolve definitions.
  // elaborate_definitions(ss);
}


} // namespace fe

} // namespace banjo
