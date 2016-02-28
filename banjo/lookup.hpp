// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_LOOKUP_HPP
#define BANJO_LOOKUP_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{


Decl&     simple_lookup(Context&, Scope&, Name const&);

Decl_list unqualified_lookup(Context&, Name const&);
Decl_list unqualified_lookup(Context&, Scope&, Name const&);

// Decl_list qualified_lookup(Scope&, Symbol const&);
// Decl_list argument_dependent_lookup(Scope&, Expr_list&);

Expr* requirement_lookup(Context& cxt, Expr&);
Decl_list required_expression_lookup(Context& cxt, Name const&);

} // namespace banjo


#endif
