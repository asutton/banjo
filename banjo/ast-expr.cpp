// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-expr.hpp"
#include "ast-type.hpp"
#include "ast-decl.hpp"


namespace banjo
{

Object_decl const&
Object_expr::declaration() const
{
  return cast<Object_decl>(*decl_);
}


Object_decl&
Object_expr::declaration()
{
  return cast<Object_decl>(*decl_);
}


Function_decl const&
Function_expr::declaration() const
{
  return cast<Function_decl>(*decl_);
}


Function_decl&
Function_expr::declaration()
{
  return cast<Function_decl>(*decl_);
}



Concept_decl const&
Check_expr::declaration() const
{
  return cast<Concept_decl>(*con);
}


Concept_decl&
Check_expr::declaration()
{
  return cast<Concept_decl>(*con);
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


// Returns true if `e` has reference type.
bool
has_reference_type(Expr const& e)
{
  return is_reference_type(e.type());
}


// Returns trhe if `e` has array type.
bool
has_array_type(Expr const& e)
{
  return is_array_type(e.type());
}


// Returns trhe if `e` has tuple type.
bool
has_tuple_type(Expr const& e)
{
  return is_tuple_type(e.type());
}

// -------------------------------------------------------------------------- //
// Depdendent expressions


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
Type&
declared_type(Expr& e)
{
  if (Decl_expr* d = as<Decl_expr>(&e))
    return declared_type(d->declaration());
  else
    return e.type();
}


Type const&
declared_type(Expr const& e)
{
  return declared_type(modify(e));
}


} // namespace banjo
