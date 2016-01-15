// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_INITIALIZATION_HPP
#define BEAKER_INITIALIZATION_HPP

#include "prelude.hpp"
#include "context.hpp"


namespace beaker
{

struct Type;
struct Expr;
struct Init;


Init& zero_initialize(Type&);
Init& default_initialize(Type&);
Init& value_initialize(Type&);

Init& initialize(Type&, Init&);
Init& initialize_reference(Type&, Init&);
Init& initialize_aggregate(Type&, Init&);


} // namespace beaker


#endif
