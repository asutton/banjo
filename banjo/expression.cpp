// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "equivalence.hpp"
#include "initialization.hpp"
#include "conversion.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{


Expr&
make_logical_and(Context& cxt, Expr& e1, Expr& e2)
{
  Builder build(cxt);
  Expr& c1 = contextual_conversion_to_bool(cxt, e1);
  Expr& c2 = contextual_conversion_to_bool(cxt, e2);
  Type& t = c1.type();
  return build.make_and(t, c1, c2);
}


Expr&
make_logical_or(Context& cxt, Expr& e1, Expr& e2)
{
  Builder build(cxt);
  Expr& c1 = contextual_conversion_to_bool(cxt, e1);
  Expr& c2 = contextual_conversion_to_bool(cxt, e2);
  Type& t = c1.type();
  return build.make_or(t, c1, c2);
}


Expr&
make_logical_not(Context& cxt, Expr& e)
{
  Builder build(cxt);
  Expr& c = contextual_conversion_to_bool(cxt, e);
  Type& t = c.type();
  return build.make_not(t, c);
}


} // namespace banjo
