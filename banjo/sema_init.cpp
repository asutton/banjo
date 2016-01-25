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
initialize_declaration(Decl& d, Expr& e)
{
  if (Variable_decl* var = as<Variable_decl>(&d))
    var->init = &e;
  else
    lingo_unreachable();
}


// FIXME: This is dumb. We should have a base class that contributes
// a type to the declaration hiearchy (Typed_decl).
static inline Type&
declared_type(Decl& d)
{
  if (Variable_decl* var = as<Variable_decl>(&d))
    return var->type();

  // We can initialize other things too.
  lingo_unreachable();
}


// Perform initialization.
static inline Expr&
do_initialization(Context& cxt, Decl& d, Init& i)
{
  Type& t = declared_type(d);
  Expr& e = initialize(cxt, t, i);
  initialize_declaration(d, e);
  return i;
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


Expr&
Parser::on_equal_initialization(Decl& d, Expr& e)
{
  // FIXME: Call copy initialize.
  Init& i = build.make_equal_init(e);
  return do_initialization(cxt, d, i);
}


Expr&
Parser::on_paren_initialization(Decl& d, Expr_list const& es)
{
  // FIXME: Call paren initialize?
  Init& i = build.make_paren_init(es);
  return do_initialization(cxt, d, i);
}


// TODO: Perform analysis?
Expr&
Parser::on_brace_initialization(Decl& d, Expr_list const& es)
{
  // FIXME: Call brace initialize?
  Init& i = build.make_brace_init(es);
  return do_initialization(cxt, d, i);
}


} // namespace banjo
