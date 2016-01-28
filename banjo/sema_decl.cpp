// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "scope.hpp"
#include "overload.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// If we have an unassigned list of template parameters, then they
// provide the context for this declaration. Transform the declaration
// into a template. Clear the parameters so they aren't "re-used" for
// a nested declaration.
//
Decl&
Parser::templatize_declaration(Decl& d)
{
  if (state.template_parms) {
    Decl& tmp = build.make_template(*state.template_parms, d);
    state.template_parms = nullptr;
    return tmp;
  }
  return d;
}


// -------------------------------------------------------------------------- //
// Declaration
//
// TODO: Move this into a separate semantic module?

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
Parser::declare(Scope& scope, Decl& d)
{
  // Declare the ajusted declaration.
  if (Overload_set* ovl = scope.lookup(d.declared_name())) {
    return banjo::declare(*ovl, d);
  } else {
    scope.bind(d);
    return nullptr;
  }
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


Decl&
Parser::on_variable_declaration(Token, Name& n, Type& t)
{
  Decl& d1 = build.make_variable(n, t);
  Decl& d2 = templatize_declaration(d1);
  declare(current_scope(), d2);
  return d2;
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


// -------------------------------------------------------------------------- //
// Templates

Type_parm&
Parser::on_type_template_parameter(Name& n)
{
  Type_parm& parm = build.make_type_parameter(n);
  declare(current_scope(), parm);
  return parm;
}


Type_parm&
Parser::on_type_template_parameter(Name& n, Type& t)
{
  Type_parm& parm = build.make_type_parameter(n, t);
  declare(current_scope(), parm);
  return parm;
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
