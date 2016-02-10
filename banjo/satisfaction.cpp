// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "satisfaction.hpp"
#include "constraint.hpp"
#include "normalization.hpp"
#include "evaluation.hpp"
#include "builder.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// To satisfy a concept check, we must instantiate that
// concept with the given arguments.
inline bool
satisfy_concept(Context& cxt, Concept_cons& c)
{
  return is_satisfied(cxt, expand(cxt, c));
}


// A predicate constraint is satisfied if its expression
// evaluates to true.
inline bool
satisfy_predicate(Context& cxt, Predicate_cons& p)
{
  Value v = evaluate(p.expression());
  return v.get_boolean();
}


// A conjunction is satisfied iff both operands are satisfied.
// The right operand is not evaluated if the left operand is
// not satisfied.
inline bool
satisfy_conjunction(Context& cxt, Conjunction_cons& c)
{
  return is_satisfied(cxt, c.left())
      && is_satisfied(cxt, c.right());
}


// A disjunction is satsifed iff either operand is satisfied. The
// right operand is not evaluated if the left operand is satisfied.
inline bool
satisfy_disjunction(Context& cxt, Disjunction_cons& c)
{
  return is_satisfied(cxt, c.left())
      || is_satisfied(cxt, c.right());
}


// Determine if a constraint c is satisfied.
bool
is_satisfied(Context& cxt, Cons& c)
{
  struct fn
  {
    Context&      cxt;
    bool operator()(Cons& c)             { banjo_unhandled_case(c); }
    bool operator()(Concept_cons& c)     { return satisfy_concept(cxt, c); }
    bool operator()(Predicate_cons& c)   { return satisfy_predicate(cxt, c); }
    bool operator()(Conjunction_cons& c) { return satisfy_conjunction(cxt, c); }
    bool operator()(Disjunction_cons& c) { return satisfy_disjunction(cxt, c); }
  };
  return apply(c, fn{cxt});
}


// Determine if a constraint expression e is satisfied. Note that e
// must be a non-dependent expression. We must have already generated
// e from a prior substitution into a constraint expression to produce
// this non-dependent expression.
bool
is_satisfied(Context& cxt, Expr& e)
{
  return is_satisfied(cxt, normalize(cxt, e));
}


} // namespace banjo
