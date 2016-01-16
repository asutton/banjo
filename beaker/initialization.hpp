// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_INITIALIZATION_HPP
#define BEAKER_INITIALIZATION_HPP

#include "prelude.hpp"
#include "context.hpp"

namespace beaker
{

struct Type;
struct Reference_type;
struct Expr;
struct Init;
struct Brace_init;


Init& zero_initialize(Context&, Type&);
Init& default_initialize(Context&, Type&);
Init& value_initialize(Context&, Type&);
Init& reference_initialize(Context&, Reference_type&, Init&);
Init& aggregate_initialize(Context&, Type&, Brace_init&);

Init& initialize(Context&, Type&, Init&);
Init& copy_initialize(Context&, Type&, Init&);
Init& direct_initialize(Context&, Type&, Init&);


} // namespace beaker


#endif
