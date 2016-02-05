// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "prelude.hpp"
#include "equivalence.hpp"

#include <boost/functional/hash.hpp>



namespace banjo
{

std::size_t hash_value(Name const&);
std::size_t hash_value(Type const&);
std::size_t hash_value(Expr const&);
std::size_t hash_value(Decl const&);
std::size_t hash_value(Cons const&);


template<typename T>
struct Term_hash
{
  bool operator()(T const* t) const
  {
    return hash_value(*t);
  }
};


using Name_hash = Term_hash<Name>;
using Type_hash = Term_hash<Type>;
using Expr_hash = Term_hash<Expr>;
using Cons_hash = Term_hash<Cons>;


} // namespace banjo
