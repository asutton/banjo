// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "constraint.hpp"
#include "ast_cons.hpp"
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
  if (Expression_def* expr = as<Expression_def>(&def)) {
    Expr& e = substitute(cxt, expr->expression(), sub);
    return normalize(cxt, e);
  }
  banjo_unhandled_case(def);
}


Cons const&
expand(Context& cxt, Concept_cons const& c)
{
  return expand(cxt, const_cast<Concept_cons&>(c));
}



} // namespace banjo
