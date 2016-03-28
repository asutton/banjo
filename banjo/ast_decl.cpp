// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast_decl.hpp"
#include "ast_name.hpp"
#include "ast_type.hpp"
#include "ast_def.hpp"
#include "scope.hpp"


namespace banjo
{

Name const&
Decl::declared_name() const
{
  return id->unqualified_name();
}


Name&
Decl::declared_name()
{
  return id->unqualified_name();
}


Function_type const&
Function_decl::type() const
{
  return *cast<Function_type>(ty);
}


Function_type&
Function_decl::type()
{
  return *cast<Function_type>(ty);
}


Type const&
Function_decl::return_type() const
{
  return type().return_type();
}


Type&
Function_decl::return_type()
{
  return type().return_type();
}


Class_def const&
Class_decl::definition() const
{
  return *cast<Class_def>(def);
}


Class_def&
Class_decl::definition()
{
  return *cast<Class_def>(def);
}


Union_def const&
Union_decl::definition() const
{
  return *cast<Union_def>(def);
}


Union_def&
Union_decl::definition()
{
  return *cast<Union_def>(def);
}


Enum_def const&
Enum_decl::definition() const
{
  return *cast<Enum_def>(def);
}


Enum_def&
Enum_decl::definition()
{
  return *cast<Enum_def>(def);
}


// Create a namespace with its own scope.
Namespace_decl::Namespace_decl(Name& n)
  : Decl(n), decls(), lookup(new Namespace_scope(*this))
{ }


// TODO: Manage the memory for the scope?
Namespace_decl::Namespace_decl(Decl& cxt, Name& n)
  : Decl(cxt, n)
  , decls()
  , lookup(new Namespace_scope(cxt, *this))
{ }


bool
Namespace_decl::is_anonymous() const
{
  return is<Placeholder_id>(id);
}


// -------------------------------------------------------------------------- //
// Declared type

// Return the type of a declaration. A type describes objects and
// references.
//
// FIXME: This is kind of dumb. We should have a base class that
// contributes a type to the declaration hiearchy (Typed_decl).
Type&
declared_type(Decl& decl)
{
  struct fn
  {
    Type& operator()(Decl& d)          { banjo_unhandled_case(d); }
    Type& operator()(Object_decl& d)   { return d.type(); }
    Type& operator()(Function_decl& d) { return d.type(); }
  };
  Decl& d = decl.parameterized_declaration();
  return apply(d, fn{});
}


Type const&
declared_type(Decl const& d)
{
  return declared_type(const_cast<Decl&>(d));
}

} // namespace banjo
