// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-stmt.hpp"

#include <iostream>

namespace banjo
{

// Parse a statement.
//
//    statement:
//      compound-statement
//      return-statement
//      ...
//      declaration-statement
Stmt&
Parser::statement()
{
  switch (lookahead()) {
    case var_tok:
    case def_tok:
    case type_tok:
    case concept_tok:
      return declaration_statement();

    case lbrace_tok:
      return compound_statement();
    case return_tok:
      return return_statement();

    default:
      return expression_statement();
  }
}


// Parse a compound statement.
//
//    compound-statement:
//      '{' [statement-seq] '}'
Stmt&
Parser::compound_statement()
{
  // A compound statement defines a new scope.
  Enter_scope(cxt, cxt.make_block_scope());

  Stmt_list ss;
  match(lbrace_tok);
  if (lookahead() != rbrace_tok)
    ss = statement_seq();
  match(rbrace_tok);
  return on_compound_statement(ss);
}


Stmt&
Parser::return_statement()
{
  Token tok = require(return_tok);
  Expr& e = expression();
  match(semicolon_tok);
  return on_return_statement(tok, e);
}


// Parse a declaration-statement.
Stmt&
Parser::declaration_statement()
{
  Decl& d = declaration();
  return on_declaration_statement(d);
}


// Parse an expression-statement.
//
//    expression-statement:
//      expression ';'
Stmt&
Parser::expression_statement()
{
  Expr& e = expression();
  Stmt& s = on_expression_statement(e);
  match(semicolon_tok);
  return s;
}


// Parse a sequence of statements.
//
//    statement-seq:
//      statement
//      statement-seq statement
//
// NOTE: Every time we parse a sequence of statements, we expect that
//  a new scope has been pushed.
Stmt_list
Parser::statement_seq()
{

  // First pass: collect declarations
  Stmt_list ss;
  do {
    Stmt& s = statement();
    ss.push_back(s);
  } while (!is_eof() && next_token_is_not(rbrace_tok));

  // Second pass: Resolve declared types.
  elaborate_declarations(ss);

  // Thrid pass. Elaborate definitions.
  // for (Stmt* s : ss)
  //   elaborate_definition (cxt, *s);

  return ss;
}


} // namespace banjo
