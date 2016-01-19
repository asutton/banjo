// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_DEDUCTION_HPP
#define BANJO_DEDUCTION_HPP

#include "prelude.hpp"
#include "substitution.hpp"


namespace banjo
{


void deduce(Type&, Type&, Substitution&);


} // namespace banjo


#endif
