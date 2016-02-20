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
Decl*
declare(Overload_set& ovl, Decl& d)
{
  declare_overload(ovl, d);

  // if (can_categorically_overload(ovl, d)) {
  //   auto ins = ovl.insert(d);
  //   if (!ins.second)
  //     explain_overload_error(ovl, d);
  // } else {
  //   error("declaration of '{}' conflicts with previous declaration(s)", d.name());
  //   throw Translation_error("overload conflict");
  // }

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
// FIXME: Handle re-declarations gracefully. Note that nearly every kind
// of declaration can be re-declared. For example:
//
//    struct S;
//    struct S; // OK: redeclaration
//
//    def f(int) -> int;
//    def f(int) -> int; // OK: redeclaraiton
//
//    extern int n;
//    extern int n; // OK: redeclaration
//
// This gets a bit interesting with overloading. We don't want to
// put each declaration of a function in an overload set -- we just
// want a single declaration.
//
// FIXME: Handle redefinition errors:
//
//    struct S { }
//    struct S { } // error: redefinition
//
//
// FIXME: If `d`'s name is a qualified-id, then we need to adjust
// the context to that specified by `d`s nested name specifier.
Decl*
declare(Context&, Scope& scope, Decl& decl)
{
  // Find an appropriate declartive region for the declaration.
  Scope& s = adjust_scope(scope, decl);

  // Try to declare entity.
  if (Overload_set* ovl = s.lookup(decl.declared_name())) {
    return declare(*ovl, decl);
  } else {
    s.bind(decl);
    return nullptr;
  }
}


} // namespace banjo
