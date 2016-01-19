// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Identifiers

Name*
Parser::on_simple_id(Token)
{
  return nullptr;
}


Name*
Parser::on_destructor_id(Token, Type*)
{
  return nullptr;
}


Name*
Parser::on_operator_id()
{
  return nullptr;
}


Name*
Parser::on_conversion_id()
{
  return nullptr;
}


Name*
Parser::on_literal_id()
{
  return nullptr;
}


Name*
Parser::on_template_id(Token, Decl*, Term_list const&)
{
  return nullptr;
}


Name*
Parser::on_qualified_id(Name*, Name*)
{
  return nullptr;
}


Term_list
Parser::on_template_argument_list()
{
  return {};
}


// -------------------------------------------------------------------------- //
// Nested name specifiers

Name*
Parser::on_nested_name_specifier(Token)
{
  return nullptr;
}


Name*
Parser::on_nested_name_specifier(Decl*)
{
  return nullptr;
}


Name*
Parser::on_nested_name_specifier(Type*)
{
  return nullptr;
}


Name*
Parser::on_nested_name_specifier(Name*, Token)
{
  return nullptr;
}


Name*
Parser::on_nested_name_specifier(Name*, Name*)
{
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Resolved names

Type*
Parser::on_class_name(Token)
{
  return nullptr;
}


Type*
Parser::on_class_name(Name*)
{
  return nullptr;
}


Type*
Parser::on_union_name(Token)
{
  return nullptr;
}


Type*
Parser::on_union_name(Name*)
{
  return nullptr;
}


Type*
Parser::on_enum_name(Token)
{
  return nullptr;
}


Type*
Parser::on_enum_name(Name*)
{
  return nullptr;
}


Type*
Parser::on_type_alias(Token)
{
  return nullptr;
}


Type*
Parser::on_type_alias(Name*)
{
  return nullptr;
}


Decl*
Parser::on_namespace_name(Token)
{
  return nullptr;
}


Decl*
Parser::on_namespace_name(Name*)
{
  return nullptr;
}


Decl*
Parser::on_namespace_alias(Token)
{
  return nullptr;
}


Decl*
Parser::on_namespace_alias(Name*)
{
  return nullptr;
}


Decl*
Parser::on_template_name(Token)
{
  return nullptr;
}


} // namespace banjo
