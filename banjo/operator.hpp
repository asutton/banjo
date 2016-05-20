// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_OPERATOR_HPP
#define BANJO_CORE_OPERATOR_HPP

// This module contains information related to operator overloading.

#include "prelude.hpp"


namespace banjo
{

// Kinds of operator names.
//
// TODO: I'm missing bitwise and logical operators.
enum Operator_kind
{
  add_op,    // +
  sub_op,    // -
  mul_op,    // *
  div_op,    // /
  rem_op,    // %
  eq_op,     // ==
  ne_op,     // !=
  lt_op,     // <
  gt_op,     // >
  le_op,     // <=
  ge_op,     // >=
  cmp_op,    // <=>
  and_op,    // &&
  or_op,     // ||
  not_op,    // !
  call_op,   // ()
  index_op,  // []
  assign_op, // =
};


} // namespace banjo


#endif
