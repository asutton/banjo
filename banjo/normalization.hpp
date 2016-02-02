// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_NORMALIZATION_HPP
#define BANJO_NORMALIZATION_HPP

#include "prelude.hpp"


namespace banjo
{

struct Expr;
struct Cons;
struct Context;


Cons& normalize(Context&, Expr&);


} // namespace banjo


#endif
