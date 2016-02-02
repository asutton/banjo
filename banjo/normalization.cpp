// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "normalization.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

Predicate_cons&
normalize_expr(Context& cxt, Expr& e)
{
  Builder build(cxt);
  return build.make_predicate_constraint(e);
}


Conjunction_cons&
normalize_and(Context& cxt, And_expr& e)
{
  Builder build(cxt);
  Cons& l = normalize(cxt, e.left());
  Cons& r = normalize(cxt, e.right());
  return build.make_conjunction_constraint(l, r);
}


Disjunction_cons&
normalize_or(Context& cxt, Or_expr& e)
{
  Builder build(cxt);
  Cons& l = normalize(cxt, e.left());
  Cons& r = normalize(cxt, e.right());
  return build.make_disjunction_constraint(l, r);
}


Concept_cons&
normalize_check(Context& cxt, Check_expr& e)
{
  Builder build(cxt);
  return build.make_concept_constraint(e.declaration(), e.arguments());
}


// Return the normalized constraint of an expression.
Cons&
normalize(Context& cxt, Expr& e)
{
  struct fn
  {
    Context& cxt;
    Cons& operator()(Expr& e)       { return normalize_expr(cxt, e); }
    Cons& operator()(And_expr& e)   { return normalize_and(cxt, e); }
    Cons& operator()(Or_expr& e)    { return normalize_or(cxt, e); }
    Cons& operator()(Check_expr& e) { return normalize_check(cxt, e); }
  };
  return apply(e, fn{cxt});
}


} // namespace banjo
