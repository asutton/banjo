// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

Expr*
Parser::on_id_expression(Name*)
{
  return nullptr;
}


Expr*
Parser::on_integer_literal(Token)
{
  return nullptr;
}


} // namespace banjo
