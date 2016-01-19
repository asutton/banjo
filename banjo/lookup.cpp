// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lookup.hpp"
#include "ast.hpp"


namespace banjo
{


// Returns the set of declarations for give (unqualified) id.
Decl_list
unqualified_lookup(Scope& scope, Simple_id const& id)
{
  Scope* p = &scope;
  while (p) {
    // Overload_set* ovl = scope->look
    // p = p->parent(); 
  }
  return {};
}


/*
Decl_list
qualified_lookup(Scope&, Symbol const&)
{
  return nullptr;
}


Decl_list
argument_dependent_lookup(Scope&, Expr_list&)
{
  return nullptr;
}
*/


} // namespace banjo
