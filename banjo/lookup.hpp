// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_LOOKUP_HPP
#define BANJO_LOOKUP_HPP

#include "prelude.hpp"
#include "language.hpp"
#include "hash.hpp"


namespace banjo
{


Decl&     simple_lookup(Scope&, Simple_id const&);
Decl_list unqualified_lookup(Scope&, Simple_id const&);

// Decl_list qualified_lookup(Scope&, Symbol const&);
// Decl_list argument_dependent_lookup(Scope&, Expr_list&);


// -------------------------------------------------------------------------- //
// Facts
//
// TODO: Find some way to merge this with name bindings. It would
// be kind of nice to have a single system for building associations
// between terms and their properties.
//
// I think that this would work by making scope a term hash, and
// making its mapped type something like a fact, below. Each "fact"
// about a term might be an overload set, or a type binding, or
// a set of conversions.

// A fact set is a set of known properties about a dependent expression.
// For any such expression, we can know:
//
//    - it's type
//    - it's set of conversions
//
// TODO: This isn't all. I'm sure there's much more.
struct Fact
{
  explicit operator bool() const { return ty; }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type_list const& conversions() const { return conv; }
  Type_list&       conversions()       { return conv; }

  void merge(Fact const&);

  Type*     ty;
  Type_list conv;
};


// Merge `f` into this fact. If both `f` and this have types,
// those types must be equivalent.
inline void
Fact::merge(Fact const& f)
{
  // Copy the the type.
  if (!ty) {
    ty = f.ty;
  } else {
    lingo_assert(is_equivalent(*ty, *f.ty));
  }

  // Merge sequences of conversions.
  //
  // TODO: It would be nice if we only copied conversions
  // that we didn't already know.
  for (Type const& t : f.conv)
    conv.push_back(modify(t));
}



// Evidence is a structured mapping of expressions to known facts.
//
// TODO: This isn't the best of names. I would still prefer to somehow
// unify this with the scope structure to provide a uniform method
// for recording information about terms.
struct Evidence
{
  using Fact_map = std::unordered_map<Expr const*, Fact, Expr_hash, Expr_eq>;

  Evidence(Evidence* p = nullptr)
    : map(), prev(p)
  { }

  Fact const* lookup(Expr const&) const;
  Fact*       lookup(Expr const&);

  void save_type(Expr const&, Type& t);
  void save_conversion(Expr const&, Type& t);

  Fact_map  map;
  Evidence* prev;
};


// Search the fact environment for facts about `e`. If no
// facts are known, return nullptr.
inline Fact const*
Evidence::lookup(Expr const& e) const
{
  auto iter = map.find(&e);
  if (iter == map.end())
    return nullptr;
  else
    return &iter->second;
}


inline Fact*
Evidence::lookup(Expr const& e)
{
  auto iter = map.find(&e);
  if (iter == map.end())
    return nullptr;
  else
    return &iter->second;
}


// Save the type of an expression. The expression shall not have
// been previously assigned a type.
inline void
Evidence::save_type(Expr const& e, Type& t)
{
  lingo_assert(!map.count(&e));
  auto ins = map.emplace(&e, Fact{});
  ins.first->second.ty = &t;
}


// Save a conversion of `e` to `t`. The expression shall have
// a type.
inline void
Evidence::save_conversion(Expr const& e, Type& t)
{
  lingo_assert(map.count(&e));
  auto iter = map.find(&e);
  iter->second.conv.push_back(t);
}


// -------------------------------------------------------------------------- //
// Dependent lookup

Fact dependent_lookup(Evidence&, Expr const&);

} // namespace banjo


#endif
