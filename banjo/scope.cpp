// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "scope.hpp"
#include "ast.hpp"


namespace banjo
{

// For convenience.
using Name_binding = Scope::Name_binding;


// Construct a scope enclosed by that of its surrounding
// declaration.
Scope::Scope(Decl& cxt, Decl& d)
  : parent(cxt.scope()), decl(&d)
{ }


// Register a name binding for the declaration `d`.
Name_binding&
Scope::bind(Decl& d)
{
  return bind(d.declared_name(), d);
}


Namespace_decl const&
Namespace_scope::declaration() const
{
  return *cast<Namespace_decl>(context());
}


Namespace_decl&
Namespace_scope::declaration()
{
  return *cast<Namespace_decl>(context());
}


Class_decl const&
Class_scope::declaration() const
{
  return *cast<Class_decl>(context());
}


Class_decl&
Class_scope::declaration()
{
  return *cast<Class_decl>(context());
}


} // namespace banjo
