// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EQUIVALENCE_HPP
#define BEAKER_EQUIVALENCE_HPP

#include "prelude.hpp"

#include <vector>
#include <unordered_map>


namespace beaker
{

struct Type;


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


using Type_eq = Term_eq<Type>;





// -------------------------------------------------------------------------- //
// Equivalence closure

template<typename T>
struct Equivalence_closure
{

};

} // namespace beaker

#endif
