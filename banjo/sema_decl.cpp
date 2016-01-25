// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "scope.hpp"
#include "overload.hpp"

#include <iostream>


namespace banjo
{

// TODO: Factor each kind of declaration into its own file.


// -------------------------------------------------------------------------- //
// Declaration
//
// TODO: Move this into a separate semantic module.

using Binding = Scope::Binding;


Decl* declare(Namespace_decl&, Decl&);
Decl* declare(Overload_set&, Decl&);


// Save a declaration `d` in the declaration context `cxt`.
// If `d` is a re-declaration, returns a pointer to (the most
// recent version of) `d`.
Decl*
declare(Decl& cxt, Decl& d)
{
  struct fn
  {
    Decl& d;
    Decl* operator()(Decl& cxt)           { lingo_unimplemented(); }
    Decl* operator()(Namespace_decl& cxt) { return declare(cxt, d); }
  };
  return apply(cxt, fn{d});
}


// FIXME: If `d`'s name is a qualified-id, then we need to adjust
// the context to that specified by `d`s nested name specifier.
Decl*
declare(Namespace_decl& cxt, Decl& d)
{
  Scope& scope = *cxt.scope();

  // If there is already a declaration of `d` in this scope, then
  // we are either re-declaring `d` or adding an overload.
  //
  // TODO: Implement re-declaration.
  Decl* r = nullptr;
  if (Binding* bind = scope.lookup(d.declared_name()))
    declare(bind->second, d);
  else
    scope.bind(d);

  // If declaration is successful, add `d` to the list of
  // declarations in `cxt`.
  cxt.members().push_back(d);

  return r;
}


// Save `d` as a new declaration in the given overload set.
//
// TODO: Implement me.
//
// TODO: Actually check for overloading and re-definition
// errors.
Decl*
declare(Overload_set& ovl, Decl& d)
{
  lingo_unimplemented();
  return nullptr;
}


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


Variable_decl&
Parser::on_variable_declaration(Token, Name& n, Type& t)
{
  Decl& cxt = current_context();
  Variable_decl& var = build.make_variable(n, t);

  // FIXME: Actually declare the variable.
  declare(cxt, var);

  // FIXME: Select a default initializers for t.

  return var;
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
