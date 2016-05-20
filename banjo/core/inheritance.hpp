// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_CONVERT_HPP
#define BANJO_CORE_CONVERT_HPP

#include "prelude.hpp"
#include "context.hpp"


namespace banjo
{

struct Type;


bool is_base_class(Type const&, Type const&);

} // namespace banjo


#endif
