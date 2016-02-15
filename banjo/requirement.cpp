// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "requirement.hpp"
#include "ast_req.hpp"
#include "builder.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

Req&
make_simple_requirement(Context& cxt, Expr& e)
{
  lingo_unimplemented();
}


Req&
make_conversion_requirement(Context& cxt, Expr& e, Type& t)
{
  std::cout << "YEAH!\n";
  lingo_unimplemented();
  // FIXME: Is there actually anything we need to do here?
  // Builder build(cxt);
  // return build.make_conversion_requirement(e, t);
}


Req&
make_deduction_requirement(Context& cxt, Expr& e, Type& t)
{
  lingo_unimplemented();
}


} // namespace banjo
