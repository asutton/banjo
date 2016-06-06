// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Literal expressions

Void_expr&
Builder::get_void()
{
  return void_expr(get_void_type());
}


Void_expr&
Builder::get_void(Type& t)
{
  return void_expr(t);
}


Boolean_expr&
Builder::get_boolean(Type& t, bool b)
{
  return bool_expr(t, b);
}


Boolean_expr&
Builder::get_true(Type& t)
{
  return get_boolean(t, true);
}


Boolean_expr&
Builder::get_false(Type& t)
{
  return get_boolean(t, false);
}


// TODO: Verify that T can have an integer value?
// I think that all scalars can have integer values.
Integer_expr&
Builder::get_integer(Type& t, Integer const& n)
{
  return int_expr(t, n);
}


// Returns the 0 constant, with scalar type `t`.
//
// TODO: What if t is not an integer type?
Integer_expr&
Builder::get_zero(Type& t)
{
  return get_integer(t, 0);
}


// Returns a tuple expression.
Tuple_expr&
Builder::make_tuple(Type& t, Expr_list&& es)
{
  return tuple_expr(t, std::move(es));
}


// Create a reference to a declared object (variable or parameter).
Id_object_expr&
Builder::make_reference(Type& t, Object_decl& d)
{
  return id_object_expr(t, d.name(), d);
}


// Create a reference to a function.
Id_function_expr&
Builder::make_reference(Type& t, Function_decl& d)
{
  return id_function_expr(t, d.name(), d);
}


// Create an expression referring to an overload set. Note that overload
// sets are untyped and have no value category.
Id_overload_expr&
Builder::make_reference(Name& n, Decl_list&& ds)
{
  return id_overload_expr(n, std::move(ds));
}


Dot_object_expr&
Builder::make_reference(Type& t, Expr& e, Field_decl& d)
{
  return dot_object_expr(t, e, d.name(), d);
}


Dot_function_expr&
Builder::make_reference(Type& t, Expr& e, Method_decl& d)
{
  return dot_function_expr(t, e, d.name(), d);
}


Dot_overload_expr&
Builder::make_reference(Expr& e, Name& n, Decl_list&& ds)
{
  lingo_unreachable();
}


And_expr&
Builder::make_and(Type& t, Expr& e1, Expr& e2)
{
  return and_expr(t, e1, e2);
}


Or_expr&
Builder::make_or(Type& t, Expr& e1, Expr& e2)
{
  return or_expr(t, e1, e2);
}


Not_expr&
Builder::make_not(Type& t, Expr& e)
{
  return not_expr(t, e);
}


Eq_expr&
Builder::make_eq(Type& t, Expr& e1, Expr& e2)
{
  return eq_expr(t, e1, e2);
}


Ne_expr&
Builder::make_ne(Type& t, Expr& e1, Expr& e2)
{
  return ne_expr(t, e1, e2);
}


Lt_expr&
Builder::make_lt(Type& t, Expr& e1, Expr& e2)
{
  return lt_expr(t, e1, e2);
}


Gt_expr&
Builder::make_gt(Type& t, Expr& e1, Expr& e2)
{
  return gt_expr(t, e1, e2);
}


Le_expr&
Builder::make_le(Type& t, Expr& e1, Expr& e2)
{
  return le_expr(t, e1, e2);
}


Ge_expr&
Builder::make_ge(Type& t, Expr& e1, Expr& e2)
{
  return ge_expr(t, e1, e2);
}


Add_expr&
Builder::make_add(Type& t, Expr& e1, Expr& e2)
{
  return add_expr(t, e1, e2);
}


Sub_expr&
Builder::make_sub(Type& t, Expr& e1, Expr& e2)
{
  return sub_expr(t, e1, e2);
}


Mul_expr&
Builder::make_mul(Type& t, Expr& e1, Expr& e2)
{
  return mul_expr(t, e1, e2);
}


Div_expr&
Builder::make_div(Type& t, Expr& e1, Expr& e2)
{
  return div_expr(t, e1, e2);
}


Rem_expr&
Builder::make_rem(Type& t, Expr& e1, Expr& e2)
{
  return rem_expr(t, e1, e2);
}


Neg_expr&
Builder::make_neg(Type& t, Expr& e)
{
  return neg_expr(t, e);
}


Pos_expr&
Builder::make_pos(Type& t, Expr& e)
{
  return pos_expr(t, e);
}


Bit_and_expr&
Builder::make_bit_and(Type& t, Expr& e1, Expr& e2)
{
  return bit_and_expr(t, e1, e2);
}


Bit_or_expr&
Builder::make_bit_or(Type& t, Expr& e1, Expr& e2)
{
  return bit_or_expr(t, e1, e2);
}


Bit_xor_expr&
Builder::make_bit_xor(Type& t, Expr& e1, Expr& e2)
{
  return bit_xor_expr(t, e1, e2);
}


Bit_lsh_expr&
Builder::make_bit_lsh(Type& t, Expr& e1, Expr& e2)
{
  return bit_lsh_expr(t, e1, e2);
}


Bit_rsh_expr&
Builder::make_bit_rsh(Type& t, Expr& e1, Expr& e2)
{
  return bit_rsh_expr(t, e1, e2);
}


Bit_not_expr&
Builder::make_bit_not(Type& t, Expr& e)
{
  return bit_not_expr(t, e);
}


Call_expr&
Builder::make_call(Type& t, Expr& f, Expr_list&& a)
{
  return call_expr(t, f, std::move(a));
}


// -------------------------------------------------------------------------- //
// Initializers

Trivial_init&
Builder::make_trivial_init()
{
  return trivial_init(get_void_type());
}


Copy_init&
Builder::make_copy_init(Expr& e)
{
  return copy_init(get_void_type(), e);
}


Bind_init&
Builder::make_bind_init(Expr& e)
{
  return bind_init(get_void_type(), e);
}


Direct_init&
Builder::make_direct_init(Decl& d, Expr_list const& es)
{
  return direct_init(get_void_type(), d, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list const& es)
{
  return aggregate_init(t, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list&& es)
{
  return aggregate_init(t, std::move(es));
}


} // namespace banjo
