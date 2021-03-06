// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-decl.hpp"
#include "ast-name.hpp"
#include "ast-type.hpp"
#include "ast-def.hpp"


namespace banjo
{


Function_type const&
Mapping_decl::type() const
{
  return cast<Function_type>(*type_);
}


Function_type&
Mapping_decl::type()
{
  return cast<Function_type>(*type_);
}


Type const&
Mapping_decl::return_type() const
{
  return type().return_type();
}


Type&
Mapping_decl::return_type()
{
  return type().return_type();
}


// Class_decl

Decl_list const& 
Class_decl::objects() const 
{ 
  return cast<Class_def>(def_)->objects();
}

Decl_list& 
Class_decl::objects() 
{ 
  return cast<Class_def>(def_)->objects();
}


// -------------------------------------------------------------------------- //
// Declared type

Type&
declared_type(Decl& d)
{
  if (Typed_decl* td = as<Typed_decl>(&d))
    return td->type();
  lingo_unreachable();
}



} // namespace banjo
