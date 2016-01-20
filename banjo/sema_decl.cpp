// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

// TODO: Factor each kind of declaration into its own file.

// -------------------------------------------------------------------------- //
// Declarators

Name&
Parser::on_declarator(Name&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Variable declarations

Decl&
Parser::on_variable_declaration(Token, Name&, Type&, Expr&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Function declarations


Decl&
Parser::on_function_declaration(Token, Name&, Decl_list const&, Type&, Expr&)
{
  lingo_unimplemented();
}


Decl&
Parser::on_parameter_declaration(Name&, Type&, Expr&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Namespace declarations


Decl&
Parser::on_namespace_declaration(Token, Name&, Decl_list const&)
{
  lingo_unimplemented();
}


Decl_list
Parser::on_declaration_seq()
{
  return {};
}


} // namespace banjo
