// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "constraint.hpp"
#include "ast_cons.hpp"
#include "ast_def.hpp"
#include "builder.hpp"
#include "substitution.hpp"
#include "normalization.hpp"
#include "hash.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Concept expansion

Cons&
expand_expr(Context& cxt, Expression_def& def, Substitution& sub)
{
  Expr& e = substitute(cxt, def.expression(), sub);
  return normalize(cxt, e);
}


// This is backwards from the previous case where we substitute
// first and normalize second. I believe that these operations are
// interchangeable.
Cons&
expand_def(Context& cxt, Concept_def& def, Substitution& sub)
{
  Cons& c1 = normalize(cxt, def);
  return substitute(cxt, c1, sub);
}



// Expand the concept by substituting the template arguments
// throughthe concept's definition and normalizing the result.
//
// TODO: Memoize the expansions.
Cons&
expand(Context& cxt, Concept_cons& c)
{
  Concept_decl& d = c.declaration();
  Decl_list& tparms = d.parameters();
  Term_list& targs = c.arguments();

  // NOTE: Template arguments must have been checked (in kind?)
  // prior to the formation of the constraint. It's should be
  // a semantic requirement of the original check expression.
  Substitution sub(tparms, targs);

  Def& def = d.definition();
  if (Expression_def* expr = as<Expression_def>(&def))
    return expand_expr(cxt, *expr, sub);
  if (Concept_def* body = as<Concept_def>(&def))
    return expand_def(cxt, *body, sub);
  banjo_unhandled_case(def);
}


Cons const&
expand(Context& cxt, Concept_cons const& c)
{
  return expand(cxt, const_cast<Concept_cons&>(c));
}


// -------------------------------------------------------------------------- //
// Admissibility of expressons
//
// Determine the admissibility of an expression within a dependent
// context. Note that this is the slow way of doing it since we
// are doing a full recursion over the constraint in order to find
// a proof of admissibility.

Expr*
admit_expression(Context& cxt, Expr& c, Expr& e)
{
  note("determine admissibility of '{}'", e);
  return admit_expression(cxt, normalize(cxt, c), e);
}


inline Expr*
admit_concept(Context& cxt, Concept_cons& c, Expr& e)
{
  note("looking in {}", c);
  return admit_expression(cxt, expand(cxt, c), e);
}


Expr*
admit_usage(Context& cxt, Expression_cons& c, Expr& e)
{
  note("looking in {}", c);
  Expr& a = c.expression();
  if (is_equivalent(a, e)) {
    note("matched expression {}", e);
    e.ty = a.ty;
    return &e;
  }
  return nullptr;
}


Expr*
admit_usage(Context& cxt, Conversion_cons& c, Expr& e)
{
  note("looking in {}", c);
  return nullptr;
}


// Just look through to the nested constraint.
inline Expr*
admit_parametric(Context& cxt, Parameterized_cons& c, Expr& e)
{
  note("looking in {}", c);
  return admit_expression(cxt, c.constraint(), e);
}


// An expression is admissible for a conjunction of assumptions if
// support is found in either the left or right operand.
Expr*
admit_conjunction(Context& cxt, Conjunction_cons& c, Expr& e)
{
  note("looking in {}", c);
  if (Expr* e1 = admit_expression(cxt, c.left(), e))
    return e1;
  return admit_expression(cxt, c.right(), e);
}


// An expression is admissible for a disjinction of assumptions if
// support is found both the left and right operand.
inline Expr*
admit_disjunction(Context& cxt, Disjunction_cons& c, Expr& e)
{
  note("looking in {}", c);
  if (Expr* e1 = admit_expression(cxt, c.left(), e)) {
    if (Expr* e2 = admit_expression(cxt, c.right(), e)) {
      if (!is_equivalent(e1->type(), e2->type())) {
        error(cxt, "multiple types deduced for '{}'", e);
        throw Translation_error("deduction failed");
      }
      return e1;
    }
  }
  return nullptr;
}


// Determine if the expression `e` is admissible under the given
// constraint set. Returns a fully typed expression if admissible
// and nullptr if not.
//
// FIXME: This needs to be a deduction algorithm. That is, we
// want find a substitution form `e` to some assumed expression.
//
// FIXME: What happens if we have requirements like these:
//
//    |e : t1| /\ |e : t2|
//
// This only seems illogical to assume contradictory things. However,
// from the perspective of checking it doesn't actually matter since
// any proof requires only that e have type t1 or t2. Not both.
//
// Then there's this:
//
//    |e : t1| \/ |e : t2|
//
// Here, we have to show that both are satisfied. I would think that t
// his is never satisfiable since e could never have different types.
// However, we *could* defer that resolution by creating an intersection
// type. But that might not fly.
Expr*
admit_expression(Context& cxt, Cons& c, Expr& e)
{
  struct fn
  {
    Context& cxt;
    Expr&    e;
    Expr* operator()(Cons& c)               { banjo_unhandled_case(c); }
    Expr* operator()(Concept_cons& c)       { return admit_concept(cxt, c, e); }
    Expr* operator()(Expression_cons& c)    { return admit_usage(cxt, c, e); }
    Expr* operator()(Conversion_cons& c)    { return admit_usage(cxt, c, e); }
    Expr* operator()(Parameterized_cons& c) { return admit_parametric(cxt, c, e); }
    Expr* operator()(Conjunction_cons& c)   { return admit_conjunction(cxt, c, e); }
    Expr* operator()(Disjunction_cons& c)   { return admit_disjunction(cxt, c, e); }
  };
  return apply(c, fn{cxt, e});
}


} // namespace banjo
