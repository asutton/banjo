// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

Init*
Parser::on_default_initializer()
{
  return nullptr;
}


Init*
Parser::on_value_initializer(Expr*)
{
  return nullptr;
}


Init*
Parser::on_direct_initializer(Expr_list const&)
{
  return nullptr;
}


Init*
Parser::on_aggregate_initializer(Expr_list const&)
{
  return nullptr;
}


} // namespace banjo
