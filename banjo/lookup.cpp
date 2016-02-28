// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lookup.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "scope.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{


Decl_list
unqualified_lookup(Context& cxt, Name const& id)
{
  return unqualified_lookup(cxt, cxt.current_scope(), id);
}


// Returns the non-empty set of declarations for give (unqualified) id.
// Throws an exception if no matching declarations are found.
//
// Lookup ends as soon as a declaration is found for the given name.
//
// TODO: How should we handle non-simple id's like operator-ids
// and conversion function ids.
Decl_list
unqualified_lookup(Context& cxt, Scope& scope, Name const& id)
{
  Scope* p = &scope;
  while (p) {
    // In general, a name used in any context must be declared
    // before it's use. Search this scope for such a declaration.
    if (Overload_set* ovl = p->lookup(id))
      return *ovl;

    // Depending on current scope, we might re-direct the scope
    // to search different things.

    if (Function_scope* s = as<Function_scope>(p)) {
      Decl& f = s->declaration();
      (void)f;

      // TODO: If fn is defined by a qualified-id, then we should
      // should search the scope(s) named in the id before resuming
      // the search.
    }

    else if (Class_scope* cs = as<Class_scope>(p)) {
      Class_decl& c = cs->declaration();
      (void)c;

      // TODO: Include base classes of c. Lookup also depends on
      // the declarative region of c (nested classes, locals, etc).
    }


    else if (Initializer_scope* s = as<Initializer_scope>(p)) {
      Decl& v = s->declaration();
      (void)v;

      // TODO: If v is declared by a qualified-id, then re-direct
      // to the scope of v before working outwards.
    }

    p = p->enclosing_scope();
  }

  throw Lookup_error(cxt, "no matching declaration for '{}'", id);
}


// Simple lookup is a form of unqualified lookup that returns the
// single declaration associated with the name.
Decl&
simple_lookup(Context& cxt, Scope& scope, Name const& id)
{
  Decl_list result = unqualified_lookup(cxt, scope, id);

  // FIXME: Can we find names that are *like* id?
  if (result.empty())
    throw Lookup_error(cxt, "no matching declaration for '{}'", id);

  // FIXME: Find some way of attaching informative diagnotics
  // to the error (i.e., candidates).
  if (result.size() > 1)
    throw Lookup_error(cxt, "lookup of '{}' is ambiguous", id);

  return result.front();
}


/*
Decl_list
qualified_lookup(Scope&, Symbol const&)
{
  return nullptr;
}


Decl_list
argument_dependent_lookup(Scope&, Expr_list&)
{
  return nullptr;
}
*/


static Type_list
get_operand_types(Call_expr& e)
{
  Type_list ts;
  ts.push_back(e.function().type());
  for (Expr& a : e.arguments())
    ts.push_back(a.type());
  return ts;
}


static Type_list
get_operand_types(Expr& e)
{
  struct fn
  {
    Type_list operator()(Expr& e)        { banjo_unhandled_case(e); }
    Type_list operator()(Unary_expr& e)  { return {&e.operand().type()}; }
    Type_list operator()(Binary_expr& e) { return {&e.left().type(), &e.right().type()}; }
    Type_list operator()(Call_expr& e)   { return get_operand_types(e); }
  };
  return apply(e, fn{});
}


// Lookup the expression in the current requirement scope. This
// returns the expressions whose operands have equivalent type or
// nullptr if they no such expression has been declared.
//
// FIXME: This is a hack. Lookup should use an operator-id and
// perform a lookup in a table, and NOT searching through a
// list of requirements.
Expr*
requirement_lookup(Context& cxt, Expr& e)
{
  Requires_scope& s = *cxt.current_requires_scope();

  Type_list t1 = get_operand_types(e); // Yuck.
  for (Expr& e2 : s.exprs) {
    // Expressions of different kinds are not comparable.
    if (typeid(e) != typeid(e2))
      continue;

    // Compare the types of operands.
    Type_list t2 = get_operand_types(e2);
    if (is_equivalent(t1, t2)) {
      return &e;
    }
  }
  return nullptr;
}


// Search for a declaration of a required expression in the current
// requirement scope. The resulting list is empty if no prior
// requirements have been encountered.
Decl_list
required_expression_lookup(Context& cxt, Name const& id)
{
  Decl_list ds;
  Scope& s = *cxt.current_requires_scope();
  if (Overload_set* ovl = s.lookup(id)) {
    // Filter the overload set for expression declarations.
    for (Decl& d : *ovl) {
      if (is<Expression_decl>(&d))
        ds.push_back(d);
    }
  }
  return ds;
}


} // namespace banjo
