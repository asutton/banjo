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
//      empty-statement
//      return-statement
//      if-statement
//      while-statement
//      break-statement
//      continue-statement
//      compound-statement
//      declaration-statement
//      expression-statement
Stmt&
Parser::statement()
{
  switch (lookahead()) {
    // Declaration specifiers start declarations.
    case virtual_tok:
    case abstract_tok:
    case static_tok:
    case inline_tok:
    case explicit_tok:
    case implicit_tok:
    case public_tok:
    case private_tok:
    case protected_tok:
    // Declaration introducers.
    case super_tok:
    case var_tok:
    case def_tok:
    case coroutine_tok: // co_def
    case class_tok:
    case concept_tok:
      return declaration_statement();

    case return_tok:
      return return_statement();

    case yield_tok:
      return yield_statement();

    case if_tok:
      return if_statement();

    case while_tok:
      return while_statement();

    case break_tok:
      return break_statement();

    case continue_tok:
      return continue_statement();

    case lbrace_tok:
      return compound_statement();

    case semicolon_tok:
      return empty_statement();

    default:
      return expression_statement();
  }
}


// Parse the empty statement.
//
//    empty-statement:
//      ';'
Stmt&
Parser::empty_statement()
{
  require(semicolon_tok);
  return on_empty_statement();
}


// Parse a compound statement.
//
//    compound-statement:
//      '{' [statement-seq] '}'
//
// A compound statement defines a new block scope.
Stmt&
Parser::compound_statement()
{
  Enter_scope scope(cxt);
  Stmt_list ss;
  match(lbrace_tok);
  if (lookahead() != rbrace_tok)
    ss = statement_seq();
  match(rbrace_tok);
  return on_compound_statement(std::move(ss));
}


// Parse a member statement, which represents the body of a class.
//
//    member-statement:
//      '{' [statement-seq] '}'
//
// Note that that the scope into which declarations are added is
// pushed prior to the parsing of the member statement.
Stmt&
Parser::member_statement()
{
  Stmt_list ss;
  match(lbrace_tok);
  if (lookahead() != rbrace_tok)
    ss = statement_seq();
  match(rbrace_tok);
  return on_member_statement(std::move(ss));
}


Stmt&
Parser::return_statement()
{
  Token tok = require(return_tok);
  Expr& e = expression();
  match(semicolon_tok);
  return on_return_statement(tok, e);
}

Stmt&
Parser::yield_statement()
{
  Token tok = require(yield_tok);
  Expr&e = expression();
  match(semicolon_tok);
  return on_yield_statement(tok, e);
}

// Parse an if statement.
//
//    if-statement:
//      'if' '(' expression ')' statement
//      'if' '(' expression ')' statement 'else' statement
Stmt&
Parser::if_statement()
{
  require(if_tok);
  match(lparen_tok);
  Expr& cond = expression();
  match(rparen_tok);
  Stmt& branch1 = statement();
  if (match_if(else_tok)) {
    Stmt& branch2 = statement();
    return on_if_statement(cond, branch1, branch2);
  } else {
    return on_if_statement(cond, branch1);
  }
}


Stmt&
Parser::while_statement()
{
  require(while_tok);
  match(lparen_tok);
  Expr& cond = expression();
  match(rparen_tok);
  Stmt& body = statement();
  return on_while_statement(cond, body);
}


Stmt&
Parser::break_statement()
{
  require(break_tok);
  match(semicolon_tok);
  return on_break_statement();
}


Stmt&
Parser::continue_statement()
{
  require(continue_tok);
  match(semicolon_tok);
  return on_continue_statement();
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
// NOTE: In general, we expect that a new scope has been pushed prior
// to the parsing of the nested statements.
Stmt_list
Parser::statement_seq()
{
  // First pass: collect declarations
  Stmt_list ss;
  do {
    Stmt& s = statement();
    ss.push_back(s);
  } while (!is_eof() && next_token_is_not(rbrace_tok));
  on_statement_seq(ss);
  return ss;
}


} // namespace banjo
