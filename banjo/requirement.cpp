// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "requirement.hpp"
#include "ast_expr.hpp"
#include "ast_req.hpp"
#include "context.hpp"
#include "type.hpp"
#include "lookup.hpp"
#include "declaration.hpp"
#include "print.hpp"

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

  e.ty = &t;

  // Save the declaration of this binding.
  declare_required_expression(cxt, e);

  return cxt.make_basic_requirement(e, t);
}


// Build a requirement that e must be convertible to t. Note that
// the type of `e` is unspecified.
//
// TODO: Currently, e has placeholder type. We should perform a lookup
// to ensure that e acutally has the correct type, so that we can unify
// expessions and types.
Req&
make_conversion_requirement(Context& cxt, Expr& e, Type& t)
{
  return cxt.make_conversion_requirement(e, t);
}


Req&
make_deduction_requirement(Context& cxt, Expr& e, Type& t)
{
  lingo_unimplemented();
}


Req&
make_syntactic_requirement(Context& cxt, Expr& e)
{
  Builder build(cxt);
  return build.make_syntactic_requirement(e);
}


} // namespace banjo
