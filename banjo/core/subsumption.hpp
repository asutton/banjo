// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_SUBSUMPTION_HPP
#define BANJO_CORE_SUBSUMPTION_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

bool subsumes(Context&, Cons const&, Cons const&);
bool subsumes(Context&, Expr const&, Expr const&);


} // namespace banjo


#endif
