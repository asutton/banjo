// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_INITIALIZATION_HPP
#define BANJO_INITIALIZATION_HPP

#include "ast-base.hpp"

namespace banjo
{

Expr& zero_initialize(Context&, Type&);
Expr& default_initialize(Context&, Type&);
Expr& value_initialize(Context&, Type&);
Expr& copy_initialize(Context&, Type&, Expr&);
Expr& direct_initialize(Context&, Type&, Expr&);
Expr& direct_initialize(Context&, Type&, Expr_list&);
Expr& list_initialize(Context&, Type&, Expr_list&);
Expr& reference_initialize(Context&, Reference_type&, Expr&);
Expr& aggregate_initialize(Context&, Type&, Expr_list&);
Expr& array_initialize(Type&, Expr&);
Expr& tuple_initialize(Type&, Expr&);
Expr& tuple_array_init(Type&, Expr&);
Expr& array_tuple_init(Type&, Expr&);


} // namespace banjo


#endif
