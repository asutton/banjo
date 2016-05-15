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


// A scope defines a maximal lexical region of text where an entity may be 
// referred to without qualification. Scope objects are associated with the
// term defining their enclosing region of text.
//
// TODO: Do all scopes have an associated term? Probably.
struct Scope
{
  using Binding = Name_map::value_type;

  // Construct a new scope with the given parent. This is
  // used to create scopes that are not affiliated with a
  // declaration.
  Scope(Scope& p)
    : parent(&p), cxt(nullptr)
  { }

  // Construct a scope having the given parent and affiliated with
  // the declaration.
  Scope(Scope& p, Term& t)
    : parent(&p), cxt(&t)
  { }

  virtual ~Scope() { }

  // Returns the enclosing scope, if any. Only the global namespace does 
  // not have an enclosing scope.
  Scope const* enclosing_scope() const { return parent; }
  Scope*       enclosing_scope()       { return parent; }

  // Returns the syntactic construct that defines the scope.
  Term const& context() const { return *cxt; }
  Term&       context()       { return *cxt; }

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
  Term*    cxt;
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


// Debugging
std::ostream& operator<<(std::ostream&, Scope const&);


} // namespace banjo


#endif
