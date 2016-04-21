// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_OVERLOAD_HPP
#define BANJO_OVERLOAD_HPP

#include "language.hpp"

#include <unordered_set>


namespace banjo
{

// Represents a set of overloaded declarations. All declarations have
// the same name, scope, and kind, but may differ in their different
// types and constraints.
//
// Note that an overload set is never empty.
struct Overload_set : Decl_list
{
  using iterator       = Decl_list::iterator;
  using const_iterator = Decl_list::const_iterator;

  // Initialize the overload set with a single element.
  Overload_set(Decl& d)
    : Decl_list {&d}
  { }

  // Returns the name of the overloaded declaratin.
  Name const& name() const;
  Name&       name();

  // Inserts a new declaration into the overload set. The declaration
  // shall be overloadable with all previous elements of the set.
  void insert(Decl& d) { push_back(d); }
};


bool can_overload(Decl&, Decl&);


std::ostream& operator<<(std::ostream&, Overload_set const&);


} // namespace banjo


#endif
