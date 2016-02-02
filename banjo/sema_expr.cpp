// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

// FIXME: Actually perform semantic anlaysis.

Expr&
Parser::on_logical_and_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_and(t, e1, e2);
}


Expr&
Parser::on_logical_or_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_or(t, e1, e2);
}


Expr&
Parser::on_logical_not_expression(Token tok, Expr& e)
{
  Type& t = build.get_bool_type();
  return build.make_not(t, e);
}


Expr&
Parser::on_eq_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_eq(t, e1, e2);
}


Expr&
Parser::on_ne_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_ne(t, e1, e2);
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


// TODO: This is going to be non-trivial.
Expr&
Parser::on_call_expression(Expr& e, Expr_list& es)
{
  lingo_unimplemented();
}


// TOOD: Resolve the id as either a variable name, a function
// name, or an overload name. It can be nothing else.
Expr&
Parser::on_id_expression(Name& n)
{
  // FIXME: There are a lot of questions to ask here... Presumably,
  // I must ensure that this resoles to a legitimate check, and the
  // arguments should match in kind (and type?). What if they don't.
  //
  // Also, if the arguments are non-dependent, should I fold the
  // constant?
  //
  // If the arguments are dependent, should I enter into a constrained
  // scope?
  //
  // As mentioned... lots of interesting things to do here.
  if (Concept_id* id = as<Concept_id>(&n)) {
    return build.make_check(id->declaration(), id->arguments());
  }
  lingo_unimplemented();
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


} // namespace banjo
