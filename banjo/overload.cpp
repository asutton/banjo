// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "overload.hpp"
#include "ast.hpp"


namespace banjo
{


Name const&
Overload_set::name() const
{
  return front()->name();
}


Name&
Overload_set::name()
{
  return front()->name();
}


// Returns true if a new declaration `d1` can be declared as
// an overload of an existing declartion `d2`.
//
// TODO: Implement me.
bool
can_overload(Decl& d1, Decl& d2)
{
  return false;
}


} // namespace banjo
