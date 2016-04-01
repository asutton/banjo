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
Parser::on_ior_expression(Token tok, Expr& e1, Expr& e2)
{
  banjo_unimplemented("inclusive or expression");
}


Expr&
Parser::on_xor_expression(Token tok, Expr& e1, Expr& e2)
{
  banjo_unimplemented("exclusive or expression");
}


Expr&
Parser::on_and_expression(Token tok, Expr& e1, Expr& e2)
{
  banjo_unimplemented("and expression");
}


Expr&
Parser::on_lsh_expression(Token tok, Expr& e1, Expr& e2)
{
  banjo_unimplemented("left-shift expression");
}


Expr&
Parser::on_rsh_expression(Token tok, Expr& e1, Expr& e2)
{
  banjo_unimplemented("right-shift expression");
}


Expr&
Parser::on_compl_expression(Token tok, Expr& e)
{
  banjo_unimplemented("one's complement expression");
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
  lingo_unimplemented();
}


Expr&
Parser::on_sub_expression(Token tok, Expr& e1, Expr& e2)
{
  lingo_unimplemented();
}


Expr&
Parser::on_mul_expression(Token tok, Expr& e1, Expr& e2)
{
  lingo_unimplemented();
}


Expr&
Parser::on_div_expression(Token tok, Expr& e1, Expr& e2)
{
  lingo_unimplemented();
}


Expr&
Parser::on_rem_expression(Token tok, Expr& e1, Expr& e2)
{
  banjo_unimplemented("remainder expression");
}


Expr&
Parser::on_neg_expression(Token tok, Expr& e)
{
  banjo_unimplemented("negation expression");
}


Expr&
Parser::on_pos_expression(Token tok, Expr& e)
{
  banjo_unimplemented("identity expression");
}


// TODO: This is going to be non-trivial.
Expr&
Parser::on_call_expression(Expr& e, Expr_list& es)
{
  return make_call(cxt, e, es);
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


} // namespace banjo
