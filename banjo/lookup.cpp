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


// FIXME: The names accepted by qualified lookup must be "atomic". That is,
// they can be neither qualified nor template-ids.


// Returns the non-empty set of declarations for give (unqualified) id.
// Throws an exception if no matching declarations are found.
//
// Lookup ends as soon as a declaration is found for the given name.
//
// TODO: How should we handle non-simple id's like operator-ids
// and conversion function ids.
Decl_list
unqualified_lookup(Context& cxt, Name const& name)
{
  Scope* p = &cxt.current_scope();
  while (p) {
    // In general, a name used in any context must be declared
    // before it's use. Search this scope for such a declaration.
    if (Overload_set* ovl = p->lookup(name))
      return *ovl;

    // TODO: The "advanced" search rules depend on the declaration
    // associated with the current scope. For example, unqualified
    // lookup within a class searches base classes.

    p = p->enclosing_scope();
  }

  error(cxt, "no matching declaration for '{}'", name);
  throw Lookup_error("no matching declaration");
}


// Simple lookup is a form of unqualified lookup that returns the
// single declaration associated with the name.
Decl&
simple_lookup(Context& cxt, Name const& name)
{
  Decl_list result = unqualified_lookup(cxt, name);

  // TODO: Can we find names that are similar to name in order to support 
  // better diagnostics? As in "did you mean...?".
  if (result.empty()) {
    error(cxt, "no matching declaration for '{}'", name);
    throw Lookup_error("no matching declaration");
  }

  // TODO: List candidates.
  if (result.size() > 1) {
    error(cxt, "lookup of '{}' is ambiguous", name);
    throw Lookup_error("ambiguous lookup");
  }

  return result.front();
}


// -------------------------------------------------------------------------- //
// Qualified lookup

// Just search in the local scope.
Decl_list
qualified_lookup(Context& cxt, Scope& scope, Name const& name)
{
  if (Overload_set* ovl = scope.lookup(name))
    return *ovl;
  else
    return {};  
}


// Perform qualified lookup. This searches the scope of the user-defined 
// type t and its base classes for the declared name n. If lookup fails, 
// the program is ill-formed.
Decl_list
qualified_lookup(Context& cxt, Type& type, Name const& name)
{
  // TODO: This probably needs to strip of all reference qualifiers,
  // not just &.
  Type& t1 = type.non_reference_type();
  
  if (!is<Declared_type>(t1)) {
    error("'{}' is not a user-defined type");
    throw Lookup_error("wrong type");
  }
  Decl& decl = cast<Declared_type>(t1).declaration();
  
  // Start by searching this scope.
  Decl_list decls = qualified_lookup(cxt, cxt.saved_scope(decl), name);

  // TODO: Search (all) bases for a member with the given name.
  // Note that multiple members can be found in multiple base classes.
  // That would constitute an ambiguous lookup.

  return decls;
}


/*
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
