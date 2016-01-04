// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast.hpp"


namespace beaker
{

// Compute the fully qualified name for the declaration.
//
// Note that the fully qualified id for the global namespace is
// just the global namespace name.
Name const*
Decl::qualified_id() const
{
  Name* n = first;
  Decl* d = cxt;
  while (d) {
    n = new Qualified_id(d, n);
    d = d->cxt;
  }
  return n;
}


} // namespace beaker
