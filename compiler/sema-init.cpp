// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast-expr.hpp>
#include <banjo/initialization.hpp>


namespace banjo
{

namespace fe
{

// Select a default initializer for `d`.
//
// FIXME: This relies on the construction of placeholder nodes
// in the initialization branch. That seems wrong. We should be
// able to convey syntax via flags.
Expr&
Parser::on_default_initialization(Decl& d)
{
  lingo_unreachable();
  // Type& t = declared_type(d);
  // Expr& i = default_initialize(cxt, t);
  // initialize_declaration(d, i);
  // return i;
}


// Copy initialize the declaration `d` with `e`.
Expr&
Parser::on_equal_initialization(Decl& d, Expr& e)
{
  lingo_unreachable();
  // Expr& i = copy_initialize(cxt, declared_type(d), e);
  // initialize_declaration(d, i);
  // return i;
}


// Direct initialize `d` by a paren-enclosed list of expressions
// `es`.
Expr&
Parser::on_paren_initialization(Decl& d, Expr_list& es)
{
  lingo_unreachable();
  // Expr& i = direct_initialize(cxt, declared_type(d), es);
  // initialize_declaration(d, i);
  // return i;
}


// Direct initialzie `d` by a brace-enclosed list of expressions
// `es`.
Expr&
Parser::on_brace_initialization(Decl& d, Expr_list& es)
{
  lingo_unreachable();
  // Expr& i = list_initialize(cxt, declared_type(d), es);
  // initialize_declaration(d, i);
  // return i;
}


} // namespace fe

} // namespace banjo
