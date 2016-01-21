// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lookup.hpp"
#include "ast.hpp"
#include "scope.hpp"


namespace banjo
{

// For convenience
using Binding = Scope::Binding;


// Returns the set of declarations for give (unqualified) id.
//
// Lookup ends as soon as a declaration is found
// for the given name.
//
// TODO: How should we handle non-simple id's like operator-ids
// and conversion function ids.
Decl_list
unqualified_lookup(Scope& scope, Simple_id const& id)
{
  Scope* p = &scope;
  while (p) {
    // In general, a name used in any context must be declared
    // before it's use. Search this scope for such a declaration.
    if (Binding* b = p->lookup(id))
      return b->second.base();

    // Depending on current scope, we might re-direct the scope
    // to search different things.

    if (Function_scope* s = as<Function_scope>(p)) {
      Function_decl& f = s->declaration();
      (void)f;

      // TODO: If fn is defined by a qualified-id, then we should
      // should search the scope(s) named in the id before resuming
      // the search.
    }

    else if (Class_scope* cs = as<Class_scope>(p)) {
      Class_decl& c = cs->declaration();
      (void)c;

      // TODO: Include base classes of c. Lookup also depends on
      // the declarative region of c (nested classes, locals, etc).
    }


    else if (Initializer_scope* s = as<Initializer_scope>(p)) {
      Object_decl& v = s->declaration();
      (void)v;

      // TODO: If v is declared by a qualified-id, then re-direct
      // to the scope of v before working outwards.
    }

    p = p->enclosing_scope();
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
