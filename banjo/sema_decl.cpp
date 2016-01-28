// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "scope.hpp"
#include "overload.hpp"
#include "print.hpp"

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


// Try to declare a name binding in the current scope.
//
// FIXME: The specific rules probably depend on a) the kind of scope,
// and b) the kind of declaration.
//
// FIXME: Check for re-declaration and overloading. This probably
// requires that we match on the entity declared.
//
// FIXME: If `d`'s name is a qualified-id, then we need to adjust
// the context to that specified by `d`s nested name specifier.
Decl*
declare(Scope& scope, Decl& d)
{
  if (Overload_set* ovl = scope.lookup(d.declared_name())) {
    return declare(*ovl, d);
  } else {
    scope.bind(d);
    return nullptr;
  }
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
// Variables


Variable_decl&
Parser::on_variable_declaration(Token, Name& n, Type& t)
{
  Variable_decl& var = build.make_variable(n, t);
  declare(current_scope(), var);
  return var;
}


// -------------------------------------------------------------------------- //
// Functions


Function_decl&
Parser::on_function_declaration(Token, Name& n, Decl_list& ps, Type& t)
{
  Function_decl& fn = build.make_function(n, ps, t);
  declare(current_scope(), fn);
  return fn;
}


Object_parm&
Parser::on_function_parameter(Name& n, Type& t)
{
  Object_parm& parm = build.make_object_parm(n, t);
  declare(current_scope(), parm);
  return parm;
}


// -------------------------------------------------------------------------- //
// Namespaces


Namespace_decl&
Parser::on_namespace_declaration(Token, Name&, Decl_list&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Translation units


// Merge the parsed declarations into the global namespace.
Namespace_decl&
Parser::on_translation_unit(Decl_list& ds)
{
  Namespace_decl& ns = cxt.global_namespace();
  ns.decls.append(ds.begin(), ds.end());
  return ns;
}



} // namespace banjo
