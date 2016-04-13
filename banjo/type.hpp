// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_TYPE_HPP
#define BANJO_TYPE_HPP

// Tools for working with types.

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{


Type& make_type(Context&, Name&);
Type& make_fresh_type(Context&);


} // namespace banjo


#endif
