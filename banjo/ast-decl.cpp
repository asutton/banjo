// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-decl.hpp"
#include "ast-name.hpp"
#include "ast-type.hpp"
#include "ast-def.hpp"
#include "scope.hpp"


namespace banjo
{

// Function_decl

Function_type const&
Function_decl::type() const
{
  return *cast<Function_type>(type_);
}


Function_type&
Function_decl::type()
{
  return *cast<Function_type>(type_);
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

// Return the type of a declaration. A type describes objects and
// references.
//
// FIXME: This is kind of dumb. We should have a base class that
// contributes a type to the declaration hierarchy (Typed_decl).
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
