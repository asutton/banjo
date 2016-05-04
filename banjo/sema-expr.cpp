// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-expr.hpp"
#include "ast-type.hpp"
#include "ast-decl.hpp"
#include "expression.hpp"

#include <iostream>


namespace banjo
{

Expr&
Parser::on_logical_and_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_logical_and(cxt, e1, e2);
}


Expr&
Parser::on_logical_or_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_logical_or(cxt, e1, e2);
}


Expr&
Parser::on_logical_not_expression(Token tok, Expr& e)
{
  return make_logical_not(cxt, e);
}


Expr&
Parser::on_or_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_bit_or(cxt, e1, e2);
}


Expr&
Parser::on_xor_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_bit_xor(cxt, e1, e2);
}


Expr&
Parser::on_and_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_bit_and(cxt, e1, e2);
}


Expr&
Parser::on_lsh_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_bit_lsh(cxt, e1, e2);
}


Expr&
Parser::on_rsh_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_bit_rsh(cxt, e1, e2);
}


Expr&
Parser::on_compl_expression(Token tok, Expr& e)
{
  return make_bit_not(cxt, e);
}


Expr&
Parser::on_eq_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_eq(cxt, e1, e2);
}


Expr&
Parser::on_ne_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_ne(cxt, e1, e2);
}


Expr&
Parser::on_lt_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_lt(cxt, e1, e2);
}


Expr&
Parser::on_gt_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_gt(cxt, e1, e2);
}


Expr&
Parser::on_le_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_le(cxt, e1, e2);
}


Expr&
Parser::on_ge_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_ge(cxt, e1, e2);
}


Expr&
Parser::on_cmp_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_ge(cxt, e1, e2);
}


Expr&
Parser::on_add_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_add(cxt, e1, e2);
}


Expr&
Parser::on_sub_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_sub(cxt, e1, e2);
}


Expr&
Parser::on_mul_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_mul(cxt, e1, e2);
}


Expr&
Parser::on_div_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_div(cxt, e1, e2);
}


Expr&
Parser::on_rem_expression(Token tok, Expr& e1, Expr& e2)
{
  return make_rem(cxt, e1, e2);
}


Expr&
Parser::on_neg_expression(Token tok, Expr& e)
{
  return make_neg(cxt, e);
}


Expr&
Parser::on_pos_expression(Token tok, Expr& e)
{
  return make_pos(cxt, e);
}


// TODO: This is going to be non-trivial.
Expr&
Parser::on_call_expression(Expr& e, Expr_list& es)
{
  return make_call(cxt, e, es);
}


// TODO: This is going to be non-trivial.
Expr&
Parser::on_tuple_expression(Expr_list& es)
{
  return make_tuple_expr(cxt, es);
}


// Returns type declaration associated with the type of the expression.
// If e does not have user-defined type, the program is ill-formed.
Expr&
Parser::on_dot_expression(Expr& e, Name& n)
{
  return make_member_reference(cxt, e, n);
}


Expr&
Parser::on_id_expression(Name& n)
{
  return make_reference(cxt, n);
}


Expr&
Parser::on_boolean_literal(Token, bool b)
{
  return build.get_bool(b);
}


Expr&
Parser::on_integer_literal(Token tok)
{
  Type& t = build.get_int_type();
  Integer n = tok.spelling();
  return build.get_integer(t, n);
}


Expr&
Parser::on_requires_expression(Token tok, Decl_list& tps, Decl_list& ps, Req_list& rs)
{
  return make_requirements(cxt, tps, ps, rs);
}


Expr&
Parser::on_unparsed_expression(Token_seq&& toks)
{
  // FIXME: Use a factory method.
  return *new Unparsed_expr(std::move(toks));
}


} // namespace banjo
