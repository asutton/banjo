// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "declaration.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "scope.hpp"
#include "lookup.hpp"
#include "overload.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Declaration of names


// Save d in the given overload set. If d conflicts with any errors,
// this function diagnoses the error and throws an exception.
//
// Note that the overload set is only valid when all members have 
// well-formed types.
static inline void
declare(Context& cxt, Overload_set& ovl, Decl& decl)
{
  ovl.push_back(decl);
}


// Add the declaration d to the given scope.
void
declare(Context& cxt, Scope& scope, Decl& decl)
{
  if (Overload_set* ovl = scope.lookup(decl.name()))
    declare(cxt, *ovl, decl);
  else
    scope.bind(decl);
}


// Add the declaration d to the current scope.
void
declare(Context& cxt, Decl& d)
{
  declare(cxt, cxt.current_scope(), d);
}


// -------------------------------------------------------------------------- //
// Declaration of required expressions

// Save the declaration of a required expression.
//
// FIXME: This is a hack. This should be name-based. Otherwise, lookup
// is going to be very, very slow.
//
// FIXME: Who is responsible for guaranteeing non-repetition?
void
declare_required_expression(Context& cxt, Expr& e)
{
  // Requires_scope& s = *cxt.current_requires_scope();
  // s.exprs.push_back(e);
}


// -------------------------------------------------------------------------- //
// Declaration checking

// Given declarations d1 and d2, a declaration error occurs when:
//
//    - d2 changes the meaning of the name declared by d1, or if not that, then
//      - d1 and d2 are both objects, or
//      - d1 and d2 are functions that cannot be overloaded, or
//      - d1 and d2 are types having different kinds.
//
// Note that d1 precedes d2 in lexical order.
//
// TODO: Would it make sense to poison the declaration so that it's not
// analyzed in subsequent passes? We could essentially replace the existing
// overload set with one containing a poisoned declaration. Any expression
// that uses that name would have an invalid type. We could then use this
// to list the places where the error affects use.
void
check_declarations(Context& cxt, Decl const& d1, Decl const& d2)
{
  struct fn
  {
    Context& cxt;
    Decl const& d2;
    void operator()(Decl const& d)           { lingo_unhandled(d); }
    void operator()(Object_decl const& d1)   { return check_declarations(cxt, d1, cast_as(d1, d2)); }
    void operator()(Function_decl const& d1) { return check_declarations(cxt, d1, cast_as(d1, d2)); }
    void operator()(Type_decl const& d1)     { return check_declarations(cxt, d1, cast_as(d1, d2)); }
  };

  if (typeid(d1) != typeid(d2)) {
    // TODO: Get the source location right.
    error(cxt, "declaration changes the meaning of '{}'", d1.name());
    note("'{}' previously declared as:", d1.name());
    
    // TODO: Don't print the definition. It's not germaine to
    // the error. If we have source locations, I wonder if we
    // can just point at the line.
    note("{}", d1);
    throw Declaration_error();
  }
  apply(d1, fn{cxt, d2});
}


void
check_declarations(Context& cxt, Object_decl const& d1, Object_decl const& d2)
{
  struct fn
  {
    char const* operator()(Decl const& d)          { lingo_unhandled(d); }
    char const* operator()(Variable_decl const& d) { return "variable"; }
    char const* operator()(Field_decl const& d)    { return "member variable"; }
    char const* operator()(Object_parm const& d)   { return "parameter"; }
  };
  error(cxt, "redeclaration of {} with the same name", apply(d1, fn{}));
  throw Declaration_error();
}


void
check_declarations(Context& cxt, Function_decl const& d1, Function_decl const& d2)
{
  // FIXME: Actually check overloading rules.
}


void
check_declarations(Context& cxt, Type_decl const& d1, Type_decl const& d2)
{
  Type const& t1 = d1.type();
  Type const& t2 = d2.type();
  if (is_different(t1, t2)) {
    // TODO: Get the source location right.
    error(cxt, "declaration of '{}' as a different kind of type", d1.name());
    note("'{}' previously declared as:", d1.name());
    
    // TODO: Don't print the definition. It's not germaine to
    // the error. If we have source locations, I wonder if we
    // can just point at the line.
    note("{}", d1);
    throw Declaration_error();
  }
}


} // namespace banjo
