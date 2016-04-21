// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "declaration.hpp"
#include "ast-type.hpp"
#include "ast-decl.hpp"
#include "context.hpp"
#include "scope.hpp"
#include "lookup.hpp"
#include "overload.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Declaration of untyped entities
//
// The remember() function is used to store first-class entities prior
// to the elaboration of their type. Note that we can't distinguish (or
// make equivalent) declarations without knowing their types. This also
// means that we may save multiple declarations for partial definitions
// (e.g., namespaces).
//
// TODO: I don't like this design.

void
remember(Context&, Scope& scope, Decl& decl)
{
  if (Overload_set* ovl = scope.lookup(decl.name()))
    ovl->push_back(decl);
  else
    scope.bind(decl);
}


// Try to declare d in the current scope.
void
remember(Context& cxt, Decl& d)
{
  remember(cxt, cxt.current_scope(), d);
}


// -------------------------------------------------------------------------- //
// Declaration of typed entities
//
// FIXME: Declaration is totally broken right now.


// Returns if the given declaration can be overloaded with each
// declaration in the overload set. See comments on the functions
// below for cases.
void
declare_overload(Overload_set& ovl, Decl& given)
{
  for (Decl& prev : ovl) {
    if (!can_overload(prev, given))
      throw Type_error("invalid declaration");
  }
  ovl.insert(given);
}


// Save d in the given overload set, if possible.
//
// FIXME: Handle unification and redefinition errors.
void
declare(Overload_set& ovl, Decl& d)
{
  declare_overload(ovl, d);
}


// Try to declare a name binding in the current scope.
void
declare(Context&, Scope& scope, Decl& decl)
{
  if (Overload_set* ovl = scope.lookup(decl.name()))
    declare(*ovl, decl);
  else
    scope.bind(decl);
}


// Try to declare d in the current scope.
void
declare(Context& cxt, Decl& d)
{
  declare(cxt, cxt.current_scope(), d);
}


// -------------------------------------------------------------------------- //
// Declaration of required expressions

// Save the declaration of a required expression.
//
// FIXME: This is a hack. This should be name-based. Otherwise, lookup
// is going to be very, very slow.
//
// FIXME: Who is responsible for guaranteeing non-repetition?
void
declare_required_expression(Context& cxt, Expr& e)
{
  // Requires_scope& s = *cxt.current_requires_scope();
  // s.exprs.push_back(e);
}

} // namespace banjo
