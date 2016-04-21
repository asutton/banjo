// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lookup.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "scope.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{


// Returns the non-empty set of declarations for give (unqualified) id.
// Throws an exception if no matching declarations are found.
//
// Lookup ends as soon as a declaration is found for the given name.
//
// TODO: How should we handle non-simple id's like operator-ids
// and conversion function ids.
Decl_list
unqualified_lookup(Context& cxt, Scope& scope, Simple_id const& id)
{
  Scope* p = &scope;
  while (p) {
    // In general, a name used in any context must be declared
    // before it's use. Search this scope for such a declaration.
    if (Overload_set* ovl = p->lookup(id))
      return *ovl;

    // TODO: The "advanced" search rules depend on the declaration
    // associated with the current scope. For example, unqualified
    // lookup within a class searches base classes.

    p = p->enclosing_scope();
  }

  throw Lookup_error(cxt, "no matching declaration for '{}'", id);
}


// Simple lookup is a form of unqualified lookup that returns the
// single declaration associated with the name.
Decl&
simple_lookup(Context& cxt, Scope& scope, Simple_id const& id)
{
  Decl_list result = unqualified_lookup(cxt, scope, id);

  // FIXME: Can we find names that are *like* id?
  if (result.empty())
    throw Lookup_error(cxt, "no matching declaration for '{}'", id);

  // FIXME: Find some way of attaching informative diagnotics
  // to the error (i.e., candidates).
  if (result.size() > 1)
    throw Lookup_error(cxt, "lookup of '{}' is ambiguous", id);

  return result.front();
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


// Lookup the expression in the current requirement scope. This
// returns the expressions whose operands have equivalent type or
// nullptr if they no such expression has been declared.
//
// FIXME: This is a hack. Lookup should use an operator-id and
// perform a lookup in a table, and NOT searching through a
// list of requirements.
Expr*
requirement_lookup(Context& cxt, Expr& e)
{
  #if 0
  Requires_scope& s = *cxt.current_requires_scope();

  Type_list t1 = get_operand_types(e); // Yuck.
  for (Expr& e2 : s.exprs) {
    // Expressions of different kinds are not comparable.
    if (typeid(e) != typeid(e2))
      continue;

    // Compare the types of operands.
    Type_list t2 = get_operand_types(e2);
    if (is_equivalent(t1, t2)) {
      return &e;
    }
  }
  #endif
  return nullptr;
}



} // namespace banjo
