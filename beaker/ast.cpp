// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast.hpp"


namespace beaker
{

// Returns a qualified id for the declaration.
//
// TODO: Use an allocator.
Name const&
Decl::qualified_id() const
{
  Name* n = first;
  Decl* d = cxt;
  while (d->cxt) {
    n = new Qualified_id(d, n);
    d = d->cxt;
  }
  return *n;
}


// Returns the fully qualified id for the declaration.
//
// TODO: Use an allocator.
Name const&
Decl::fully_qualified_id() const
{
  Name* n = first;
  Decl* d = cxt;
  while (d) {
    n = new Qualified_id(d, n);
    d = d->cxt;
  }
  return *n;
}


} // namespace beaker
