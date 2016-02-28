// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "type.hpp"
#include "constraint.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "conversion.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{


static Decl&
declare_required_expression(Context& cxt, Name& id, Expr& e1, Expr& e2)
{
  Type_list parms {&declared_type(e1), &declared_type(e2)};
  Type& ret = make_fresh_type(cxt);
  Type& t = cxt.get_function_type(parms, ret);

  // FIXME: Yuck.
  Decl& d = *new Expression_decl(id, t);

  // Register the new declaration.
  Scope& s = *cxt.current_requires_scope();
  s.bind(d);

  return d;
}


// Within a set of requriements, determine if we've already
// seen a declaration with this name.
static Type&
find_required_expression(Context& cxt, Name& id, Expr& e1, Expr& e2)
{
  // FIXME: All of this looks like it belongs in lookup to me.
  Decl_list ds = required_expression_lookup(cxt, id);
  if (ds.empty()) {
    Decl& d = declare_required_expression(cxt, id, e1, e2);
    std::cout << "HERE: " << id << " : " << declared_type(d) << '\n';
    return declared_type(d);
  } else {
    // Handle the easy case.
    if (ds.size() == 1)
      return declared_type(ds.front());

    // Search the list of declared expressions for a best match.
    banjo_unimplemented("ambiguous required expression");
  }
}



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
make_dependent_relational_expr(Context& cxt, Name& id, Expr& e1, Expr& e2, Make make)
{
  // Unify with previous expressions.
  if (cxt.in_requirements()) {
    Type& t = find_required_expression(cxt, id, e1, e2);
    return make(t, e1, e2);
  }


  // Build a dependent expression.
  Type& t = make_fresh_type(cxt);
  Expr& init = make(t, e1, e2);

  // // Unify with previous expressions.
  // if (cxt.in_requirements())
  //   return make_required_expression(cxt, id, init);

  // Don't check in unconstrained templates.
  if (cxt.in_unconstrained_template())
    return init;

  // Inside a constrained template, search the constraints to
  // determine if the expression is admissible.
  Expr& con = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, con, init))
    return *ret;

  // Search for dependent conversions.
  return make_standard_relational_expr(cxt, e1, e2, make);
}


// Search for an overload of the given operator. The type is determined
// by overload resolution.
//
// FIXME: Try overload resolution.
template<typename Make>
static Expr&
make_regular_relational_expr(Context& cxt, Name& id, Expr& e1, Expr& e2, Make make)
{
  return make_standard_relational_expr(cxt, e1, e2, make);
}


// Determine the result type of the relational expression.
template<typename Make>
static Expr&
make_relational_expr(Context& cxt, Name& id, Expr& e1, Expr& e2, Make make)
{
  Type& t1 = e1.type();
  Type& t2 = e2.type();
  try {
    if (is_dependent_type(t1) || is_dependent_type(t2))
      return make_dependent_relational_expr(cxt, id, e1, e2, make);
    else
      return make_regular_relational_expr(cxt, id, e1, e2, make);
  } catch (Translation_error&) {
    // FIXME: Diagnose the error, but create an expression
    // with a poisoned type.
    throw;
  }
}


Expr&
make_eq(Context& cxt, Expr& e1, Expr& e2)
{
  Name& id = cxt.get_id(eq_op);
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_eq(t, e1, e2);
  };
  return make_relational_expr(cxt, id, e1, e2, make);
}


Expr&
make_ne(Context& cxt, Expr& e1, Expr& e2)
{
  Name& id = cxt.get_id(ne_op);
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_ne(t, e1, e2);
  };
  return make_relational_expr(cxt, id, e1, e2, make);
}


Expr&
make_lt(Context& cxt, Expr& e1, Expr& e2)
{
  Name& id = cxt.get_id(lt_op);
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_lt(t, e1, e2);
  };
  return make_relational_expr(cxt, id, e1, e2, make);
}


Expr&
make_gt(Context& cxt, Expr& e1, Expr& e2)
{
  Name& id = cxt.get_id(gt_op);
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_gt(t, e1, e2);
  };
  return make_relational_expr(cxt, id, e1, e2, make);
}


Expr&
make_le(Context& cxt, Expr& e1, Expr& e2)
{
  Name& id = cxt.get_id(le_op);
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_le(t, e1, e2);
  };
  return make_relational_expr(cxt, id, e1, e2, make);
}


Expr&
make_ge(Context& cxt, Expr& e1, Expr& e2)
{
  Name& id = cxt.get_id(ge_op);
  auto make = [&cxt](Type& t, Expr& e1, Expr& e2) -> Expr& {
    return cxt.make_ge(t, e1, e2);
  };
  return make_relational_expr(cxt, id, e1, e2, make);
}


} // namespace banjo
