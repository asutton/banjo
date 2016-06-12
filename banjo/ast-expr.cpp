// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-expr.hpp"
#include "ast-type.hpp"
#include "ast-decl.hpp"


namespace banjo
{

// Decl_ref

Typed_decl const& 
Decl_ref::declaration() const
{
  return cast<Typed_decl>(*decl_);
}


Typed_decl&
Decl_ref::declaration()
{
  return cast<Typed_decl>(*decl_);
}


// Variable_ref

Variable_decl const&
Variable_ref::declaration() const
{
  return cast<Variable_decl>(*decl_);
}


Variable_decl&      
Variable_ref::declaration()
{
  return cast<Variable_decl>(*decl_);
}


// Function_ref

Function_decl const&
Function_ref::declaration() const
{
  return cast<Function_decl>(*decl_);
}


Function_decl&      
Function_ref::declaration()
{
  return cast<Function_decl>(*decl_);
}


// Scoped_ref

Typed_decl const& 
Scoped_ref::declaration() const
{
  return cast<Typed_decl>(*decl_);
}


Typed_decl&
Scoped_ref::declaration()
{
  return cast<Typed_decl>(*decl_);
}


// Field_ref

Field_decl const&
Field_ref::declaration() const
{
  return cast<Field_decl>(*decl_);
}


Field_decl&      
Field_ref::declaration()
{
  return cast<Field_decl>(*decl_);
}


// Method_ref

Method_decl const&
Method_ref::declaration() const
{
  return cast<Method_decl>(*decl_);
}


Method_decl&      
Method_ref::declaration()
{
  return cast<Method_decl>(*decl_);
}



// -------------------------------------------------------------------------- //
// Operations on expressions

// Returns true if `e` has type `bool`.
bool
has_bool_type(Expr const& e)
{
  return is_boolean_type(e.type());
}


// Returns true if the expression `e` has integer type.
bool
has_integer_type(Expr const& e)
{
  return is_integer_type(e.type());
}


// Returns true if the expression `e` has floating point type.
bool
has_floating_point_type(Expr const& e)
{
  return is_floating_point_type(e.type());
}


// Returns true if `e` has array type.
bool
has_array_type(Expr const& e)
{
  return is_array_type(e.type());
}


// Returns true if `e` has tuple type.
bool
has_tuple_type(Expr const& e)
{
  return is_tuple_type(e.type());
}


// -------------------------------------------------------------------------- //
// Dependent expressions


// Returns true if the expression is type-dependent. An expression
// is type-dependent if it has dependent type.
//
// TODO: There are probably some other interesting cases here.
bool
is_type_dependent(Expr const& e)
{
  return is_dependent_type(e.type());
}


// Returns true if any expression in the list is type dependent.
bool
is_type_dependent(Expr_list const& es)
{
  auto p = [](Expr const& e) { return is_type_dependent(e); };
  return std::any_of(es.begin(), es.end(), p);
}


// -------------------------------------------------------------------------- //
// Declared type of an expression

// Returns the declared type of an expression. In general, this is type of 
// the expression. However, for id-expressions, we actually use the declared 
// type of the referenced declaration.
Type const&
declared_type(Expr const& e)
{
  return declared_type(modify(e));
}


Type&
declared_type(Expr& e)
{
  if (Decl_ref* d = as<Decl_ref>(&e))
    return d->declaration().type();
  if (Scoped_ref* d = as<Scoped_ref>(&e))
    return d->declaration().type();

    return e.type();
}


} // namespace banjo
