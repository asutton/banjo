// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>

namespace banjo
{


// Parse an expression.
Expr&
Parser::expression()
{
  return postfix_expression();
}



Expr&
Parser::postfix_expression()
{
  return primary_expression();
}


// Parse a primary expression.
//
//    primary-expression:
//      literal
//      '(' expression ')'
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

    case lparen_tok:
      // FIXME: Add fold expressions.
      return grouped_expression();

    default:
      break;
  }

  if (lookahead() == lparen_tok)
    return grouped_expression();

  error(tokens.location(), "expected primary-expression");
  throw Syntax_error("primary");
}


// Parse an id-expression.
//
//    id-expression:
//      id
Expr&
Parser::id_expression()
{
  Name& n = id();
  return on_id_expression(n);
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


#if 0
// Parse a unary epxression. A unary expressions is one
// that begins with an operator and is followed by a
// unary expression.
//
//    unary-expression ::=
//        primary-expression
//      | unary-operator unary-expression.
Expr const*
Parser::unary_expression()
{
  if (Token tok = match_if(plus_tok))
    return on_unary(tok, unary());
  if (Token tok = match_if(minus_tok))
    return on_unary(tok, unary());
  return primary();
}


// Parse a multiplicative expression.
//
//    multiplicative-expression:
//      unary-expression
//      multiplicative-expression multiplicative-operator unary-expression
Expr const*
Parser::multiplicative_expression()
{
  Expr const* e = unary();
  while (true) {
    if (Token tok = match_if(star_tok))
      e = on_binary(tok, e, unary());
    else if (Token tok = match_if(slash_tok))
      e = on_binary(tok, e, unary());
    else if (Token tok = match_if(percent_tok))
      e = on_binary(tok, e, unary());
    else
      break;
  }
  return e;
}


// Parse an additive expression.
//
//    additive-expression ::=
//        multiplicative-expression
//      | additive-expression additive-operator multiplicative-expression
Expr const*
Parser::additive_expression()
{
  Expr const* e = multiplicative();
  while (true) {
    if (Token tok = match_if(plus_tok))
      e = on_binary(tok, e, multiplicative());
    else if (Token tok = match_if(minus_tok))
      e = on_binary(tok, e, multiplicative());
    else
      break;
  }
  return e;
}


// Parse a binary expression. This is the top-level entry point
// for the binary precedence parser.
inline Expr const*
Parser::binary_expression()
{
  return additive();
}
#endif


} // namespace banjo
