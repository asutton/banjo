// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast_expr.hpp"
#include "ast_type.hpp"
#include "ast_decl.hpp"
#include "expression.hpp"
#include "print.hpp"

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
  Type& t = build.get_bool_type();
  return build.make_lt(t, e1, e2);
}


Expr&
Parser::on_gt_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_gt(t, e1, e2);
}


Expr&
Parser::on_le_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_le(t, e1, e2);
}


Expr&
Parser::on_ge_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_ge(t, e1, e2);
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
  lingo_unimplemented();
}


// TODO: This is going to be non-trivial.
Expr&
Parser::on_call_expression(Expr& e, Expr_list& es)
{
  if (Reference_expr* ref = as<Reference_expr>(&e)) {
    Decl& d = ref->declaration();
    Type& t = declared_type(d);
    if (Function_type* f = as<Function_type>(&t))
      return build.make_call(f->return_type(), e, es);

    // FIXME: Handle lambda expressions. Handle objects of class
    // type with overloads of '()'.

    throw Translation_error("'{}' is not callable", e);
  }

  // FIXME: Handle overload sets.

  lingo_unimplemented();
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
