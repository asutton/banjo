// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

Expr&
Parser::on_default_initializer()
{
  lingo_unimplemented();
}


Expr&
Parser::on_value_initializer(Expr&)
{
  lingo_unimplemented();
}


Expr&
Parser::on_direct_initializer(Expr_list const&)
{
  lingo_unimplemented();
}


Expr&
Parser::on_aggregate_initializer(Expr_list const&)
{
  lingo_unimplemented();
}


} // namespace banjo
