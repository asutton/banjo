// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "overload.hpp"
#include "ast.hpp"


namespace banjo
{

// In general, the hash value of a declaration in an overload set is
// that of its declared type.
//
// The hash value of a function declaration is that of its parameter types;
// the return type is excluded. This means that functions with equivalent
// parameter types will have equal hash values. For all other types, the
// hash value is the declared type.
//
// Note that the hash value of function templates includes the hash
// value of the return type.
//
// All type declarations have the same hash value since types cannot
// be overloaded.
std::size_t
Overload_hash::operator()(Decl const* d) const
{
  // Pick a number that might make a good hash valule.
  //
  // TODO: Add a kinding system and return the hash of
  // the kind.
  if (is<Type_decl>(d))
    return 13;

  // Otherwise, the declaration has a declared type.
  Type const& t = declared_type(*d);
  if (Function_type const* f = as<Function_type>(&t))
    return hash_value(f->parameter_types());
  return hash_value(t);
}


// In general, two declarations in an overload set are equivalent if
// they have the equivalent declared types.
//
// Two function declarations are equivalent if the have equivalent
// parameter types.
//
// Note that the comparison of function templates also incorporates
// the result type.
//
// All type declarations are considered to be equivalent since they
// cannot be overloaded.
bool
Overload_eq::operator()(Decl const* d1, Decl const* d2) const
{
  // Types declarations are equivalent.
  if (is<Type_decl>(d1))
    return is<Type_decl>(d2);
  else if (is<Type_decl>(d2))
    return false;

  Type const& t1 = declared_type(*d1);
  Type const& t2 = declared_type(*d2);
  if (Function_type const* f1 = as<Function_type>(&t1)) {
    if (Function_type const* f2 = as<Function_type>(&t2))
      return is_equivalent(*f1, *f2);
  }
  return is_equivalent(t1, t2);
}


Name const&
Overload_set::name() const
{
  return front().name();
}


Name&
Overload_set::name()
{
  return modify(front().name());
}


// Try to insert `d` into the overload set. Insertion fails if there
// is a declaration in the overload set with an equivalent type.
//
// Note that this function should only ever be called when d and all
// elements in the overload set are functions or function templates.
std::pair<Overload_set::iterator, bool>
Overload_set::insert(Decl& d)
{
  lingo_assert(is<Function_decl>(&d.parameterized_declaration()));
  lingo_assert(is<Function_decl>(&front().parameterized_declaration()));
  auto x = set.insert(&d);
  return {x.first, x.second};
}


// Returns if we can categorically overload the given declaration with
// any in the overload set. Only functions and function templates can
// be overlaoded.
//
// Note that this does not guarantee that the declaration can actually
// be overloaded.
bool
can_categorically_overload(Overload_set& ovl, Decl& d)
{
  return can_categorically_overload(ovl.front(), d);
}


// Returns true if we can categorically overload the given declaration
// with a previous declaration. Only functions and function templates can
// be overloaded.
//
// Note that this does not guarantee that the declaration can actually
// be overloaded.
bool
can_categorically_overload(Decl& prev, Decl& given)
{
  // Strip off template declarations (if any) and compare the
  // underlying declarations.
  Decl& d1 = prev.parameterized_declaration();
  Decl& d2 = given.parameterized_declaration();
  return is<Function_decl>(&d1) && is<Function_decl>(&d2);
}


} // namespace banjo
