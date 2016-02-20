// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SCOPE_HPP
#define BANJO_SCOPE_HPP

#include "prelude.hpp"
#include "language.hpp"
#include "hash.hpp"
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



// Represents a namespace scope.
struct Namespace_scope : Scope
{
  using Scope::Scope;

  // Returns the namespace declaration associated with the scope.
  Namespace_decl const& declaration() const;
  Namespace_decl&       declaration();
};


// Represents function scope. Only labels have function scope.
//
// Note that the corresponding declaration may be a template.
struct Function_scope : Scope
{
  using Scope::Scope;

  // Returns the function declaration associated with the scope.
  Decl const& declaration() const { return *context(); }
  Decl&       declaration()       { return *context(); }
};


// Represents the scope of function parameter names.
//
// In C++ this is called function prototype scope, and it applies
// only to the names of parameters in function declarations (not
// definitions). For simplicity, we say that function parameter
// scope applies to all function parameter names.
struct Function_parameter_scope : Scope
{
  Function_parameter_scope(Scope& s)
    : Scope(s)
  { }
};


// Represents the scope of template parameter names.
struct Template_parameter_scope : Scope
{
  Template_parameter_scope(Scope& s)
    : Scope(s)
  { }
};


// Represents a class scope.
struct Class_scope : Scope
{
  // Returns the function declaration associated with the scope.
  Class_decl const& declaration() const;
  Class_decl&       declaration();
};


// Represents the scope of a variables initializer. This is primarily
// used to support lookup of identifiers when the variable is declared
// with a qualified-id (i.e., a static member of a class or a namespace
// variable defined outside of the namespace.
//
// The corresponding declaration is a variable or variable template.
struct Initializer_scope : Scope
{
  Initializer_scope(Scope& s, Decl& d)
    : Scope(s, d)
  { }

  // Returns the function declaration associated with the scope.
  Decl const& declaration() const { return *context(); }
  Decl&       declaration()       { return *context(); }
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
