// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

// TODO: Factor each kind of declaration into its own file.

// -------------------------------------------------------------------------- //
// Declarators

Name*
Parser::on_declarator(Name*)
{
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Variable declarations

Decl*
Parser::on_variable_declaration(Token, Name*, Type*, Init*)
{
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Function declarations


Decl*
Parser::on_function_declaration(Token, Name*, Decl_list const&, Type*, Init*)
{
  return nullptr;
}


Decl*
Parser::on_parameter_declaration(Name*, Type*, Init*)
{
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Namespace declarations


Decl*
Parser::on_namespace_declaration(Token, Name*, Decl_list const&)
{
  return nullptr;
}


Decl_list
Parser::on_declaration_seq()
{
  return {};
}


} // namespace banjo
