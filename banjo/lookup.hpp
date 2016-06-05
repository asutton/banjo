// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_LOOKUP_HPP
#define BANJO_LOOKUP_HPP

// This module defines the basic set of lookup rules for finding names
// in a scope.
//
// TODO: These rules should really be defined by the front-end.

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{


Decl&     simple_lookup(Context&, Name const&);
Decl_list unqualified_lookup(Context&, Name const&);
Decl_list qualified_lookup(Context&, Type&, Name const&);

// Decl_list argument_dependent_lookup(Scope&, Expr_list&);

Expr* requirement_lookup(Context& cxt, Expr&);

} // namespace banjo


#endif
