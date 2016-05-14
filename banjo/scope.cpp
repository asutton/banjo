// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "scope.hpp"
#include "ast.hpp"
#include "printer.hpp"

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

std::ostream& 
operator<<(std::ostream& os, Scope const& scope)
{
  if (scope.context() && scope.context()->is_named())
    os << "==== scope for " << scope.context()->name() << " ====\n";
  else 
    os << "==== unnamed scope ====\n";

  for (auto const& bind : scope.names) {
    Name const* name = bind.first;
    std::cout << *name << '\n';
  }

  return os;
}


} // namespace banjo
