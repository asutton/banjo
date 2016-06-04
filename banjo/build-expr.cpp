// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "build-expr.hpp"

#include <unordered_set>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Literal expressions

Void_expr&
Expr_builder::get_void(Type t)
{
  return void_expr(t);
}


Boolean_expr&
Expr_builder::get_boolean(Type t, bool b)
{
  return bool_expr(t, b);
}


Boolean_expr&
Expr_builder::get_true(Type t)
{
  return get_boolean(t, true);
}


Boolean_expr&
Expr_builder::get_false(Type t)
{
  return get_boolean(t, false);
}


// TODO: Verify that T can have an integer value?
// I think that all scalars can have integer values.
Integer_expr&
Expr_builder::get_integer(Type t, Integer const& n)
{
  return int_expr(t, n);
}


// Returns the 0 constant, with scalar type `t`.
//
// TODO: What if t is not an integer type?
Integer_expr&
Expr_builder::get_zero(Type t)
{
  return get_integer(t, 0);
}


// Returns a tuple expression.
Tuple_expr&
Expr_builder::make_tuple(Type t, Expr_list&& es)
{
  return tuple_expr(t, std::move(es));
}


// Create a reference to a declared object (variable or parameter).
Id_object_expr&
Expr_builder::make_reference(Type t, Object_decl& d)
{
  return id_object_expr(t, d.name(), d);
}


// Create a reference to a function.
Id_function_expr&
Expr_builder::make_reference(Type t, Function_decl& d)
{
  return id_function_expr(t, d.name(), d);
}


// Create an expression referring to an overload set. Note that overload
// sets are untyped and have no value category.
Id_overload_expr&
Expr_builder::make_reference(Name& n, Decl_list&& ds)
{
  return id_overload_expr(n, std::move(ds));
}


Dot_object_expr&
Expr_builder::make_reference(Type t, Expr& e, Field_decl& d)
{
  return dot_object_expr(t, e, d.name(), d);
}


Dot_function_expr&
Expr_builder::make_reference(Type t, Expr& e, Method_decl& d)
{
  return dot_function_expr(t, e, d.name(), d);
}


Dot_overload_expr&
Expr_builder::make_reference(Expr& e, Name& n, Decl_list&& ds)
{
  lingo_unreachable();
}


And_expr&
Expr_builder::make_and(Type t, Expr& e1, Expr& e2)
{
  return and_expr(t, e1, e2);
}


Or_expr&
Expr_builder::make_or(Type t, Expr& e1, Expr& e2)
{
  return or_expr(t, e1, e2);
}


Not_expr&
Expr_builder::make_not(Type t, Expr& e)
{
  return not_expr(t, e);
}


Eq_expr&
Expr_builder::make_eq(Type t, Expr& e1, Expr& e2)
{
  return eq_expr(t, e1, e2);
}


Ne_expr&
Expr_builder::make_ne(Type t, Expr& e1, Expr& e2)
{
  return ne_expr(t, e1, e2);
}


Lt_expr&
Expr_builder::make_lt(Type t, Expr& e1, Expr& e2)
{
  return lt_expr(t, e1, e2);
}


Gt_expr&
Expr_builder::make_gt(Type t, Expr& e1, Expr& e2)
{
  return gt_expr(t, e1, e2);
}


Le_expr&
Expr_builder::make_le(Type t, Expr& e1, Expr& e2)
{
  return le_expr(t, e1, e2);
}


Ge_expr&
Expr_builder::make_ge(Type t, Expr& e1, Expr& e2)
{
  return ge_expr(t, e1, e2);
}


Add_expr&
Expr_builder::make_add(Type t, Expr& e1, Expr& e2)
{
  return add_expr(t, e1, e2);
}


Sub_expr&
Expr_builder::make_sub(Type t, Expr& e1, Expr& e2)
{
  return sub_expr(t, e1, e2);
}


Mul_expr&
Expr_builder::make_mul(Type t, Expr& e1, Expr& e2)
{
  return mul_expr(t, e1, e2);
}


Div_expr&
Expr_builder::make_div(Type t, Expr& e1, Expr& e2)
{
  return div_expr(t, e1, e2);
}


Rem_expr&
Expr_builder::make_rem(Type t, Expr& e1, Expr& e2)
{
  return rem_expr(t, e1, e2);
}


Neg_expr&
Expr_builder::make_neg(Type t, Expr& e)
{
  return neg_expr(t, e);
}


Pos_expr&
Expr_builder::make_pos(Type t, Expr& e)
{
  return pos_expr(t, e);
}


Bit_and_expr&
Expr_builder::make_bit_and(Type t, Expr& e1, Expr& e2)
{
  return bit_and_expr(t, e1, e2);
}


Bit_or_expr&
Expr_builder::make_bit_or(Type t, Expr& e1, Expr& e2)
{
  return bit_or_expr(t, e1, e2);
}


Bit_xor_expr&
Expr_builder::make_bit_xor(Type t, Expr& e1, Expr& e2)
{
  return bit_xor_expr(t, e1, e2);
}


Bit_lsh_expr&
Expr_builder::make_bit_lsh(Type t, Expr& e1, Expr& e2)
{
  return bit_lsh_expr(t, e1, e2);
}


Bit_rsh_expr&
Expr_builder::make_bit_rsh(Type t, Expr& e1, Expr& e2)
{
  return bit_rsh_expr(t, e1, e2);
}


Bit_not_expr&
Expr_builder::make_bit_not(Type t, Expr& e)
{
  return bit_not_expr(t, e);
}


Call_expr&
Expr_builder::make_call(Type t, Expr& f, Expr_list&& a)
{
  return call_expr(t, f, std::move(a));
}


} // namespace banjo
