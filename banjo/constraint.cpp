// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "constraint.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "call.hpp"
#include "initialization.hpp"
#include "substitution.hpp"
#include "normalization.hpp"
#include "hash.hpp"
#include "printer.hpp"
#include "inspection.hpp"

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
// Admissibility of expressions
//
// Determine the admissibility of an expression within a dependent
// context. Note that this is the slow way of doing it since we
// are doing a full recursion over the constraint in order to find
// a proof of admissibility.


inline Expr*
admit_concept_expr(Context& cxt, Concept_cons& c, Expr& e)
{
  return admit_expression(cxt, expand(cxt, c), e);
}


// This is plain weird, and I should probably never be here. When
// does a reference-expression appear as a constraint?
template<typename Usage>
Expr*
admit_reference_expr(Context& cxt, Usage& c, Reference_expr& e)
{
  return &e;
}


// For both basic and conversion constraints. Determine if the operands
// of e can be converted to the declared types of the required expression.
// If so, return e with its result type adjusted to that of the required
// expression.
//
// Note that for a conversion constraint, the result type of the matched
// expression is *not* the destinatio type. It is the expression type.
template<typename Usage>
Expr*
admit_binary_expr(Context& cxt, Usage& c, Binary_expr& e)
{
  // Determine if the operands can be converted to the
  // declared type of the required expressin.
  Binary_expr& a = cast<Binary_expr>(c.expression());
  Type& t1 = declared_type(a.left());
  Type& t2 = declared_type(a.right());
  try {
    copy_initialize(cxt, t1, e.left());
    copy_initialize(cxt, t2, e.right());
  } catch(Translation_error&) {
    return nullptr;
  }

  // Adjust the type of the expression under test to that of
  // the required expression.
  //
  // FIXME: See the notes on admit_binary_conv. We may want
  // to preserve the conversions for the purpose of ordering.
  e.ty = &a.type();
  return &e;
}


// Determine if a basic or conversion constraint admit the call
// expression e.
template<typename Usage>
Expr*
admit_call_expr(Context& cxt, Usage& c, Call_expr& e)
{
  Call_expr& a = cast<Call_expr>(c.expression());

  // Build the list of parameter types from the declared types
  // of operands in the constraint.
  Type_list ts {&declared_type(a.function())};
  for (Expr& e0 : a.arguments())
    ts.push_back(declared_type(e0));

  // Build the list of arguments from e. Note that the first
  // argument is actually the function.
  Expr_list es {&e.function()};
  for (Expr& e0 : e.arguments())
    es.push_back(e0);

  // If conversion fails, this is not accessible.
  try {
    initialize_parameters(cxt, ts, es);
  } catch (Translation_error&) {
    return nullptr;
  }

  // Adjust the type and admit the expression.
  e.ty = &a.type();
  return &e;
}


// For both basic and conversion constraints, determine if the constraints
// admits the expression. In general, this is the case when the operands
// of the e are dependently convertible to the types of the expression in c.
template<typename Usage>
Expr*
admit_usage_expr(Context& cxt, Usage& c, Expr& e)
{
  struct fn
  {
    Context& cxt;
    Usage&   c;
    Expr* operator()(Expr& e)           { banjo_unhandled_case(e); }
    Expr* operator()(Reference_expr& e) { return admit_reference_expr(cxt, c, e); }
    Expr* operator()(Binary_expr& e)    { return admit_binary_expr(cxt, c, e); }
    Expr* operator()(Call_expr& e)      { return admit_call_expr(cxt, c, e); }
  };

  // An expression of a different kind prove admissibility.
  if (typeid(c.expression()) != typeid(e))
    return nullptr;

  return apply(e, fn{cxt, c});
}


// Just look through to the nested constraint.
inline Expr*
admit_parametric_expr(Context& cxt, Parameterized_cons& c, Expr& e)
{
  return admit_expression(cxt, c.constraint(), e);
}


// An expression is admissible for a conjunction of assumptions if
// support is found in either the left or right operand.
Expr*
admit_conjunction_expr(Context& cxt, Conjunction_cons& c, Expr& e)
{
  if (Expr* e1 = admit_expression(cxt, c.left(), e))
    return e1;
  else
    return admit_expression(cxt, c.right(), e);
}


// An expression is admissible for a disjinction of assumptions if
// support is found both the left and right operand.
inline Expr*
admit_disjunction_expr(Context& cxt, Disjunction_cons& c, Expr& e)
{
  if (Expr* e1 = admit_expression(cxt, c.left(), e)) {
    if (Expr* e2 = admit_expression(cxt, c.right(), e)) {
      if (!is_equivalent(e1->type(), e2->type()))
        throw Translation_error(cxt, "multiple types deduced for '{}'", e);
      return e1;
    }
  }
  return nullptr;
}


// Determine if the expression `e` is admissible under the given
// constraint set. Returns a fully typed expression if admissible
// and nullptr if not.
//
// Note that a predicate constraint does not admit any expressions.
// That will change when we add axioms.
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
    Expr* operator()(Concept_cons& c)       { return admit_concept_expr(cxt, c, e); }
    Expr* operator()(Predicate_cons& c)     { return nullptr; }
    Expr* operator()(Expression_cons& c)    { return admit_usage_expr(cxt, c, e); }
    Expr* operator()(Conversion_cons& c)    { return admit_usage_expr(cxt, c, e); }
    Expr* operator()(Parameterized_cons& c) { return admit_parametric_expr(cxt, c, e); }
    Expr* operator()(Conjunction_cons& c)   { return admit_conjunction_expr(cxt, c, e); }
    Expr* operator()(Disjunction_cons& c)   { return admit_disjunction_expr(cxt, c, e); }
  };
  // note("admit expr '{}' in '{}'", e, c);
  return apply(c, fn{cxt, e});
}


Expr*
admit_expression(Context& cxt, Expr& c, Expr& e)
{
  return admit_expression(cxt, normalize(cxt, c), e);
}


// -------------------------------------------------------------------------- //
// Availability of conversions

Expr*
admit_concept_conv(Context& cxt, Concept_cons& c, Expr& e, Type& t)
{
  return admit_conversion(cxt, expand(cxt, c), e, t);
}


// Note that, within this function, the the conversion's type
// is equivalent to the destination type.
//
// FIXME: Factor out the "conformance checking" code.
Expr*
admit_binary_conv(Context& cxt, Conversion_cons& c, Binary_expr& e)
{
  // Determine if the operands can be converted to the
  // declared type of the required expressin.
  Binary_expr& a = cast<Binary_expr>(c.expression());
  Type& t1 = declared_type(a.left());
  Type& t2 = declared_type(a.right());
  try {
    copy_initialize(cxt, t1, e.left());
    copy_initialize(cxt, t2, e.right());
  } catch(Translation_error&) {
    return nullptr;
  }

  // Adjust the type of the expression under test to that of
  // the required expression.
  //
  // FIXME: It's possible that we need to preserve the original
  // conversions in order to sort candidates. Consider:
  //
  //    requires (T a, T const b) {
  //      f(a) -> T;        // #1
  //      f(b) -> T const;  // #2
  //    }
  //
  // In an algorithm that uses a f(x) where x is const, we would
  // prefer #1. Perhaps we should collect viable conversion
  // and then sort at the end. Note that this is true for simple
  // typings also.
  //
  // FIXME: Add constructors to the builder. This is just plain dumb.
  return new Dependent_conv(c.type(), e);
}


Expr*
admit_usage_conv(Context& cxt, Conversion_cons& c, Expr& e, Type& t)
{
  struct fn
  {
    Context&         cxt;
    Conversion_cons& c;
    Expr* operator()(Expr& e)        { banjo_unhandled_case(e); }
    Expr* operator()(Binary_expr& e) { return admit_binary_conv(cxt, c, e); }
  };

  // An expression of a different kind prove admissibility.
  Expr& e2 = c.expression();
  if (typeid(e2) != typeid(e))
    return nullptr;

  // If the expression's type is not equivalent to t, this constraint
  // does not prove admissibility.
  if (!is_equivalent(c.type(), t))
    return nullptr;

  return apply(e, fn{cxt, c});
}


Expr*
admit_parametric_conv(Context& cxt, Parameterized_cons& c, Expr& e, Type& t)
{
  return admit_conversion(cxt, c.constraint(), e, t);
}


Expr*
admit_conjunction_conv(Context& cxt, Conjunction_cons& c, Expr& e, Type& t)
{
  if (Expr* c1 = admit_conversion(cxt, c.left(), e, t))
    return c1;
  else
    return admit_conversion(cxt, c.right(), e, t);
}


Expr*
admit_disjunction_conv(Context& cxt, Disjunction_cons& c, Expr& e, Type& t)
{
  // Note that if we find evidence for a conversion in both branches
  // then it is guaranteed that they are equivalent.
  if (Expr* c1 = admit_conversion(cxt, c.left(), e, t)) {
    if (admit_conversion(cxt, c.right(), e, t))
      return c1;
  }
  return nullptr;
}


// TODO: Factor out the traversal of constraints in order to avoid
// repeating all this bolier plate?
Expr*
admit_conversion(Context& cxt, Cons& c, Expr& e, Type& t)
{
  struct fn
  {
    Context& cxt;
    Expr&    e;
    Type&    t;
    Expr* operator()(Cons& c)               { banjo_unhandled_case(c); }
    Expr* operator()(Concept_cons& c)       { return admit_concept_conv(cxt, c, e, t); }
    Expr* operator()(Expression_cons& c)    { return nullptr; }
    Expr* operator()(Conversion_cons& c)    { return admit_usage_conv(cxt, c, e, t); }
    Expr* operator()(Parameterized_cons& c) { return admit_parametric_conv(cxt, c, e, t); }
    Expr* operator()(Conjunction_cons& c)   { return admit_conjunction_conv(cxt, c, e, t); }
    Expr* operator()(Disjunction_cons& c)   { return admit_disjunction_conv(cxt, c, e, t); }
  };
  // note("admit conv '{}' to '{}' in '{}'", e, t, c);
  return apply(c, fn{cxt, e, t});
}


Expr*
admit_conversion(Context& cxt, Expr& c, Expr& e, Type& t)
{
  return admit_conversion(cxt, normalize(cxt, c), e, t);
}


} // namespace banjo
