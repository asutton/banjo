// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

// TODO: Factor each kind of declaration into its own file.

// -------------------------------------------------------------------------- //
// Declarators

// FIXME: Is there really anything interesting to do here?
// Maybe if the name is qualified, guarantee that it was
// previously defined.
Name&
Parser::on_declarator(Name& n)
{
  return n;
}


// -------------------------------------------------------------------------- //
// Variable declarations


Decl&
Parser::on_variable_declaration(Token, Name& n, Type& t)
{
  Decl& cxt = current_context();
  Variable_decl& var = build.make_variable(n, t);

  // FIXME: Actually declare the variable.

  // FIXME: Determine if we need a default initializer.

  // FIXME: This is ridiculous. Factor the notion of declaration
  // context out of all declarations that share that concept.
  if (Namespace_decl* ns = as<Namespace_decl>(&cxt))
    ns->members().push_back(var);
  var.context(cxt);

  return var;
}


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
Parser::on_parameter_declaration(Name&, Type&)
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
