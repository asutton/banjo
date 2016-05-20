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
  return cxt.make_add(t, e1, e2);
}


Expr&
make_sub(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_sub(t, e1, e2);
}


Expr&
make_mul(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_mul(t, e1, e2);
}


Expr&
make_div(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_div(t, e1, e2);
}


Expr&
make_rem(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_rem(t, e1, e2);
}


Expr&
make_neg(Context& cxt, Expr& e)
{
  Type& t = cxt.get_int_type();
  return cxt.make_neg(t, e);
}


Expr&
make_pos(Context& cxt, Expr& e)
{
  return cxt.make_pos(e.type(), e);
}



} // namespace banjo
