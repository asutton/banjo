// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_INITIALIZATION_HPP
#define BANJO_INITIALIZATION_HPP

// This module defines the core routines that determine the initialization
// semantics for an object (variable, constant, or parameter).
//
// TODO: The bifurcation of objects and values is annoying.

#include "ast-base.hpp"

namespace banjo
{

Expr& zero_initialize(Context&, Decl&);
Expr& default_initialize(Context&, Decl&);
Expr& value_initialize(Context&, Decl&);
Expr& copy_initialize(Context&, Decl&, Expr&);
Expr& direct_initialize(Context&, Decl&, Expr&);
Expr& direct_initialize(Context&, Decl&, Expr_list&);
Expr& list_initialize(Context&, Decl&, Expr_list&);
Expr& reference_initialize(Context&, Decl&, Expr&);
Expr& aggregate_initialize(Context&, Decl&, Expr_list&);
Expr& array_initialize(Decl&, Expr&);
Expr& tuple_initialize(Context&, Decl&, Tuple_expr&);

Expr& tuple_array_init(Type&, Expr&);
Expr& array_tuple_init(Type&, Expr&);


} // namespace banjo


#endif
