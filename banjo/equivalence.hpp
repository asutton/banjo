// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_EQUIVALENCE_HPP
#define BANJO_EQUIVALENCE_HPP

// This module defines the common equivalence relation on terms of the
// language. In general, two terms are equivalent when they have the
// same syntactic structure, or are identifiers that refer to the same
// declarations.

#include "ast.hpp"

#include <algorithm>


namespace banjo
{

bool is_equivalent(Term const&, Term const&);
bool is_equivalent(Name const&, Name const&);
bool is_equivalent(Expr const&, Expr const&);
bool is_equivalent(Decl const&, Decl const&);
bool is_equivalent(Cons const&, Cons const&);

bool is_equivalent(Type, Type);
bool is_equivalent(Basic_type const&, Basic_type const&);
bool is_equivalent(Type_list, Type_list);


// Compare two lists of terms (but not Types)
template<typename T>
inline bool
is_equivalent(List<T> const& a, List<T> const& b)
{
  auto cmp = [](T const& x, T const& y) {
    return is_equivalent(x, y);
  };
  return std::equal(a.begin(), a.end(), b.begin(), b.end(), cmp);
}


// Returns true when type t1 differs from type t2.
inline bool
is_different(Type t1, Type t2)
{
  return !is_equivalent(t1, t2);
}


// Equality comparison for pointers.
template<typename T>
struct Term_eq
{
  bool operator()(T const* a, T const* b) const
  {
    return is_equivalent(*a, *b);
  }
};


using Name_eq = Term_eq<Name>;
using Type_eq = Term_eq<Type>;
using Expr_eq = Term_eq<Expr>;
using Decl_eq = Term_eq<Decl>;
using Cons_eq = Term_eq<Cons>;


} // namespace banjo

#endif
