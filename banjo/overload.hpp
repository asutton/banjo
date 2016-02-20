// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_OVERLOAD_HPP
#define BANJO_OVERLOAD_HPP

#include "prelude.hpp"
#include "hash.hpp"

#include <unordered_set>


namespace banjo
{

struct Name;
struct Decl;


// Compute a hash value for declarations in an overload set.
struct Overload_hash
{
  std::size_t operator()(Decl const*) const;
};


// Determine the equivalence of declarations in an overload set.
struct Overload_eq
{
  bool operator()(Decl const*, Decl const*) const;
};


using Overload_impl = std::unordered_set<Decl*, Overload_hash, Overload_eq>;


struct Overload_iterator
{
  using Iter = Overload_impl::iterator;
  using value_type        = Decl;
  using reference         = Decl&;
  using pointer           = Decl*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  Overload_iterator(Iter i)
    : iter(i)
  { }

  reference operator*() const { return modify(**iter); }
  pointer  operator->() const { return modify(*iter); }

  Overload_iterator& operator++()    { ++iter; return *this; }
  Overload_iterator  operator++(int) { Overload_iterator x = *this; ++iter; return x; }

  bool operator==(Overload_iterator i) const { return iter == i.iter; }
  bool operator!=(Overload_iterator i) const { return iter != i.iter; }

  Iter iter;
};


struct Overload_const_iterator
{
  using Iter = Overload_impl::const_iterator;
  using value_type        = Decl;
  using reference         = Decl const&;
  using pointer           = Decl const*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  Overload_const_iterator(Iter i)
    : iter(i)
  { }

  Overload_const_iterator(Overload_iterator i)
    : iter(i.iter)
  { }

  reference operator*() const { return **iter; }
  pointer  operator->() const { return *iter; }

  Overload_const_iterator& operator++()    { ++iter; return *this; }
  Overload_const_iterator  operator++(int) { Overload_const_iterator x = *this; ++iter; return x; }

  bool operator==(Overload_const_iterator i) const { return iter == i.iter; }
  bool operator!=(Overload_const_iterator i) const { return iter != i.iter; }

  Iter iter;
};


// Represents a set of overloaded declarations. All declarations have
// the same name, scope, and kind, but may differ in their different
// types and constraints.
//
// Note that an overload set is never empty.
struct Overload_set
{
  using iterator       = Overload_iterator;
  using const_iterator = Overload_const_iterator;

  Overload_set(Decl&);

  // Returns the name of the overloaded declaratin.
  Name const& name() const;
  Name&       name();

  std::pair<iterator, bool> insert(Decl&);

  // Returns the first element in the overload set.
  Decl const& front() const { return *begin(); }
  Decl&       front()       { return *begin(); }

  // Iterators
  iterator begin() { return set.begin(); }
  iterator end()   { return set.end(); }

  const_iterator begin() const { return set.begin(); }
  const_iterator end() const   { return set.end(); }

  Overload_impl set;
};


// Initialize the overload set with a single element.
inline
Overload_set::Overload_set(Decl& d)
  : set {&d}
{ }


bool can_categorically_overload(Overload_set&, Decl&);
bool can_categorically_overload(Decl&, Decl&);


} // namespace banjo


#endif
