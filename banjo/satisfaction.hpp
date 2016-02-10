// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SATISFACTION_HPP
#define BANJO_SATISFACTION_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "substitution.hpp"

namespace banjo
{

bool is_satisfied(Context&, Cons&);
bool is_satisfied(Context&, Expr&);


} // namespace banjo


#endif
