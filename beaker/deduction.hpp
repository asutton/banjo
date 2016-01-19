// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_DEDUCTION_HPP
#define BEAKER_DEDUCTION_HPP

#include "prelude.hpp"
#include "substitution.hpp"


namespace beaker
{


void deduce(Type&, Type&, Substitution&);


} // namespace beaker


#endif
