// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_EXPRESSION_HPP
#define BANJO_EXPRESSION_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

struct Context;

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

Expr& make_call(Context& cxt, Expr& e, Expr_list&);

Expr& make_tuple_expr(Context& cxt, Expr_list&);

Expr& make_reference(Context& cxt, Name&);
Expr& make_member_reference(Context& cxt, Expr&, Name&);

Expr& make_requirements(Context& cxt, Decl_list const&, Decl_list const&, Req_list const&);


} // namespace banjo


#endif
