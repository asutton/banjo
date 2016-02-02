// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SUBSUMPTION_HPP
#define BANJO_SUBSUMPTION_HPP

#include "prelude.hpp"


namespace banjo
{

struct Cons;
struct Context;


bool subsumes(Context&, Cons const&, Cons const&);


} // namespace banjo


#endif
