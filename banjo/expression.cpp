// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "builder.hpp"
#include "type.hpp"
#include "lookup.hpp"
#include "template.hpp"
#include "equivalence.hpp"
#include "initialization.hpp"
#include "conversion.hpp"
#include "constraint.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Identifiers

// Return a reference to the given declaration.
//
// FIXME: Specialize the reference based on whether it's a variable
// or function? Also, handle all of the other things that can be
// referred to (e.g., overload sets, parameters, etc).
Expr&
make_reference(Context& cxt, Decl& d)
{
  Builder build(cxt);

  // TODO: What other kinds of objects do we have here...
  //
  // TODO: Dispatch.
  if (Variable_decl* v = as<Variable_decl>(&d))
    return build.make_reference(*v);
  if (Object_parm* p = as<Object_parm>(&d))
    return build.make_reference(*p);
  if (Function_decl* f = as<Function_decl>(&d))
    return build.make_reference(*f);

  // Here are some things that lookup can find that are not
  // valid expressions.
  //
  // TODO: Diagnose the error and point to the declaration.
  if (Type_decl* t = as<Type_decl>(&d))
    throw Type_error("'{}' is not an object or function", t->name());
  if (Namespace_decl* ns = as<Namespace_decl>(&d))
    throw Type_error("'{}' is not an object or function", ns->name());

  banjo_unhandled_case(d);
}


// Perform unqualified lookup.
Expr&
make_reference(Context& cxt, Simple_id& id)
{
  Decl_list decls = unqualified_lookup(cxt.current_scope(), id);
  if (decls.size() == 1)
    return make_reference(cxt, decls.front());

  // TODO: Return a reference to an overload set.
  banjo_unhandled_case(id);
}


Expr&
make_reference(Context& cxt, Template_id& id)
{
  // FIXME: Validate that this is actually a referrable entity.
  // Basically, we're going to perform the same analysis as we
  // do above on the resolved declaration (is it a var, fn, etc.?).
  //
  // FIXME: This needs to refer to a *saved* implicit instantiation
  // and not an arbitrarily created declaration. When the arguments
  // are dependent, this could be the same as the primary template
  // declaration -- or it could be something else altogether.
  Template_decl& tmp = id.declaration();
  Term_list& args = id.arguments();
  Decl& d = specialize_template(cxt, tmp, args);
  return make_reference(cxt, d);
}


Expr&
make_reference(Context& cxt, Concept_id& id)
{
  Builder build(cxt);

  // FIXME: There are a lot of questions to ask here... Presumably,
  // I must ensure that this resoles to a legitimate check, and the
  // arguments should match in kind (and type?). What if they don't.
  //
  // Also, if the arguments are non-dependent, should I fold the
  // constant?
  //
  // If the arguments are dependent, should I enter into a constrained
  // scope?
  //
  // As mentioned... lots of interesting things to do here.
  return build.make_check(id.declaration(), id.arguments());
}


Expr&
make_reference(Context& cxt, Name& n)
{
  struct fn
  {
    Context& cxt;
    Expr& operator()(Name& n)        { banjo_unhandled_case(n); }
    Expr& operator()(Simple_id& n)   { return make_reference(cxt, n); }
    Expr& operator()(Template_id& n) { return make_reference(cxt, n); }
    Expr& operator()(Concept_id& n)  { return make_reference(cxt, n); }
  };
  return apply(n, fn{cxt});
}


// -------------------------------------------------------------------------- //
// Logical expressions

// Unary logical expressions

// Build a standard unary logical operator. The operand is contextually
// converted to bool, and the result type is bool.
template<typename Make>
static Expr&
make_standard_logical_expr(Context& cxt, Expr& e, Make make)
{
  Expr& c = contextual_conversion_to_bool(cxt, e);
  Type& t = cxt.get_bool_type();
  return make(t, c);
}


// Build a dependent unary logical expression. See comments on the
// binary overload for details.
template<typename Make>
static Expr&
make_dependent_logical_expr(Context& cxt, Expr& e, Make make)
{
  // Build a dependent expression.
  Type& t = make_fresh_type(cxt);
  Expr& f = make(t, e);

  // Don't do lookup in requirements or unconstrained templates.
  if (cxt.in_requirements())
    return f;
  if (!cxt.current_template_constraints())
    return f;

  // Inside a constrained template, search the constraints to determine
  // if the expression is admissible.
  Expr& con = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, con, f))
    return *ret;

  // If no no such expression is admitted by the constraints, then
  // we must be using the standard operator. Dependent conversions
  // may apply.
  return make_standard_logical_expr(cxt, e, make);
}


template<typename Make>
static Expr&
make_regular_logical_expr(Context& cxt, Expr& e, Make make)
{
  return make_standard_logical_expr(cxt, e, make);
}


// Determine the result type of logical expression.
template<typename Make>
static Expr&
make_logical_expr(Context& cxt, Expr& e, Make make)
{
  Type& t = e.type();
  try {
    if (is_dependent_type(t))
      return make_dependent_logical_expr(cxt, e, make);
    else
      return make_regular_logical_expr(cxt, e, make);
  } catch (Translation_error&) {
    error(cxt, "no matching operator");
    throw;
  }
}


// Binary logical expressions

// Build a standard binary logical operators. The operands are
// contextually converted to bool, and the result type is bool.
template<typename Make>
static Expr&
make_standard_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Expr& c1 = contextual_conversion_to_bool(cxt, e1);
  Expr& c2 = contextual_conversion_to_bool(cxt, e2);
  Type& t = cxt.get_bool_type();
  return make(t, c1, c2);
}


// Build a dependent binary logical expression.
//
// TODO: If either expression has occurred previously, then we should
// use it's result type and not generate a fresh type. Also, should the
// initial typing of the expression be a fresh type or the bottom type?
//
// FIXME: If we're in requirements, it might also be nice to unify the
// types of expressions. However, there isn't currently a mechanism to
// do that.
template<typename Make>
static Expr&
make_dependent_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  // Build a dependent expression.
  Type& t = make_fresh_type(cxt);
  Expr& f = make(t, e1, e2);

  // Don't do dependent when parsing requirements.
  if (cxt.in_requirements())
    return f;
  if (!cxt.current_template_constraints())
    return f;

  // Inside a constrained template, search the constraints to determine
  // if the expression is admissible.
  Expr& con = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, con, f))
    return *ret;

  // If no no such expression is admitted by the constraints, then
  // we must be using the standard operator. Dependent conversions
  // may apply.
  return make_standard_logical_expr(cxt, e1, e2, make);
}



// Build a non-dependent logical operator. If either e1 or e2 is
// a user-defined type, then this may find an overloaded operator.
template<typename Make>
static Expr&
make_regular_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  // FIXME: search for overloaded operators.
  return make_standard_logical_expr(cxt, e1, e2, make);
}


// Determine the result type of logical expression.
template<typename Make>
static Expr&
make_logical_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Type& t1 = e1.type();
  Type& t2 = e2.type();
  try {
    if (is_dependent_type(t1) || is_dependent_type(t2))
      return make_dependent_logical_expr(cxt, e1, e2, make);
    else
      return make_regular_logical_expr(cxt, e1, e2, make);
  } catch (Translation_error&) {
    error(cxt, "no matching operator");
    throw;
  }
}


Expr&
make_logical_and(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_and(t, e1, e2);
  };
  return make_logical_expr(cxt, e1, e2, make);
}


Expr&
make_logical_or(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_or(t, e1, e2);
  };
  return make_logical_expr(cxt, e1, e2, make);
}


Expr&
make_logical_not(Context& cxt, Expr& e)
{
  Builder build(cxt);
  Expr& c = contextual_conversion_to_bool(cxt, e);
  Type& t = c.type();
  return build.make_not(t, c);
}


// -------------------------------------------------------------------------- //
// Relational expressions


// Apply the usual arithmetic conversions. The result type of
// the expression is that determined by the conversions.
template<typename Make>
static Expr&
make_standard_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Expr_pair conv = arithmetic_conversion(e1, e2);
  Type& t = e1.type();
  return make(t, conv.first, conv.second);
}


// If either expression has dependent type, then the type of the
// expression is a fresh type.
//
// TODO: If either expression has occurred previously, then we should
// use it's result type and not generate a fresh type.
template<typename Make>
static Expr&
make_dependent_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  // Build a dependent expression.
  Type& t = make_fresh_type(cxt);
  Expr& e = make(t, e1, e2);

  // Don't do dependent lookup.
  if (cxt.in_requirements())
    return e;

  // Inside a constrained template, search the constraints to
  // determine if the expression is admissible.
  //
  // TODO: In fully generality, we have to accumulate constraints
  // in each scope and search that.
  //
  // TODO: This would be more lookup-oriented if we were decomposing
  // constraints on the fly.
  if (Expr* con = cxt.current_template_constraints()) {
    // FIXME: Do we need to transform ret or not?
    if (Expr* ret = admit_expression(cxt, *con, e))
      return *ret;
    warning(cxt, "expression '{}' may result in substitution failure", e);
    return e;
  }

  banjo_unimplemented("unconstrained lookup");
}


// Search for an overload of the given operator. The type is determined
// by overload resolution.
//
// FIXME: This doesn't say which operator!
template<typename Make>
static Expr&
make_overloaded_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  lingo_unimplemented();
}


// Determine the result type of the relational expression.
template<typename Make>
static Expr&
make_relational_expr(Context& cxt, Expr& e1, Expr& e2, Make make)
{
  Type& t1 = e1.type();
  Type& t2 = e2.type();
  try {
    if (is_dependent_type(t1) || is_dependent_type(t2))
      return make_dependent_relational_expr(cxt, e1, e2, make);
    else if (is_maybe_qualified_class_type(t1) || is_maybe_qualified_class_type(t2))
      return make_overloaded_relational_expr(cxt, e1, e2, make);
    else
      return make_standard_relational_expr(cxt, e1, e2, make);
  } catch (Translation_error&) {
    error(cxt, "no matching operator");
    throw;
  }
}


Expr&
make_eq(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_eq(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_ne(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_ne(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_lt(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_lt(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_gt(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_gt(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_le(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_le(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


Expr&
make_ge(Context& cxt, Expr& e1, Expr& e2)
{
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_ge(t, e1, e2);
  };
  return make_relational_expr(cxt, e1, e2, make);
}


// -------------------------------------------------------------------------- //
// Requirements

// TODO: Actually validate informatio about the requires
// expression. The statement cannot be emtpy. No variadic
// parameter, etc.
//
// A requires expression has type bool.
Expr&
make_requirements(Context& cxt,
                  Decl_list const& tparms,
                  Decl_list const& parms,
                  Req_list const& reqs)
{
  return cxt.make_requires(tparms, parms, reqs);
}

} // namespace banjo
