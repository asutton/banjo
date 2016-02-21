// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "requirement.hpp"
#include "ast_req.hpp"
#include "builder.hpp"
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
  Builder build(cxt);
  Type& t = make_fresh_type(cxt);
  return build.make_basic_requirement(e, t);
}


// Construct a basic requirement for the expression e.
Req&
make_basic_requirement(Context& cxt, Expr& e, Type& t)
{
  Builder build(cxt);
  return build.make_basic_requirement(e, t);
}


Req&
make_conversion_requirement(Context& cxt, Expr& e, Type& t)
{
  lingo_unimplemented();
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
