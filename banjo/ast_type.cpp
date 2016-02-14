// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast_type.hpp"
#include "ast_decl.hpp"


namespace banjo
{

Name const&
User_defined_type::name() const
{
  return declaration().name();
}


Name&
User_defined_type::name()
{
  return declaration().name();
}


Class_decl const&
Class_type::declaration() const
{
  return *cast<Class_decl>(decl);
}


Class_decl&
Class_type::declaration()
{
  return *cast<Class_decl>(decl);
}


Union_decl const&
Union_type::declaration() const
{
  return *cast<Union_decl>(decl);
}


Union_decl&
Union_type::declaration()
{
  return *cast<Union_decl>(decl);
}


Enum_decl const&
Enum_type::declaration() const
{
  return *cast<Enum_decl>(decl);
}


Enum_decl&
Enum_type::declaration()
{
  return *cast<Enum_decl>(decl);
}


Type_parm const&
Typename_type::declaration() const
{
  return *cast<Type_parm>(decl);
}


Type_parm&
Typename_type::declaration()
{
  return *cast<Type_parm>(decl);
}


} // namepace banjo
