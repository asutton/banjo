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
    // Declaration specifiers.
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
    case coroutine_tok:
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
    
    case for_tok:
      return for_statement();

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
  // Build the compound statement so that we can save its scope.
  Stmt& s = start_compound_statement();
  Enter_scope scope(cxt, s);

  // Match the enclosed statements.
  Stmt_list ss;
  match(lbrace_tok);
  if (lookahead() != rbrace_tok)
    ss = statement_seq();
  match(rbrace_tok);
  return finish_compound_statement(s, std::move(ss));
}


// Parse a return statement.
//
//    return-statement:
//      'return' ';'
//      'return' expression ';'
Stmt&
Parser::return_statement()
{
  Match_token_pred end_expr(*this, semicolon_tok);

  Token tok = require(return_tok);
  Expr& e = unparsed_expression(end_expr);
  match(semicolon_tok);
  return on_return_statement(tok, e);
}


// Parse a yield statement.
//
//    yield-statement:
//      'yield' ';'
//      'yield' expression ';'
Stmt&
Parser::yield_statement()
{
  Match_token_pred end_expr(*this, semicolon_tok);
  
  Token tok = require(yield_tok);
  Expr&e = unparsed_expression(end_expr);
  match(semicolon_tok);
  return on_yield_statement(tok, e);
}


// Parse an if statement.
//
//    if-statement:
//      'if' '(' condition ')' statement
//      'if' '(' condition ')' statement 'else' statement
//
// TODO: Allow a declaration in the condition. This is not quite
// the same as a 'let' since the name is bound until the end of the
// statement.
Stmt&
Parser::if_statement()
{
  Match_token_pred end_cond(*this, rparen_tok);

  require(if_tok);
  match(lparen_tok);
  Expr& cond = unparsed_expression(end_cond);
  match(rparen_tok);
  Stmt& branch1 = statement();
  if (match_if(else_tok)) {
    Stmt& branch2 = statement();
    return on_if_statement(cond, branch1, branch2);
  } else {
    return on_if_statement(cond, branch1);
  }
}


// Parse a while statement.
//
//    while-statement:
//      'while' '(' condition ')' statement
//
// TODO: Allow a declaration in the condition?
Stmt&
Parser::while_statement()
{
  Match_token_pred end_cond(*this, rparen_tok);
  
  require(while_tok);
  match(lparen_tok);
  Expr& cond = unparsed_expression(end_cond);
  match(rparen_tok);
  Stmt& body = statement();
  return on_while_statement(cond, body);
}


// Parse a for statement.
//
//    for-statement:
//      'for' '(' variable-declaration [condition] ';' [expression] ')' statement
//      'for' '(' expression-statement [condition] ';' [expression] ')' statement
//      'for' '(' parameter-declaration 'in' expression ')' statement
//
// TODO: Nail down the parse semantics for the for loop. We can probably
// find some way of unifying the first term.
//
// TODO: Implement a parse for the range-based for. This will require
// lookahead for the first non-nested ';' in order to determine if
// we have a range-for or a normal for.
Stmt&
Parser::for_statement()
{
  lingo_unimplemented("for loop");
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
//
//    declaration-statement:
//      declaration
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
  Match_token_pred end_expr(*this, semicolon_tok);
  Expr& e = unparsed_expression(end_expr);
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
  } while (next_token_is_not(rbrace_tok));
  
  // Process statements in the block?
  on_statement_seq(ss);
  
  return ss;
}


} // namespace banjo
