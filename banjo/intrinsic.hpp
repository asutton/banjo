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

// Boolean relations
Value bool_eq(Context&, Value const&, Value const&);
Value bool_ne(Context&, Value const&, Value const&);

// Boolean logic
Value bool_and(Context&, Value const&, Value const&);
Value bool_or(Context&, Value const&, Value const&);
Value bool_not(Context&, Value const&);

// Boolean conversions
Value bool_to_int(Context&, Value const&);

// Integer relations
Value int_eq(Context&, Value const&, Value const&);
Value int_ne(Context&, Value const&, Value const&);
Value int_lt(Context&, Value const&, Value const&);
Value int_gt(Context&, Value const&, Value const&);
Value int_le(Context&, Value const&, Value const&);
Value int_ge(Context&, Value const&, Value const&);
Value int_cmp(Context&, Value const&, Value const&);

// Integer arithmetic
Value int_add(Context&, Value const&, Value const&);
Value int_sub(Context&, Value const&, Value const&);
Value int_mul(Context&, Value const&, Value const&);
Value int_div(Context&, Value const&, Value const&);
Value int_rem(Context&, Value const&, Value const&);

// Integer conversions
Value int_to_bool(Context&, Value const&);


// Miscellaneous function
Value show_value(Context&, Value const&);
 

} // namespace intrinsic

} // namespace banjo


#endif
