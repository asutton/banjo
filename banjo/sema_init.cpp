// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "initialization.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// Update `d` with the initializer `e`.
static inline void
initialize_declaration(Decl* d, Expr& e)
{
  // Adjust for template declarations.
  //
  // TODO: Make it easier to access the pattern.
  if (Template_decl* tmp = as<Template_decl>(d))
    d = &tmp->pattern();

  if (Variable_decl* var = as<Variable_decl>(d))
    var->init = &e;
  else
    lingo_unreachable();
}


static inline void
initialize_declaration(Decl& d, Expr& e)
{
  return initialize_declaration(&d, e);
}


// Return the type of a declaration.
//
// FIXME: This is dumb. We should have a base class that contributes
// a type to the declaration hiearchy (Typed_decl).
static inline Type&
declared_type(Decl* d)
{
  // Adjust for template declarations.
  //
  // TODO: Make it easier to access the pattern.
  if (Template_decl* tmp = as<Template_decl>(d))
    d = &tmp->pattern();

  if (Variable_decl* var = as<Variable_decl>(d))
    return var->type();

  // We can initialize other things too.
  lingo_unreachable();
}


static inline Type&
declared_type(Decl& d)
{
  return declared_type(&d);
}


// Select a default initializer for `d`.
//
// FIXME: This relies on the construction of placeholder nodes
// in the initialization branch. That seems wrong. We should be
// able to convey syntax via flags.
Expr&
Parser::on_default_initialization(Decl& d)
{
  Type& t = declared_type(d);
  Expr& i = default_initialize(cxt, t);
  initialize_declaration(d, i);
  return i;
}


// Copy initialize the declaration `d` with `e`.
Expr&
Parser::on_equal_initialization(Decl& d, Expr& e)
{
  Expr& i = copy_initialize(cxt, declared_type(d), e);
  initialize_declaration(d, i);
  return i;
}


// Direct initialize `d` by a paren-enclosed list of expressions
// `es`.
Expr&
Parser::on_paren_initialization(Decl& d, Expr_list& es)
{
  Expr& i = direct_initialize(cxt, declared_type(d), es);
  initialize_declaration(d, i);
  return i;
}


// Direct initialzie `d` by a brace-enclosed list of expressions
// `es`.
Expr&
Parser::on_brace_initialization(Decl& d, Expr_list& es)
{
  Expr& i = list_initialize(cxt, declared_type(d), es);
  initialize_declaration(d, i);
  return i;
}


} // namespace banjo
