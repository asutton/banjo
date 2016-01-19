// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_OVERLOAD_HPP
#define BANJO_OVERLOAD_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "ast.hpp"


namespace banjo
{

struct Name;


// Represents a set of overloaded declarations. All declarations have 
// the same name, scope, and kind, but may differ in their different 
// types and constraints.
//
// Note that an overload set is never empty.
//
// TODO: Do we really need this? Or can we just use a declaration list.
struct Overload_set : Decl_list
{
  // Returns the name of the overloaded declaratin.
  Name const& name() const;
  Name&       name();
};


bool can_overload(Decl&, Decl&);


} // namespace banjo


#endif
