// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "prelude.hpp"
#include "equivalence.hpp"

#include <boost/functional/hash.hpp>


namespace beaker
{

struct Type;


std::size_t hash_value(Type const&);


template<typename T>
struct Term_hash
{
  bool operator()(T const* t) const
  {
    return hash_value(*t);
  }
};


using Type_hash = Term_hash<Type>;


} // namespace beaker
