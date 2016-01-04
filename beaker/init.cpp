// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace beaker
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
Parser::on_direct_initializer(List*)
{
  return nullptr;
}


Init*
Parser::on_aggregate_initializer(List*)
{
  return nullptr;
}


} // namespace beaker
