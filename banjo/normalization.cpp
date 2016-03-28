// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "normalization.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

Cons& normalize(Context&, Req_list&);


// -------------------------------------------------------------------------- //
// Normalization of constraint expressions

Predicate_cons&
normalize_expr(Context& cxt, Expr& e)
{
  Builder build(cxt);
  return build.get_predicate_constraint(e);
}


Conjunction_cons&
normalize_and(Context& cxt, And_expr& e)
{
  Builder build(cxt);
  Cons& l = normalize(cxt, e.left());
  Cons& r = normalize(cxt, e.right());
  return build.get_conjunction_constraint(l, r);
}


Disjunction_cons&
normalize_or(Context& cxt, Or_expr& e)
{
  Builder build(cxt);
  Cons& l = normalize(cxt, e.left());
  Cons& r = normalize(cxt, e.right());
  return build.get_disjunction_constraint(l, r);
}


Concept_cons&
normalize_check(Context& cxt, Check_expr& e)
{
  Builder build(cxt);
  return build.get_concept_constraint(e.declaration(), e.arguments());
}


Parameterized_cons&
normalize_reqs(Context& cxt, Requires_expr& e)
{
  Builder build(cxt);
  Decl_list const& parms = e.normal_parameters();
  Cons& cons = normalize(cxt, e.requirements());
  return build.get_parameterized_constraint(parms, cons);
}


// Return the normalized constraint of an expression.
Cons&
normalize(Context& cxt, Expr& e)
{
  struct fn
  {
    Context& cxt;
    Cons& operator()(Expr& e)          { return normalize_expr(cxt, e); }
    Cons& operator()(And_expr& e)      { return normalize_and(cxt, e); }
    Cons& operator()(Or_expr& e)       { return normalize_or(cxt, e); }
    Cons& operator()(Check_expr& e)    { return normalize_check(cxt, e); }
    Cons& operator()(Requires_expr& e) { return normalize_reqs(cxt, e); }
  };
  return apply(e, fn{cxt});
}


// -------------------------------------------------------------------------- //
// Normalization of requirements

Cons&
normalize_type_req(Context& cxt, Type_req& r)
{
  lingo_unimplemented();
}


// The normal form of a basic requirement is an expression
// constraint having that expression and type.
Cons&
normalize_basic_req(Context& cxt, Basic_req& r)
{
  return cxt.get_expression_constraint(r.expression(), r.type());
}


Cons&
normalize_conv_req(Context& cxt, Conversion_req& r)
{
  return cxt.get_conversion_constraint(r.expression(), r.type());
}


// Normalize a requirement.
Cons&
normalize(Context& cxt, Req& r)
{
  struct fn
  {
    Context& cxt;
    Cons& operator()(Type_req& r)       { return normalize_type_req(cxt, r); }
    Cons& operator()(Syntactic_req& r)  { return normalize(cxt, r.expression()); }
    Cons& operator()(Semantic_req& r)   { banjo_unhandled_case(r); }
    Cons& operator()(Expression_req& r) { return normalize(cxt, r.expression()); }
    Cons& operator()(Basic_req& r)      { return normalize_basic_req(cxt, r); }
    Cons& operator()(Conversion_req& r) { return normalize_conv_req(cxt, r); }
    Cons& operator()(Deduction_req& r)  { banjo_unhandled_case(r); }
  };
  return apply(r, fn{cxt});
}


// The normal form of a list of requiremnts is the conjunction
// of their normal forms.
Cons&
normalize(Context& cxt, Req_list& rs)
{
  auto iter = rs.begin();
  Cons* c1 = &normalize(cxt, *iter);
  ++iter;
  while (iter != rs.end()) {
    Cons& c2 = normalize(cxt, *iter);
    c1 = &cxt.get_conjunction_constraint(*c1, c2);
    ++iter;
  }
  return *c1;
}


// The normal form of a concept definition is a conjunction
// of its requirements.
Cons&
normalize(Context& cxt, Concept_def& def)
{
  return normalize(cxt, def.requirements());
}


} // namespace banjo
