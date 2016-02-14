// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast_decl.hpp"
#include "ast_name.hpp"
#include "ast_type.hpp"
#include "ast_def.hpp"


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


bool
Namespace_decl::is_anonymous() const
{
  return is<Placeholder_id>(id);
}


} // namespace banjo
