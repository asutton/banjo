// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-name.hpp"
#include "ast-decl.hpp"

namespace banjo
{


char const*
Operator_id::spelling() const
{
  switch (op_) {
    case add_op: return "+";
    case sub_op: return "-";
    case mul_op: return "*";
    case div_op: return "/";
    case rem_op: return "%";
    case bit_and_op: return "&";
    case bit_or_op: return "|";
    case bit_xor_op: return "^";
    case bit_not_op: return "~";
    case bit_lsh_op: return "<<";
    case bit_rsh_op: return ">>";
    case eq_op: return "==";
    case ne_op: return "!=";
    case lt_op: return "<";
    case gt_op: return ">";
    case le_op: return "<=";
    case ge_op: return ">=";
    case cmp_op: return "<=>";
    case and_op: return "&&";
    case or_op: return "||";
    case not_op: return "!";
    case call_op: return "()";
    case index_op: return "[]";
    case assign_op: return "=";
    default: lingo_unreachable();
  }
}


Concept_decl const&
Concept_id::declaration() const
{
  return cast<Concept_decl>(*decl);
}


Concept_decl&
Concept_id::declaration()
{
  return cast<Concept_decl>(*decl);
}


Template_decl const&
Template_id::declaration() const
{
  return cast<Template_decl>(*decl);
}


Template_decl&
Template_id::declaration()
{
  return cast<Template_decl>(*decl);
}


} // namespace
