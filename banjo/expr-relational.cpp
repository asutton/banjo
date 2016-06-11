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

#include <iostream>


namespace banjo
{


// Apply the usual arithmetic conversions for operands to relational
// expressions. The result type is bool.
template<typename Make>
static Expr&
make_standard_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Expr_pair conv = arithmetic_conversions(cxt, e1, e2);
  Type& t = cxt.get_bool_type(object_type);
  return make(t, conv.first, conv.second);
}


// If either expression has dependent type, then the type of the
// expression is a fresh type.
//
// TODO: If either expression has occurred previously, then we should
// use it's result type and not generate a fresh type.
template<typename Make>
static Expr&
make_dependent_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
#if 0
  // Build a dependent expression.
  Type& t = make_fresh_type(cxt);
  Expr& init = make(t, e1, e2);

  // Unify with previous expressions.
  if (cxt.in_requirements())
    return make_required_expression(cxt, init);

  // Don't check in unconstrained templates.
  if (cxt.in_unconstrained_template())
    return init;

  // Inside a constrained template, search the constraints to
  // determine if the expression is admissible.
  Expr& con = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, con, init))
    return *ret;
#endif

  // Search for dependent conversions.
  return make_standard_relational_expr(cxt, e1, e2, make);
}


// Search for an overload of the given operator. The type is determined
// by overload resolution.
//
// FIXME: Try overload resolution.
template<typename Make>
static Expr&
make_regular_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  return make_standard_relational_expr(cxt, e1, e2, make);
}


// Determine the result type of the relational expression.
template<typename Make>
static Expr&
make_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Type& t1 = e1.type();
  Type& t2 = e2.type();
  try {
    if (is_dependent_type(t1) || is_dependent_type(t2))
      return make_dependent_relational_expr(cxt, e1, e2, make);
    else
      return make_regular_relational_expr(cxt, e1, e2, make);
  } catch (Translation_error&) {
    // FIXME: Diagnose the error, but create an expression
    // with a poisoned type.
    throw;
  }
}


Expr&
make_eq(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_eq(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_ne(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_ne(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_lt(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_lt(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_gt(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_gt(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_le(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_le(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_ge(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_ge(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


} // namespace banjo
