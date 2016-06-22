// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "debugging.hpp"

#include <iostream>


namespace banjo
{


Expr&
make_add(Context& cxt, Expr& e1, Expr& e2)
{
  // auto& ret = make_operator(cxt, add_op, e1, e2, &Context::make_add);
  // debug(ret);
  // return ret;
  return make_operator(cxt, add_op, e1, e2, &Context::make_add);
}


Expr&
make_sub(Context& cxt, Expr& e1, Expr& e2)
{
  return make_operator(cxt, sub_op, e1, e2, &Context::make_sub);
}


Expr&
make_mul(Context& cxt, Expr& e1, Expr& e2)
{
  return make_operator(cxt, mul_op, e1, e2, &Context::make_mul);
}


Expr&
make_div(Context& cxt, Expr& e1, Expr& e2)
{
  return make_operator(cxt, div_op, e1, e2, &Context::make_div);
}


Expr&
make_rem(Context& cxt, Expr& e1, Expr& e2)
{
  return make_operator(cxt, rem_op, e1, e2, &Context::make_rem);
}


Expr&
make_neg(Context& cxt, Expr& e)
{
  return make_operator(cxt, sub_op, e, &Context::make_neg);
}


// FIXME: This needs to a indirect-to-immediate value conversion.
Expr&
make_pos(Context& cxt, Expr& e)
{
  return make_operator(cxt, add_op, e, &Context::make_pos);
}



} // namespace banjo
