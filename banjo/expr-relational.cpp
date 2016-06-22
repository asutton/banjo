// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast-type.hpp"
#include "ast-expr.hpp"
#include "type.hpp"
#include "constraint.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "conversion.hpp"
#include "debugging.hpp"

#include <iostream>


namespace banjo
{


// FXIME: Is there no way to simplify this? Yes. See expr-arithmetic.cpp.
Expr&
make_eq(Context& cxt, Expr& e1, Expr& e2)
{
  Resolution res = resolve_operator(cxt, eq_op, e1, e2);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  Eq_expr& e = cxt.make_eq(fn.return_type(), args[0], args[1]);
  e.res_ = &fn;
  return e;
}


Expr&
make_ne(Context& cxt, Expr& e1, Expr& e2)
{
  Resolution res = resolve_operator(cxt, ne_op, e1, e2);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  Ne_expr& e = cxt.make_ne(fn.return_type(), args[0], args[1]);
  e.res_ = &fn;
  return e;
}


Expr&
make_lt(Context& cxt, Expr& e1, Expr& e2)
{
  Resolution res = resolve_operator(cxt, lt_op, e1, e2);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  Lt_expr& e = cxt.make_lt(fn.return_type(), args[0], args[1]);
  e.res_ = &fn;
  return e;
}


Expr&
make_gt(Context& cxt, Expr& e1, Expr& e2)
{
  Resolution res = resolve_operator(cxt, gt_op, e1, e2);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  Gt_expr& e = cxt.make_gt(fn.return_type(), args[0], args[1]);
  e.res_ = &fn;
  return e;
}


Expr&
make_le(Context& cxt, Expr& e1, Expr& e2)
{
  Resolution res = resolve_operator(cxt, le_op, e1, e2);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  Le_expr& e = cxt.make_le(fn.return_type(), args[0], args[1]);
  e.res_ = &fn;
  return e;
}


Expr&
make_ge(Context& cxt, Expr& e1, Expr& e2)
{
  Resolution res = resolve_operator(cxt, ge_op, e1, e2);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  Ge_expr& e = cxt.make_ge(fn.return_type(), args[0], args[1]);
  e.res_ = &fn;
  return e;
}


} // namespace banjo
