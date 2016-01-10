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
Parser::on_pointer_type(Token, Type* t)
{
  return new Pointer_type(*t);
}


// Returns a qualified type. If the type is already qualified,
// then add the new qualifier to the set.
//
// TODO: Verify that we can actually const-qualify this type.
Type*
Parser::on_const_type(Token, Type* t)
{
  if (Qualified_type* q = as<Qualified_type>(t)) {
    q->qual |= const_qual;
    return q;
  } else {
    return new Qualified_type(*t, const_qual);
  }
}


// Returns a qualified type. If the type is already qualified,
// then add the new qualifier to the set.
//
// TODO: Verify that we can actually volatile-qualify this type.
Type*
Parser::on_volatile_type(Token, Type* t)
{
  if (Qualified_type* q = as<Qualified_type>(t)) {
    q->qual |= volatile_qual;
    return q;
  } else {
    return new Qualified_type(*t, volatile_qual);
  }
}


Type*
Parser::on_sequence_type(Type* t)
{
  return new Sequence_type(*t);
}


Type*
Parser::on_reference_type(Token, Type* t)
{
  return new Reference_type(*t);
}


Type_list
Parser::on_type_list()
{
  return {};
}


} // namespace beaker
