// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast-type.hpp"
#include "ast-expr.hpp"
#include "context.hpp"
#include "type.hpp"
#include "constraint.hpp"
#include "lookup.hpp"
#include "conversion.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// FIXME: These are just stubbed out for now. Actually convert arguments
// as needed.

Expr&
make_add(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_add(val_expr, t, e1, e2);
}


Expr&
make_sub(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_sub(val_expr, t, e1, e2);
}


Expr&
make_mul(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_mul(val_expr, t, e1, e2);
}


Expr&
make_div(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_div(val_expr, t, e1, e2);
}


Expr&
make_rem(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_rem(val_expr, t, e1, e2);
}


Expr&
make_neg(Context& cxt, Expr& e)
{
  Type& t = cxt.get_int_type();
  return cxt.make_neg(val_expr, t, e);
}


// FIXME: This needs to a indirect-to-immediate value conversion.
Expr&
make_pos(Context& cxt, Expr& e)
{
  return cxt.make_pos(val_expr, e.type(), e);
}



} // namespace banjo
