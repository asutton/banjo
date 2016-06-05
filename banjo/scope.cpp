// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "scope.hpp"
#include "ast.hpp"

#include <iostream>


namespace banjo
{

// Register a name binding for the declaration `d`.
Scope::Binding&
Scope::bind(Decl& d)
{
  return bind(d.name(), d);
}


// Streaming

#if 0
std::ostream& 
operator<<(std::ostream& os, Scope const& scope)
{
  // TODO: Print the name of the associated declaration, if there
  // is one.
  os << "==== scope ====\n";

  // TODO: Print the actual declarations?
  for (auto const& bind : scope.names) {
    Name const* name = bind.first;
    os << *name << '\n';
  }

  return os;
}
#endif


} // namespace banjo
