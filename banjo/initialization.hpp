// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_INITIALIZATION_HPP
#define BANJO_INITIALIZATION_HPP

#include "prelude.hpp"
#include "context.hpp"

namespace banjo
{

struct Type;
struct Reference_type;
struct Expr;
struct Init;
struct Brace_init;


Expr& zero_initialize(Context&, Type&);
Expr& default_initialize(Context&, Type&);
Expr& value_initialize(Context&, Type&);
Expr& reference_initialize(Context&, Reference_type&, Init&);
Expr& aggregate_initialize(Context&, Type&, Brace_init&);

Expr& initialize(Context&, Type&, Init&);
Expr& copy_initialize(Context&, Type&, Expr&);
Expr& direct_initialize(Context&, Type&, Init&);


} // namespace banjo


#endif
