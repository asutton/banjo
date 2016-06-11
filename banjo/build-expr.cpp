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
  return get_void(get_void_type());
}


Void_expr&
Builder::get_void(Type& t)
{
  return Void_expr::make(alloc_, t);
}


Boolean_expr&
Builder::get_boolean(Type& t, bool b)
{
  return Boolean_expr::make(alloc_, t, b);
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
  return Integer_expr::make(alloc_, t, n);
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
Builder::make_tuple(Type& t, Expr_list const& es)
{
  return Tuple_expr::make(alloc_, t, es);
}


// Returns a tuple expression.
Tuple_expr&
Builder::make_tuple(Type& t, Expr_list&& es)
{
  return Tuple_expr::make(alloc_, t, std::move(es));
}


// Returns an id-expression that refers to a declared object.
Object_expr&
Builder::make_reference(Type& t, Object_decl& d)
{
  return Object_expr::make(alloc_, t, d.name(), d);
}


// Returns an id-expression that refers to a declared reference.
Reference_expr&
Builder::make_reference(Type& t, Reference_decl& d)
{
  return Reference_expr::make(alloc_, t, d.name(), d);
}


// Returns an id-expression that refers to a declared function.
Function_expr&
Builder::make_reference(Type& t, Function_decl& d)
{
  return Function_expr::make(alloc_, t, d.name(), d);
}


// Returns an id-expression that refers to a set of declared names.
Overload_expr&
Builder::make_reference(Name& n, Decl_list const& ds)
{
  return Overload_expr::make(alloc_, n, ds);
}


// Returns an id-expression that refers to a set of declared names.
Overload_expr&
Builder::make_reference(Name& n, Decl_list&& ds)
{
  return Overload_expr::make(alloc_, n, std::move(ds));
}


Mem_object_expr&
Builder::make_reference(Type& t, Expr& e, Mem_object_decl& d)
{
  return Mem_object_expr::make(alloc_, t, e, d.name(), d);
}


Mem_reference_expr&
Builder::make_reference(Type& t, Expr& e, Mem_reference_decl& d)
{
  return Mem_reference_expr::make(alloc_, t, e, d.name(), d);
}


Mem_function_expr&
Builder::make_reference(Type& t, Expr& e, Mem_function_decl& d)
{
  return Mem_function_expr::make(alloc_, t, e, d.name(), d);
}


Mem_overload_expr&
Builder::make_reference(Expr& e, Name& n, Decl_list const& ds)
{
  return Mem_overload_expr::make(alloc_, e, n, ds);
}


Mem_overload_expr&
Builder::make_reference(Expr& e, Name& n, Decl_list&& ds)
{
  return Mem_overload_expr::make(alloc_, e, n, std::move(ds));
}


And_expr&
Builder::make_and(Type& t, Expr& e1, Expr& e2)
{
  return And_expr::make(alloc_, t, e1, e2);
}


Or_expr&
Builder::make_or(Type& t, Expr& e1, Expr& e2)
{
  return Or_expr::make(alloc_, t, e1, e2);
}


Not_expr&
Builder::make_not(Type& t, Expr& e)
{
  return Not_expr::make(alloc_, t, e);
}


Eq_expr&
Builder::make_eq(Type& t, Expr& e1, Expr& e2)
{
  return Eq_expr::make(alloc_, t, e1, e2);
}


Ne_expr&
Builder::make_ne(Type& t, Expr& e1, Expr& e2)
{
  return Ne_expr::make(alloc_, t, e1, e2);
}


Lt_expr&
Builder::make_lt(Type& t, Expr& e1, Expr& e2)
{
  return Lt_expr::make(alloc_, t, e1, e2);
}


Gt_expr&
Builder::make_gt(Type& t, Expr& e1, Expr& e2)
{
  return Gt_expr::make(alloc_, t, e1, e2);
}


Le_expr&
Builder::make_le(Type& t, Expr& e1, Expr& e2)
{
  return Le_expr::make(alloc_, t, e1, e2);
}


Ge_expr&
Builder::make_ge(Type& t, Expr& e1, Expr& e2)
{
  return Ge_expr::make(alloc_, t, e1, e2);
}


Add_expr&
Builder::make_add(Type& t, Expr& e1, Expr& e2)
{
  return Add_expr::make(alloc_, t, e1, e2);
}


Sub_expr&
Builder::make_sub(Type& t, Expr& e1, Expr& e2)
{
  return Sub_expr::make(alloc_, t, e1, e2);
}


Mul_expr&
Builder::make_mul(Type& t, Expr& e1, Expr& e2)
{
  return Mul_expr::make(alloc_, t, e1, e2);
}


Div_expr&
Builder::make_div(Type& t, Expr& e1, Expr& e2)
{
  return Div_expr::make(alloc_, t, e1, e2);
}


Rem_expr&
Builder::make_rem(Type& t, Expr& e1, Expr& e2)
{
  return Rem_expr::make(alloc_, t, e1, e2);
}


Neg_expr&
Builder::make_neg(Type& t, Expr& e)
{
  return Neg_expr::make(alloc_, t, e);
}


Pos_expr&
Builder::make_pos(Type& t, Expr& e)
{
  return Pos_expr::make(alloc_, t, e);
}


Bit_and_expr&
Builder::make_bit_and(Type& t, Expr& e1, Expr& e2)
{
  return Bit_and_expr::make(alloc_, t, e1, e2);
}


Bit_or_expr&
Builder::make_bit_or(Type& t, Expr& e1, Expr& e2)
{
  return Bit_or_expr::make(alloc_, t, e1, e2);
}


Bit_xor_expr&
Builder::make_bit_xor(Type& t, Expr& e1, Expr& e2)
{
  return Bit_xor_expr::make(alloc_, t, e1, e2);
}


Bit_lsh_expr&
Builder::make_bit_lsh(Type& t, Expr& e1, Expr& e2)
{
  return Bit_lsh_expr::make(alloc_, t, e1, e2);
}


Bit_rsh_expr&
Builder::make_bit_rsh(Type& t, Expr& e1, Expr& e2)
{
  return Bit_rsh_expr::make(alloc_, t, e1, e2);
}


Bit_not_expr&
Builder::make_bit_not(Type& t, Expr& e)
{
  return Bit_not_expr::make(alloc_, t, e);
}


Call_expr&
Builder::make_call(Type& t, Expr& f, Expr_list&& a)
{
  return Call_expr::make(alloc_, t, f, std::move(a));
}


// -------------------------------------------------------------------------- //
// Initializers

Trivial_init&
Builder::make_trivial_init()
{
  return Trivial_init::make(alloc_, get_void_type());
}


Copy_init&
Builder::make_copy_init(Expr& e)
{
  return Copy_init::make(alloc_, get_void_type(), e);
}


Bind_init&
Builder::make_bind_init(Expr& e)
{
  return Bind_init::make(alloc_, get_void_type(), e);
}


Direct_init&
Builder::make_direct_init(Decl& d, Expr_list const& es)
{
  return Direct_init::make(alloc_, get_void_type(), d, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list const& es)
{
  return Aggregate_init::make(alloc_, t, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list&& es)
{
  return Aggregate_init::make(alloc_, t, std::move(es));
}


} // namespace banjo
