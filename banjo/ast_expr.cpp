// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast_expr.hpp"
#include "ast_type.hpp"
#include "ast_decl.hpp"


namespace banjo
{

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


// Returns true if `e` has class type.
bool
has_class_type(Expr const& e)
{
  return is_class_type(e.type());
}


// Returns true if `e` has union type.
bool
has_union_type(Expr const& e)
{
  return is_union_type(e.type());
}

} // namespace banjo
