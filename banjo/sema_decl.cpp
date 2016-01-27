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
  //
  // FIXME: This is fundamentally broken. The addition of declarations
  // to the context may be a somewhat different operation than
  // establishing the binding. For examples, parameters are not
  // added to an enclosing context.
  if (!is<Object_parm>(&d))
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
// Variables


Variable_decl&
Parser::on_variable_declaration(Token, Name& n, Type& t)
{
  Variable_decl& var = build.make_variable(n, t);
  declare(current_context(), var);
  return var;
}


// -------------------------------------------------------------------------- //
// Functions


Function_decl&
Parser::on_function_declaration(Token, Name& n, Decl_list& ps, Type& t)
{
  Function_decl& fn = build.make_function(n, ps, t);
  declare(current_context(), fn);
  return fn;
}


Object_parm&
Parser::on_function_parameter(Name& n, Type& t)
{
  Object_parm& parm = build.make_object_parm(n, t);
  declare(current_context(), parm);
  return parm;
}


// FIXME: Move these into a new module, sema_def.cpp,


static inline void
define_function(Function_decl& fn, Def& def)
{
  fn.def = &def;
}


// A helper function that hides the ugliness of assinging the
// function definition.
static inline void
define_function(Decl& decl, Def& def)
{
  define_function(cast<Function_decl>(decl), def);
}


Function_def&
Parser::on_function_definition(Decl& d, Stmt& s)
{
  Function_def& def = build.make_function_def(s);
  define_function(d, def);
  return def;
}


Deleted_def&
Parser::on_deleted_definition(Decl& d)
{
  // FIXME: This could apply to other kinds of declarations too.
  Deleted_def& def = build.make_deleted_def();
  define_function(d, def);
  return def;
}


Defaulted_def&
Parser::on_defaulted_definition(Decl& d)
{
  // FIXME: This could apply to other kinds of declarations too.
  Defaulted_def& def = build.make_defaulted_def();
  define_function(d, def);
  return def;
}


// -------------------------------------------------------------------------- //
// Namespaces


Namespace_decl&
Parser::on_namespace_declaration(Token, Name&, Decl_list&)
{
  lingo_unimplemented();
}


} // namespace banjo
