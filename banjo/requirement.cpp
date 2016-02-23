// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "requirement.hpp"
#include "ast_expr.hpp"
#include "ast_req.hpp"
#include "context.hpp"
#include "type.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{


// Create a basic requirement for the expression e. Synthesize
// a fresh type for its result.
Req&
make_basic_requirement(Context& cxt, Expr& e)
{
  Type& t = make_fresh_type(cxt);
  return cxt.make_basic_requirement(e, t);
}


// Construct a basic requirement for the expression e having
// type t. Adjust e so its assumed type is t.
Req&
make_basic_requirement(Context& cxt, Expr& e, Type& t)
{
  e.ty = &t;
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
