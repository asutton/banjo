// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_INTRINSIC_HPP
#define BANJO_INTRINSIC_HPP

// This module defines all of the intrinsic function exposed by the 
// compiler. These represent the definitions of built-in functions.

#include "language.hpp"


namespace banjo
{

namespace intrinsic
{

Value eq_bool(Context&, Value const&, Value const&);
Value ne_bool(Context&, Value const&, Value const&);

Value eq_int(Context&, Value const&, Value const&);
Value ne_int(Context&, Value const&, Value const&);

Value show_value(Context&, Value const&);
 

} // namespace intrinsic

} // namespace banjo


#endif
