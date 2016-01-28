// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_EQUIVALENCE_HPP
#define BANJO_EQUIVALENCE_HPP

#include "prelude.hpp"

#include <vector>
#include <unordered_map>


namespace banjo
{

struct Name;
struct Type;


bool is_equivalent(Name const&, Name const&);
bool is_equivalent(Type const&, Type const&);


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


} // namespace banjo

#endif
