// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_REQUIREMENT_HPP
#define BANJO_REQUIREMENT_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

struct Context;

Req& make_simple_requirement(Context&, Expr&);
Req& make_conversion_requirement(Context&, Expr&, Type&);
Req& make_deduction_requirement(Context&, Expr&, Type&);

} // namespace banjo


#endif
