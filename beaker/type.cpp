// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace beaker
{

Type*
Parser::on_simple_type(Token)
{
  return nullptr;
}


Type*
Parser::on_decltype_type(Token, Expr*)
{
  return nullptr;
}


Type*
Parser::on_function_type(Type_list const&, Type*)
{
  return nullptr;
}


Type*
Parser::on_pointer_type(Token, Type*)
{
  return nullptr;
}


Type*
Parser::on_reference_type(Token, Type*)
{
  return nullptr;
}


Type_list
Parser::on_type_list()
{
  return {};
}


} // namespace beaker
