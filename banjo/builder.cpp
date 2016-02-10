// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"
#include "equivalence.hpp"
#include "hash.hpp"

#include <unordered_set>


namespace banjo
{

// FIXME: Move this into lingo.
//
// A unique factory will only allocate new objects if they have not been
// previously created.
template<typename T, typename Hash, typename Eq>
struct Hashed_unique_factory : std::unordered_set<T, Hash, Eq>
{
  template<typename... Args>
  T& make(Args&&... args)
  {
    auto ins = this->emplace(std::forward<Args>(args)...);
    return *const_cast<T*>(&*ins.first); // Yuck.
  }
};


template<typename T>
struct Hash
{
  std::size_t operator()(T const& t) const
  {
    return hash_value(t);
  }

  std::size_t operator()(List<T> const& t) const
  {
    return hash_value(t);
  }
};


template<typename T>
struct Eq
{
  bool operator()(T const& a, T const& b) const
  {
    return is_equivalent(a, b);
  }

  bool operator()(List<T> const& a, List<T> const& b) const
  {
    return is_equivalent(a, b);
  }
};


template<typename T>
using Factory = Hashed_unique_factory<T, Hash<T>, Eq<T>>;


// -------------------------------------------------------------------------- //
// Constraints

// FIXME: Save all uniqued terms in the context, not as global variables.

Concept_cons&
Builder::get_concept_constraint(Decl& d, Term_list& ts)
{
  static Factory<Concept_cons> f;
  return f.make(d, ts);
}


Predicate_cons&
Builder::get_predicate_constraint(Expr& e)
{
  static Factory<Predicate_cons> f;
  return f.make(e);
}


Conjunction_cons&
Builder::get_conjunction_constraint(Cons& c1, Cons& c2)
{
  static Factory<Conjunction_cons> f;
  return f.make(c1, c2);
}


Disjunction_cons&
Builder::get_disjunction_constraint(Cons& c1, Cons& c2)
{
  static Factory<Disjunction_cons> f;
  return f.make(c1, c2);
}


}
