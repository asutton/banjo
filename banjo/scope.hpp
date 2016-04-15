// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SCOPE_HPP
#define BANJO_SCOPE_HPP

#include "prelude.hpp"
#include "language.hpp"
#include "overload.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Scope definitions


// Maps names to overload sets.
using Name_map = std::unordered_map<Name const*, Overload_set, Name_hash, Name_eq>;


// A scope defines a maximal lexical region of text where an
// entity may be referred to without qualification. A scope can
// be (but is not always) associated with a declaration.
//
// The scope class also defines a region of text where a dependent
// expression may occur.
struct Scope
{
  using Binding = Name_map::value_type;

  // Construct a new scope with the given parent. This is
  // used to create scopes that are not affiliated with a
  // declaration.
  Scope(Scope& p)
    : parent(&p), decl(nullptr)
  { }

  // Construct a scope for the given declaration, but with
  // no enclosing scope. 
  Scope(Decl& d)
    : parent(nullptr), decl(&d)
  { }

  // Construct a scope having the given parent and affiliated with
  // the declaration.
  Scope(Scope& p, Decl& d)
    : parent(&p), decl(&d)
  { }

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

  // Return the binding for the given symbol, or nullptr
  // if no such binding exists.
  Overload_set const* lookup(Name const& n) const;
  Overload_set*       lookup(Name const& n);

  // Returns 1 if the name is bound and 0 otherwise.
  std::size_t count(Name const& n) const { return names.count(&n); }

  Scope*   parent;
  Decl*    decl;
  Name_map names;
};


// Bind n to `d` in this scope.
//
// Note that the addition of declarations to an overload set
// must be handled by semantic rules.
inline Scope::Binding&
Scope::bind(Name const& n, Decl& d)
{
  lingo_assert(count(n) == 0);
  auto ins = names.insert({&n, {d}});
  return *ins.first;
}


// Returns the binding for n, if any.
inline Overload_set const*
Scope::lookup(Name const& n) const
{
  auto iter = names.find(&n);
  if (iter != names.end())
    return &iter->second;
  else
    return nullptr;
}


inline Overload_set*
Scope::lookup(Name const& n)
{
  auto iter = names.find(&n);
  if (iter != names.end())
    return &iter->second;
  else
    return nullptr;
}


} // namespace banjo


#endif
