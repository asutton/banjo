// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SCOPE_HPP
#define BANJO_SCOPE_HPP

#include "prelude.hpp"
#include "overload.hpp"
#include "context.hpp"

#include <lingo/environment.hpp>


namespace banjo
{

struct Decl;
struct Namespace_decl;
struct Function_decl;
struct Class_decl;
struct Object_decl;


// -------------------------------------------------------------------------- //
// Scope definitions
//
// TODO: Do I need derived kinds of scopes?


// A scope defines a maximal lexical region of text where an
// entity  may be referred to without qualification. A scope can
// be (but is not always) associated with a declaration.
struct Scope : Environment<Name const*, Overload_set>
{
  using Base = Environment<banjo::Name const*, Overload_set>;

  // Construct a new scope with the given parent. This is
  // used to create scopes that are not affiliated with a
  // declaration.
  Scope(Scope& p)
    : parent(&p), decl(nullptr)
  { }

  // Construct a scope for the given declaration, but with
  // no enclosing scope. This is primarily used to create the
  // global namespace.
  Scope(Decl& d)
    : parent(nullptr), decl(&d)
  { }

  // Construt a scope having the given parent and affiliated with
  // the declaration.
  Scope(Scope& p, Decl& d)
    : parent(&p), decl(&d)
  { }

  Scope(Decl&, Decl&);

  virtual ~Scope() { }

  // Returns the enclosing scope, if any. Only the global
  // namespace does not have an enclosing scope.
  Scope const* enclosing_scope() const { return parent; }
  Scope*       enclosing_scope()       { return parent; }

  // Returns the declaration associated with the scope.
  // Not all scopes are associated with a declaration
  // (e.g., block scope).
  Decl const* context() const { return decl; }
  Decl*       context()       { return decl; }

  // Create a name binding for the given declaration. Behavior
  // is undefined if a name binding already exists.
  //
  // TODO: Assert that `n` is a form of simple id.
  Binding& bind(Decl& d);
  Binding& bind(Name const&, Decl&);

  int count(Name const& n) const { return Base::count(&n); }

  // Return the binding for the given symbol, or nullptr
  // if no such binding exists.
  Binding const* lookup(Name const& n) const { return Base::lookup(&n); }
  Binding*       lookup(Name const& n)       { return Base::lookup(&n); }

  Scope* parent;
  Decl*  decl;
};


// Represents a namespace scope.
struct Namespace_scope : Scope
{
  using Scope::Scope;

  // Returns the namespace declaration associated with the scope.
  Namespace_decl const& declaration() const;
  Namespace_decl&       declaration();
};



// Represents function scope.
struct Function_scope : Scope
{
  // Returns the function declaration associated with the scope.
  Function_decl const& declaration() const;
  Function_decl&       declaration();
};


// Represents a class scope.
struct Class_scope : Scope
{
  // Returns the function declaration associated with the scope.
  Class_decl const& declaration() const;
  Class_decl&       declaration();
};


// Represents the scope of a variables initializer. This is
// primarily used to support lookup of identifiers when the
// variable is declared with a qualified-id (i.e., a static
// member of a class or a namespace variable defined outside
// of the namespace.
struct Initializer_scope : Scope
{
  // Returns the function declaration associated with the scope.
  Object_decl const& declaration() const;
  Object_decl&       declaration();
};


// Represents a block scope.
struct Block_scope : Scope
{
};


// TODO: Define other kinds of scope.


// Returns true if s is a scope for a namespace.
inline bool
is_namespace_scope(Scope const& s)
{
  return is<Namespace_scope>(&s);
}


// Returns true if s is a scope for a function definition.
inline bool
is_function_scope(Scope const& s)
{
  return is<Function_scope>(&s);
}


} // namespace banjo


#endif
