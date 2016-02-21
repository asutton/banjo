// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_NORMALIZATION_HPP
#define BANJO_NORMALIZATION_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

Cons& normalize(Context&, Expr&);
Cons& normalize(Context&, Req&);


} // namespace banjo


#endif
