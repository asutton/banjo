// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILD_EXPR_HPP
#define BANJO_BUILD_EXPR_HPP

#include "build-core.hpp"


namespace banjo
{

// The expression builder creates expression nodes.
struct Expr_builder : Builder_base
{
  using Builder_base::Builder_base;

  // Literals
  Void_expr&     get_void(Type);
  Boolean_expr&  get_boolean(Type, bool);
  Boolean_expr&  get_true(Type);
  Boolean_expr&  get_false(Type);
  Integer_expr&  get_integer(Type, Integer const&);
  Integer_expr&  get_zero(Type);
  Integer_expr&  get_int(Type, Integer const&);
  Integer_expr&  get_uint(Type, Integer const&);
  
  // Aggregates
  Tuple_expr&    make_tuple(Type, Expr_list&&);

  // Resolved references
  Id_object_expr&     make_reference(Type, Object_decl&);
  Id_function_expr&   make_reference(Type, Function_decl&);
  Id_overload_expr&   make_reference(Name&, Decl_list&&);
  Dot_object_expr&    make_reference(Type, Expr&, Field_decl&);
  Dot_function_expr&  make_reference(Type, Expr&, Method_decl&);
  Dot_overload_expr&  make_reference(Expr&, Name&, Decl_list&&);

  // Logical expressions
  And_expr& make_and(Type, Expr&, Expr&);
  Or_expr&  make_or(Type, Expr&, Expr&);
  Not_expr& make_not(Type, Expr&);

  // Relational expressions
  Eq_expr& make_eq(Type, Expr&, Expr&);
  Ne_expr& make_ne(Type, Expr&, Expr&);
  Lt_expr& make_lt(Type, Expr&, Expr&);
  Gt_expr& make_gt(Type, Expr&, Expr&);
  Le_expr& make_le(Type, Expr&, Expr&);
  Ge_expr& make_ge(Type, Expr&, Expr&);

  // Arithmetic expressions
  Add_expr& make_add(Type, Expr&, Expr&);
  Sub_expr& make_sub(Type, Expr&, Expr&);
  Mul_expr& make_mul(Type, Expr&, Expr&);
  Div_expr& make_div(Type, Expr&, Expr&);
  Rem_expr& make_rem(Type, Expr&, Expr&);
  Neg_expr& make_neg(Type, Expr&);
  Pos_expr& make_pos(Type, Expr&);

  // Bitwise expressions
  Bit_and_expr& make_bit_and(Type, Expr&, Expr&);
  Bit_or_expr&  make_bit_or(Type, Expr&, Expr&);
  Bit_xor_expr& make_bit_xor(Type, Expr&, Expr&);
  Bit_lsh_expr& make_bit_lsh(Type, Expr&, Expr&);
  Bit_rsh_expr& make_bit_rsh(Type, Expr&, Expr&);
  Bit_not_expr& make_bit_not(Type, Expr&);

  // Function call
  Call_expr& make_call(Type, Expr&, Expr_list&&);

  Single_factory<Void_expr> void_expr;
  Basic_factory<Boolean_expr> bool_expr;
  Basic_factory<Integer_expr> int_expr;
  Basic_factory<Tuple_expr> tuple_expr;
  Basic_factory<Id_object_expr> id_object_expr;
  Basic_factory<Id_function_expr> id_function_expr;
  Basic_factory<Id_overload_expr> id_overload_expr;
  Basic_factory<Dot_object_expr> dot_object_expr;
  Basic_factory<Dot_function_expr> dot_function_expr;
  Basic_factory<Dot_overload_expr> dot_overload_expr;
  Basic_factory<And_expr> and_expr;
  Basic_factory<Or_expr> or_expr;
  Basic_factory<Not_expr> not_expr;
  Basic_factory<Eq_expr> eq_expr;
  Basic_factory<Ne_expr> ne_expr;
  Basic_factory<Lt_expr> lt_expr;
  Basic_factory<Gt_expr> gt_expr;
  Basic_factory<Le_expr> le_expr;
  Basic_factory<Ge_expr> ge_expr;
  Basic_factory<Add_expr> add_expr;
  Basic_factory<Sub_expr> sub_expr;
  Basic_factory<Mul_expr> mul_expr;
  Basic_factory<Div_expr> div_expr;
  Basic_factory<Rem_expr> rem_expr;
  Basic_factory<Neg_expr> neg_expr;
  Basic_factory<Pos_expr> pos_expr;
  Basic_factory<Bit_and_expr> bit_and_expr;
  Basic_factory<Bit_or_expr> bit_or_expr;
  Basic_factory<Bit_xor_expr> bit_xor_expr;
  Basic_factory<Bit_lsh_expr> bit_lsh_expr;
  Basic_factory<Bit_rsh_expr> bit_rsh_expr;
  Basic_factory<Bit_not_expr> bit_not_expr;
  Basic_factory<Call_expr> call_expr;
};


} // namespace banjo


#endif
