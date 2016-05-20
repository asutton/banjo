// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_REQUIREMENT_HPP
#define BANJO_CORE_REQUIREMENT_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

struct Context;

Req& make_basic_requirement(Context&, Expr&);
Req& make_basic_requirement(Context&, Expr&, Type&);
Req& make_conversion_requirement(Context&, Expr&, Type&);
Req& make_deduction_requirement(Context&, Expr&, Type&);
Req& make_syntactic_requirement(Context&, Expr&);

} // namespace banjo


#endif
