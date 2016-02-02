// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_LOOKUP_HPP
#define BANJO_LOOKUP_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "ast.hpp"

#include <lingo/environment.hpp>


namespace banjo
{


struct Simple_id;


Decl&     simple_lookup(Scope&, Simple_id const&);
Decl_list unqualified_lookup(Scope&, Simple_id const&);

// Decl_list qualified_lookup(Scope&, Symbol const&);
// Decl_list argument_dependent_lookup(Scope&, Expr_list&);


} // namespace banjo


#endif
