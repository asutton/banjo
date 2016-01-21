// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

Expr&
Parser::on_equal_initializer(Expr&)
{
  lingo_unimplemented();
}


Expr&
Parser::on_paren_initializer(Expr_list const&)
{
  lingo_unimplemented();
}


Expr&
Parser::on_brace_initializer(Expr_list const&)
{
  lingo_unimplemented();
}


} // namespace banjo
