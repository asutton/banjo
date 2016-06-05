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
bool is_equivalent(Type const&, Type const&);
bool is_equivalent(Expr const&, Expr const&);
bool is_equivalent(Decl const&, Decl const&);
bool is_equivalent(Cons const&, Cons const&);


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
is_different(Type const& t1, Type const& t2)
{
  return !is_equivalent(t1, t2);
}


} // namespace banjo

#endif
