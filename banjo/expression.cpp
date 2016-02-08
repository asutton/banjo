// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "lookup.hpp"
#include "template.hpp"
#include "equivalence.hpp"
#include "initialization.hpp"
#include "conversion.hpp"
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
  if (Variable_decl* v = as<Variable_decl>(&d))
    return build.make_reference(*v);
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

Expr&
make_logical_and(Context& cxt, Expr& e1, Expr& e2)
{
  Builder build(cxt);
  Expr& c1 = contextual_conversion_to_bool(cxt, e1);
  Expr& c2 = contextual_conversion_to_bool(cxt, e2);
  Type& t = c1.type();
  return build.make_and(t, c1, c2);
}


Expr&
make_logical_or(Context& cxt, Expr& e1, Expr& e2)
{
  Builder build(cxt);
  Expr& c1 = contextual_conversion_to_bool(cxt, e1);
  Expr& c2 = contextual_conversion_to_bool(cxt, e2);
  Type& t = c1.type();
  return build.make_or(t, c1, c2);
}


Expr&
make_logical_not(Context& cxt, Expr& e)
{
  Builder build(cxt);
  Expr& c = contextual_conversion_to_bool(cxt, e);
  Type& t = c.type();
  return build.make_not(t, c);
}


} // namespace banjo
