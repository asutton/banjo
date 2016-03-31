// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_HASH_HPP
#define BANJO_HASH_HPP

#include "prelude.hpp"
#include "language.hpp"
#include "equivalence.hpp"

#include <boost/functional/hash.hpp>


namespace banjo
{

std::size_t hash_value(Term const&);
std::size_t hash_value(Name const&);
std::size_t hash_value(Type const&);
std::size_t hash_value(Expr const&);
std::size_t hash_value(Decl const&);
std::size_t hash_value(Cons const&);


template<typename T>
inline std::size_t
hash_value(List<T> const& list)
{
  std::size_t h = 0;
  for (T const& t : list)
    boost::hash_combine(h, t);
  return h;
}


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


#endif
