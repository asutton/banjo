// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "lookup.hpp"
#include "template.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// FIXME: Actually perform semantic anlaysis.

Expr&
Parser::on_logical_and_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_and(t, e1, e2);
}


Expr&
Parser::on_logical_or_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_or(t, e1, e2);
}


Expr&
Parser::on_logical_not_expression(Token tok, Expr& e)
{
  Type& t = build.get_bool_type();
  return build.make_not(t, e);
}


Expr&
Parser::on_eq_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_eq(t, e1, e2);
}


Expr&
Parser::on_ne_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_ne(t, e1, e2);
}


Expr&
Parser::on_lt_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_lt(t, e1, e2);
}


Expr&
Parser::on_gt_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_gt(t, e1, e2);
}


Expr&
Parser::on_le_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_le(t, e1, e2);
}


Expr&
Parser::on_ge_expression(Token tok, Expr& e1, Expr& e2)
{
  Type& t = build.get_bool_type();
  return build.make_ge(t, e1, e2);
}


// TODO: This is going to be non-trivial.
Expr&
Parser::on_call_expression(Expr& e, Expr_list& es)
{
  if (Reference_expr* ref = as<Reference_expr>(&e)) {
    Decl& d = ref->declaration();
    Type& t = declared_type(d);
    if (Function_type* f = as<Function_type>(&t))
      return build.make_call(f->return_type(), e, es);

    // FIXME: Handle lambda expressions. Handle objects of class
    // type with overloads of '()'.

    throw Translation_error("'{}' is not callable", e);
  }

  // FIXME: Handle overload sets.

  lingo_unimplemented();
}


// TOOD: Resolve the id as either a variable name, a function
// name, or an overload name. It can be nothing else.
Expr&
Parser::on_id_expression(Name& n)
{
  // If we got an unqualified id, resolve the lookup.
  if (Simple_id* id = as<Simple_id>(&n)) {
    Decl_list decls = unqualified_lookup(current_scope(), *id);

    // FIXME: Specialize the reference based on whether it's
    // a variable or function? Also, handle all of the other
    // things that can be referred to (e.g., overload sets,
    // parameters, etc).
    Decl& d = decls.front();
    if (Variable_decl* v = as<Variable_decl>(&d))
      return build.make_reference(*v);
    if (Function_decl* f = as<Function_decl>(&d))
      return build.make_reference(*f);
    else
      lingo_unimplemented();
  }

  if (Template_id* id = as<Template_id>(&n)) {
    // FIXME: Validate that this is actually a referrable entity.
    // Basically, we're going to perform the same analysis as we
    // do above on the resolved declaration (is it a var, fn, etc.?).
    //
    // FIXME: This needs to refer to a *saved* implicit instantiation
    // and not an arbitrarily created declaration. When the arguments
    // are dependent, this could be the same as the primary template
    // declaration -- or it could be something else altogether.
    Decl& d = specialize_template(cxt, id->declaration(), id->arguments());
    if (Variable_decl* v = as<Variable_decl>(&d))
      return build.make_reference(*v);
    if (Function_decl* f = as<Function_decl>(&d))
      return build.make_reference(*f);
    else
      lingo_unimplemented();
  }


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
  if (Concept_id* id = as<Concept_id>(&n))
    return build.make_check(id->declaration(), id->arguments());

  // TODO: Handle template-ids and qualified-ids.


  lingo_unimplemented();
}


Expr&
Parser::on_boolean_literal(Token, bool b)
{
  return build.get_bool(b);
}


Expr&
Parser::on_integer_literal(Token tok)
{
  Type& t = build.get_int_type();
  Integer n = tok.spelling();
  return build.get_integer(t, n);
}


} // namespace banjo
