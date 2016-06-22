// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_EXPRESSION_HPP
#define BANJO_EXPRESSION_HPP

#include "ast.hpp"
#include "context.hpp"


namespace banjo
{

Expr& make_required_expression(Context&, Expr&);

Expr& make_logical_and(Context&, Expr&, Expr&);
Expr& make_logical_or(Context&, Expr&, Expr&);
Expr& make_logical_not(Context&, Expr&);

Expr& make_eq(Context&, Expr&, Expr&);
Expr& make_ne(Context&, Expr&, Expr&);
Expr& make_lt(Context&, Expr&, Expr&);
Expr& make_gt(Context&, Expr&, Expr&);
Expr& make_le(Context&, Expr&, Expr&);
Expr& make_ge(Context&, Expr&, Expr&);

Expr& make_add(Context&, Expr&, Expr&);
Expr& make_sub(Context&, Expr&, Expr&);
Expr& make_mul(Context&, Expr&, Expr&);
Expr& make_div(Context&, Expr&, Expr&);
Expr& make_rem(Context&, Expr&, Expr&);
Expr& make_neg(Context&, Expr&);
Expr& make_pos(Context&, Expr&);

Expr& make_bit_and(Context&, Expr&, Expr&);
Expr& make_bit_or(Context&, Expr&, Expr&);
Expr& make_bit_xor(Context&, Expr&, Expr&);
Expr& make_bit_lsh(Context&, Expr&, Expr&);
Expr& make_bit_rsh(Context&, Expr&, Expr&);
Expr& make_bit_not(Context&, Expr&);

// Function calls
Expr& make_call(Context&, Expr& e, Expr_list&);

// Tuples
Expr& make_tuple_expr(Context&, Expr_list&);

Expr& make_reference(Context&, Name&);
Expr& make_member_reference(Context&, Expr&, Name&);

Expr& make_requirements(Context&, Decl_list const&, Decl_list const&, Req_list const&);


// Overload resolution
Resolution resolve_call(Context&, Decl_list&, Expr&);
Resolution resolve_call(Context&, Decl_list&, Expr&, Expr&);
Resolution resolve_call(Context&, Decl_list&, Expr_list&);
Resolution resolve_operator(Context&, Operator_kind, Expr&);
Resolution resolve_operator(Context&, Operator_kind, Expr&, Expr&);
Resolution resolve_operator(Context&, Operator_kind, Expr_list&);


// Helper functions for making overloaded operator expressions. This performs
// overload resolution and then constructs a resolved expression using the
// member function pointer.

template<typename T>
T& 
make_operator(Context& cxt, Operator_kind op, Expr& e, T& (Builder::* make)(Type&, Expr&))
{
  Resolution res = resolve_operator(cxt, op, e);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  T& expr = (cxt.*make)(fn.return_type(), args[0]);
  expr.res_ = &fn;
  return expr;
}


template<typename T>
T& 
make_operator(Context& cxt, Operator_kind op, Expr& e1, Expr& e2, T& (Builder::* make)(Type&, Expr&, Expr&))
{
  Resolution res = resolve_operator(cxt, op, e1, e2);
  Function_decl& fn = res.function();
  Expr_list& args = res.arguments();
  T& expr = (cxt.*make)(fn.return_type(), args[0], args[1]);
  expr.res_ = &fn;
  return expr;
}


} // namespace banjo


#endif
