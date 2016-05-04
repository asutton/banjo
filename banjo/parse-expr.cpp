// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"

#include <iostream>

namespace banjo
{


// Parse an expression.
//
//    expression:
//      logical-or-expression -- FIXME: Probably wrong
Expr&
Parser::expression()
{
  return logical_or_expression();
}


// Parse a logical-or expression.
//
//    logical-or-expression:
//      logical-and-expression
//      logical-or-expression '||' logical-and-expression
Expr&
Parser::logical_or_expression()
{
  Expr* e1 = &logical_and_expression();
  while (true) {
    if (Token tok = match_if(bar_bar_tok)) {
      Expr& e2 = logical_and_expression();
      e1 = &on_logical_or_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a logical-and expression.
//
//    logical-and-expression:
//      inclusive-or-expression
//      logical-and-expression '&&' inclusive-or-expression
//
Expr&
Parser::logical_and_expression()
{
  Expr* e1 = &inclusive_or_expression();
  while (true) {
    if (Token tok = match_if(amp_amp_tok)) {
      Expr& e2 = inclusive_or_expression();
      e1 = &on_logical_and_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a bitwise inclusive-or expression.
//
//    inclusive-or-expression:
//      exclusive-or-expression
//      inclusive-or-expression '|' exclusive-or-expression
//
// FIXME: This skips the bitwise expressions.
Expr&
Parser::inclusive_or_expression()
{
  Expr* e1 = &exclusive_or_expression();
  while (true) {
    if (Token tok = match_if(bar_tok)) {
      Expr& e2 = exclusive_or_expression();
      e1 = &on_or_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a bitwise exclusive-or expression.
//
//    exclusive-or-expression:
//      and-expression
//      exclusive-or-expression '^' and-expression
//
Expr&
Parser::exclusive_or_expression()
{
  Expr* e1 = &and_expression();
  while (true) {
    if (Token tok = match_if(caret_tok)) {
      Expr& e2 = and_expression();
      e1 = &on_xor_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a bitwise and expression.
//
//    exclusive-or-expression:
//      equality-expression
//      exclusive-or-expression '&' equality-expression
//
Expr&
Parser::and_expression()
{
  Expr* e1 = &equality_expression();
  while (true) {
    if (Token tok = match_if(amp_tok)) {
      Expr& e2 = equality_expression();
      e1 = &on_and_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse an equality expression.
//
//    equality-expression:
//      relational-expression:
//      equality-expression '==' relational-expression
//      equality-expression '!=' relational-expression
Expr&
Parser::equality_expression()
{
  Expr* e1 = &relational_expression();
  while (true) {
    if (Token tok = match_if(eq_eq_tok)) {
      Expr& e2 = relational_expression();
      e1 = &on_eq_expression(tok, *e1, e2);
    } else if (Token tok = match_if(bang_eq_tok)) {
      Expr& e2 = relational_expression();
      e1 = &on_ne_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a relational expression.
//
//    relational-expression:
//      shift-expression:
//      relational-expression '<' shift-expression
//      relational-expression '>' shift-expression
//      relational-expression '<=' shift-expression
//      relational-expression '>=' shift-expression
//      relational-expression '<=>' shift-expression
Expr&
Parser::relational_expression()
{
  Expr* e1 = &shift_expression();
  while (true) {
    if (Token tok = match_if(lt_tok)) {
      Expr& e2 = shift_expression();
      e1 = &on_lt_expression(tok, *e1, e2);
    } else if (Token tok = match_if(gt_tok)) {
      Expr& e2 = shift_expression();
      e1 = &on_gt_expression(tok, *e1, e2);
    } else if (Token tok = match_if(lt_eq_tok)) {
      Expr& e2 = shift_expression();
      e1 = &on_le_expression(tok, *e1, e2);
    } else if (Token tok = match_if(gt_eq_tok)) {
      Expr& e2 = shift_expression();
      e1 = &on_ge_expression(tok, *e1, e2);
    } else if (Token tok = match_if(lt_eq_gt_tok)) {
      Expr& e2 = shift_expression();
      e1 = &on_cmp_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a shift expression.
//
//    shift-expression:
//      additive_expression:
//      shift-expression '<<' additive_expression
//      shift-expression '>>' additive_expression
//
Expr&
Parser::shift_expression()
{
  Expr* e1 = &additive_expression();
  while (true) {
    if (Token tok = match_if(lt_lt_tok)) {
      Expr& e2 = additive_expression();
      e1 = &on_lsh_expression(tok, *e1, e2);
    } else if (Token tok = match_if(gt_gt_tok)) {
      Expr& e2 = additive_expression();
      e1 = &on_rsh_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse an additive expression.
//
//    additive-expression:
//      multiplicative-expression:
//      additive-expression '+' multiplicative-expression
//      additive-expression '-' multiplicative-expression
Expr&
Parser::additive_expression()
{
  Expr* e1 = &multiplicative_expression();
  while (true) {
    if (Token tok = match_if(plus_tok)) {
      Expr& e2 = multiplicative_expression();
      e1 = &on_add_expression(tok, *e1, e2);
    } else if (Token tok = match_if(minus_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_sub_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a multiplicative expression.
//
//    multiplicative-expression:
//      unary-expression:
//      multiplicative-expression '*' unary-expression
//      multiplicative-expression '/' unary-expression
//      multiplicative-expression '%' unary-expression
Expr&
Parser::multiplicative_expression()
{
  Expr* e1 = &unary_expression();
  while (true) {
    // Use a switch?
    if (Token tok = match_if(star_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_mul_expression(tok, *e1, e2);
    } else if (Token tok = match_if(slash_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_div_expression(tok, *e1, e2);
    } else if (Token tok = match_if(percent_tok)) {
      Expr& e2 = unary_expression();
      e1 = &on_rem_expression(tok, *e1, e2);
    } else {
      break;
    }
  }
  return *e1;
}


// Parse a unary expression.
//
//    unary-expression:
//      postfix-expression
//      '!' unary-expression
//      '-' unary-expression
//      '+' unary-expression
//      '~' unary-expression
//
Expr&
Parser::unary_expression()
{
  if (Token tok = match_if(bang_tok)) {
    Expr& e = unary_expression();
    return on_logical_not_expression(tok, e);
  } else if (Token tok = match_if(minus_tok)) {
    Expr& e = unary_expression();
    return on_neg_expression(tok, e);
  } else if (Token tok = match_if(plus_tok)) {
    Expr& e = unary_expression();
    return on_pos_expression(tok, e);
  } else if (Token tok = match_if(caret_tok)) {
    Expr& e = unary_expression();
    return on_compl_expression(tok, e);
  } else {
    return postfix_expression();
  }
}


// Parse a postfix-expression.
//
//    postfix-expression:
//      primary-expression
//      postfix-expression '.' id
//      postfix-expression '(' [expression-list] ')'
//      postfix-expression '[' expression ']'
//
// TODO: Add lots of stuff here.
Expr&
Parser::postfix_expression()
{
  Expr* e = &primary_expression();
  while (true) {
    if (lookahead() == dot_tok)
      e = &dot_expression(*e);
    else if (lookahead() == lparen_tok)
      e = &call_expression(*e);
    else if (lookahead() == lbracket_tok)
      e = &subscript_expression(*e);
    else
      break;
  }
  return *e;
}


// Parse a call expression. This is a subroutine of the postfix
// expression parser.
//
//    postfix-expression:
//      postfix-expression '(' [expression-list] ')'
Expr&
Parser::call_expression(Expr& e)
{
  Expr_list es;
  require(lparen_tok);
  if (lookahead() != rparen_tok)
    es = expression_list();
  match(rparen_tok);
  return on_call_expression(e, es);
}


Expr&
Parser::dot_expression(Expr& e)
{
  require(dot_tok);
  Name& n = id();
  return on_dot_expression(e, n);
}


Expr&
Parser::subscript_expression(Expr& e)
{
  require(lbracket_tok);
  // Expr& index = expression();
  match(rbracket_tok);
  lingo_unreachable();
}


// Parse a comma-separated list of expressions.
//
//    expression-list;
//      expression
//      expression-list ',' expression
Expr_list
Parser::expression_list()
{
  Expr_list es;
  do {
    Expr& e = expression();
    es.push_back(e);
  } while (match_if(comma_tok));
  return es;
}


// Parse a primary expression.
//
//    primary-expression:
//      literal
//      id-expression
//      requires-expression
//      lambda-expression
//      '(' expression ')'
//      { Expr_list }
Expr&
Parser::primary_expression()
{
  switch (lookahead()) {
    case true_tok:
      return on_boolean_literal(accept(), true);
    case false_tok:
      return on_boolean_literal(accept(), false);
    case integer_tok:
      return on_integer_literal(accept());
    case identifier_tok:
      return id_expression();
    case requires_tok:
      return requires_expression();
    default:
      break;
  }

  if (lookahead() == lparen_tok)
    return grouped_expression();
    
  if(lookahead() == lbrace_tok)
    return tuple_expression();

  error(tokens.location(), "expected primary-expression");
  throw Syntax_error("primary");
}


// Parse an id-expression.
//
//    id-expression:
//      variable-name
//      function-name
//      overloaded-name
//      concept-check
//
// A concept-check is a sequence of template arguments applied
// to a concept-name.
Expr&
Parser::id_expression()
{
  Name& n = id();
  return on_id_expression(n);
}


// Parse a requires expression.
//
//    requires-expression:
//      'requires' ['<' template-parameter-list '>'] ['(' parameter-list ')'] requires-body
//
Expr&
Parser::requires_expression()
{
  Token tok = require(requires_tok);

  // Match template parameter.
  //
  // TODO: Introduce a new scope.
  Decl_list tparms;
  if (match_if(lt_tok)) {
    tparms = template_parameter_list();
    match(gt_tok);
  }

  // Parse parameters in a new block scope.
  // Enter_scope scope(cxt, cxt.make_requires_scope());
  Decl_list parms;
  if (match_if(lparen_tok)) {
    parms = parameter_list();
    match(rparen_tok);
  }

  match(lbrace_tok);
  Req_list reqs = usage_seq();
  match(rbrace_tok);

  return on_requires_expression(tok, tparms, parms, reqs);

}


// Parse a paren-enclosed expression.
//
//    grouped-expr ::= '(' expr ')'
Expr&
Parser::grouped_expression()
{
  require(lparen_tok);
  Expr& e = expression();
  match(rparen_tok);
  return e;
}


Expr&
Parser::tuple_expression()
{
  Expr_list es;
  require(lbrace_tok);
  if (lookahead() != rbrace_tok)
    es = expression_list();
  match(rbrace_tok);
  return on_tuple_expression(es);
}

} // namespace banjo
