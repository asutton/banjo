// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "requirement.hpp"
#include "ast-expr.hpp"
#include "ast-req.hpp"
#include "context.hpp"
#include "type.hpp"
#include "lookup.hpp"
#include "declaration.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{


// Create a basic requirement for the expression e. Note that e
// already has a type assigned to it: it is either a fresh type
// because e was a new expression in this context, or it was
// assigned by a prior typing.
Req&
make_basic_requirement(Context& cxt, Expr& e)
{
  return cxt.make_basic_requirement(e, e.type());
}


// Construct a basic requirement for the expression e having
// type t. Adjust e so its assumed type is t. A requirement shall
// by explicitly typed just once.
//
//      e : t1; // #1
//      e : t2; // #2
//
// When translating this, e is first parsed in #1 and assigned a fresh type.
// Its type is later adjusted to t1 by this function and that typing is
// saved to the current context. In #2, e is found by lookup and assigned
// type t1. However, this function would try to re-type the expression to t2.
// That is an error.
Req&
make_basic_requirement(Context& cxt, Expr& e, Type& t)
{
  // Check that we are't re-typing the expression.
  if (requirement_lookup(cxt, e))
    throw Type_error("expression '{}' declared to have multipe types", e);

  e.type_ = &t;

  // Save the declaration of this binding.
  declare_required_expression(cxt, e);

  return cxt.make_basic_requirement(e, t);
}


// Build a requirement that e must be convertible to t. Note that
// e has been previously typed, possibly with a fresh type variable.
//
// If e is the first occurrence of its kind (i.e., lookup fails)
// then declare e to have that type. In other words, we are transforming
// this requirement:
//
//    e -> t;
//
// to these requirements:
//
//    e : t0; // t0 is the original type of e
//    e -> t;
//
// TODO: Should we actually generate multiple requiremnts like
// this? This would affect subsumption in (likely) a positive way.
//
// TODO: I'd also like to save the conversion to optimize conversion
// lookup. We could actually save these in conversion-ids.
Req&
make_conversion_requirement(Context& cxt, Expr& e, Type& t)
{
  if (!requirement_lookup(cxt, e))
    declare_required_expression(cxt, e);
  return cxt.make_conversion_requirement(e, t);
}


Req&
make_deduction_requirement(Context& cxt, Expr& e, Type& t)
{
  lingo_unreachable();
}


Req&
make_syntactic_requirement(Context& cxt, Expr& e)
{
  Builder build(cxt);
  return build.make_syntactic_requirement(e);
}


} // namespace banjo
