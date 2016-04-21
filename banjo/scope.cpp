// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "scope.hpp"
#include "ast.hpp"


namespace banjo
{

// For convenience.
using Binding = Scope::Binding;


// Construct a scope enclosed by that of its surrounding declaration.
Scope::Scope(Decl& cxt, Decl& d)
  : parent(cxt.scope()), decl(&d)
{ }


// Register a name binding for the declaration `d`.
Binding&
Scope::bind(Decl& d)
{
  return bind(d.name(), d);
}


} // namespace banjo
