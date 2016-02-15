// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "declaration.hpp"
#include "ast_decl.hpp"
#include "scope.hpp"
#include "lookup.hpp"
#include "overload.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// Save `d` as a new declaration in the given overload set.
//
// TODO: Implement me.
//
// TODO: Actually check for overloading and re-definition
// errors.
Decl*
declare(Overload_set& ovl, Decl& d)
{
  lingo_unimplemented();
  return nullptr;
}


// Returns true if we can declare a kind of decl in scope.
//
// FIXME: This, and the function below are somewhat gross.
// The problem is that I've caused each declarative region
// of a declaration to encapsulate the others. This means,
// for example, that:
//
//    template<typename T> // Starts template scope
//    struct S             // In template scope at point of declaration
//    {                    // Begins class scope.
//    }                    // Ends class and then template scope.
//
// Note that S cannot be declared in template scope, or it
// would not be visible in its current declaration context.
//
// TODO: Be sure to keep this function up to date. Is there a
// better way to define this? Maybe a virtual function on the
// scope? In fact, this might be a very good idea.
bool
can_declare_in(Scope& scope, Decl& decl)
{
  if (is<Function_parameter_scope>(&scope)) {
    if (is<Object_parm>(&decl))
      return true;
    return false;
  }

  if (is<Template_parameter_scope>(&scope)) {
    if (is<Type_parm>(&decl))
      return true;
    if (is<Value_parm>(&decl))
      return true;
    if (is<Template_parm>(&decl))
      return true;
    return false;
  }

  return true;
}


// See the comments above.
Scope&
adjust_scope(Scope& scope, Decl& decl)
{
  Scope* s = &scope;
  while (s) {
    if (can_declare_in(*s, decl))
      return *s;
    s = s->enclosing_scope();
  }
  lingo_unreachable();
}


// Try to declare a name binding in the current scope.
//
// FIXME: The specific rules probably depend on a) the kind of scope,
// and b) the kind of declaration.
//
// FIXME: Check for re-declaration and overloading. This probably
// requires that we match on the entity declared.
//
// FIXME: If `d`'s name is a qualified-id, then we need to adjust
// the context to that specified by `d`s nested name specifier.
Decl*
declare(Context&, Scope& scope, Decl& decl)
{
  // Find an appropriate declartive region for the declaration.
  Scope& s = adjust_scope(scope, decl);

  // Declare the ajusted declaration.
  if (Overload_set* ovl = s.lookup(decl.declared_name())) {
    return banjo::declare(*ovl, decl);
  } else {
    s.bind(decl);
    return nullptr;
  }
}


} // namespace banjo
