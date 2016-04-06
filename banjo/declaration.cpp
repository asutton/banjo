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
// Declaration of un-typed entities
//
// The remember() function is used to store first-class entities prior
// to the elaboration of their type. Note that we can't distinguish (or
// make equivalent) declarations without knowing their types. This also
// means that we may save multiple declarations for partial definitions
// (e.g., namespaces).
//
// TODO: I'm not sure that I like this design. Think about it...

void
remember(Context&, Scope& scope, Decl& decl)
{
  if (Overload_set* ovl = scope.lookup(decl.declared_name()))
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

// A given function or variable declaration is a redeclaration if
// there exists a previous function or variable with the same
// name, scope, and declared type.
static inline Decl*
redeclare_object(Decl& prev, Decl& given)
{
  Type& t1 = declared_type(prev);
  Type& t2 = declared_type(given);
  if (is_equivalent(t1, t2))
    return &prev;
  else
    return nullptr;
}


// Determine if a given declaration is a redeclaration of a
// previously declared entity. Return the previous entity,
// or nullptr if not redeclared.
//
// Note that we are guaranteed that decl and prev have the same
// scope (because we're trying to declare given in this scope)
// and name (because prev is a previous declaraiton in an overload
// set).
Decl*
redeclare(Decl& prev, Decl& given)
{
  struct fn
  {
    Decl& prev;
    Decl* operator()(Decl& given) { banjo_unhandled_case(given); }

    // An object or function is a redeclaration if it has
    // the same declared type.
    Decl* operator()(Variable_decl& given) { return redeclare_object(prev, given); }
    Decl* operator()(Function_decl& given) { return redeclare_object(prev, given); }

    // User-defined types with the same names must be redeclarations.
    Decl* operator()(Class_decl& given)    { return &prev; }
    Decl* operator()(Union_decl& given)    { return &prev; }
    Decl* operator()(Enum_decl& given)     { return &prev; }
  };
  return apply(given, fn{prev});
}


// Determine if a given declaration is a redeclaration of a
// previously declared entity. Return the previous entity,
// or nullptr if not redeclared.
Decl*
redeclare(Overload_set& ovl, Decl& given)
{
  // If the declarations have different kinds, then this
  // is clearly not a redeclaraiton.
  Decl& rep = ovl.front();
  if (typeid(rep) != typeid(given))
    return nullptr;

  // Every declaration in ovl has the same kind as given.
  // We need to search.
  for (Decl& prev : ovl) {
    if (Decl* orig = redeclare(prev, given))
      return orig;
  }
  return nullptr;
}


// Save `d` as a new declaration in the given overload set, if
// possible. Two declarations with the same name and type declare
// the same entity and are therefore not overloads.
void
declare(Overload_set& ovl, Decl& d)
{
  // Handle redeclarations. Note that we may can't check for
  // redefinition at this point because the point of declaration
  // always preceeds the definition. That's done elsewhere.
  //
  // TODO: Chain d to the the previous declaration.
  if (redeclare(ovl, d))
    return;

  // If it's not a redeclaration, try to overload.
  declare_overload(ovl, d);
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

  // Nothing can be declared in constrained scope or template scope.
  if (is<Constrained_scope>(&scope))
    return false;
  if (is<Template_scope>(&scope))
    return false;

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
void
declare(Context&, Scope& scope, Decl& decl)
{
  // Find an appropriate declartive region for the declaration.
  Scope& s = adjust_scope(scope, decl);

  // Try to declare entity.
  if (Overload_set* ovl = s.lookup(decl.declared_name()))
    declare(*ovl, decl);
  else
    s.bind(decl);
}


// Try to declare d in the current scope.
void
declare(Context& cxt, Decl& d)
{
  declare(cxt, cxt.current_scope(), d);
}


// -------------------------------------------------------------------------- //
// Declaration of required expressionns

// Save the declaration of a required expression.
//
// FIXME: This is a hack. This should be name-based. Otherwise, lookup
// is going to be very, very slow.
//
// FIXME: Who is responsible for guaranteeing non-repetition?
void
declare_required_expression(Context& cxt, Expr& e)
{
  Requires_scope& s = *cxt.current_requires_scope();
  s.exprs.push_back(e);
}

} // namespace banjo
