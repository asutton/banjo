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
make_bit_and(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_bit_and(val_expr, t, e1, e2);
}


Expr&
make_bit_or(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_bit_or(val_expr, t, e1, e2);
}


Expr&
make_bit_xor(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_bit_xor(val_expr, t, e1, e2);
}


Expr&
make_bit_lsh(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_bit_lsh(val_expr, t, e1, e2);
}


Expr&
make_bit_rsh(Context& cxt, Expr& e1, Expr& e2)
{
  Type& t = cxt.get_int_type();
  return cxt.make_bit_rsh(val_expr, t, e1, e2);
}


Expr&
make_bit_not(Context& cxt, Expr& e)
{
  Type& t = cxt.get_int_type();
  return cxt.make_bit_not(val_expr, t, e);
}




} // namespace banjo
