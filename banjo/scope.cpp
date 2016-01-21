// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "scope.hpp"
#include "ast.hpp"


namespace banjo
{

// For convenience.
using Binding = Scope::Binding;


// Construct a scope enclosed by that of its surrounding
// declaration.
Scope::Scope(Decl& cxt, Decl& d)
  : parent(cxt.scope()), decl(&d)
{ }


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


Function_decl const&
Function_scope::declaration() const
{
  return *cast<Function_decl>(context());
}


Function_decl&
Function_scope::declaration()
{
  return *cast<Function_decl>(context());
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


Object_decl const&
Initializer_scope::declaration() const
{
  return *cast<Object_decl>(context());
}


Object_decl&
Initializer_scope::declaration()
{
  return *cast<Object_decl>(context());
}

} // namespace banjo
