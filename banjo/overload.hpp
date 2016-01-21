// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_OVERLOAD_HPP
#define BANJO_OVERLOAD_HPP

#include "prelude.hpp"


namespace banjo
{

struct Name;
struct Decl;


// Represents a set of overloaded declarations. All declarations have 
// the same name, scope, and kind, but may differ in their different 
// types and constraints.
//
// Note that an overload set is never empty.
struct Overload_set : std::vector<Decl*>
{
  using std::vector<Decl*>::vector;

  // Returns the underlying list of declarations.
  std::vector<Decl*> const& base() const { return *this; }
  std::vector<Decl*>&       base()       { return *this; }

  // Returns the name of the overloaded declaratin.
  Name const& name() const;
  Name&       name();
};


bool can_overload(Decl&, Decl&);


} // namespace banjo


#endif
