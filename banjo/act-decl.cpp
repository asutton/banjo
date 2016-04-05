// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-decl.hpp"
#include "ast-def.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{

// If we have an unassigned list of template parameters, then they
// provide the context for this declaration. Transform the declaration
// into a template. Clear the parameters so they aren't "re-used" for
// a nested declaration.
//
// If we are not parsing a template, no changes are made to d.
Decl&
Parser::templatize_declaration(Decl& d)
{
  if (state.template_parms) {
    // Build the template.
    Template_decl& tmp = build.make_template(*state.template_parms, d);
    state.template_parms = nullptr;

    // Apply constraints, if any.
    if (state.template_cons) {
      tmp.cons = state.template_cons;
      state.template_cons = nullptr;
    }
    return tmp;
  }
  return d;
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
Parser::on_variable_declaration(Name& n, Type& t)
{
  return cxt.make_variable(n, t);

  // Decl& d1 = build.make_variable(n, t);
  // Decl& d2 = templatize_declaration(d1);
  // declare(cxt, current_scope(), d2);
  // return d2;
}


Decl&
Parser::on_variable_declaration(Name& n, Type& t, Expr& e)
{
  return cxt.make_variable(n, t, e);

  // Decl& d1 = build.make_variable(n, t, e);
  // Decl& d2 = templatize_declaration(d1);
  // declare(cxt, current_scope(), d2);
  // return d2;
}


// -------------------------------------------------------------------------- //
// Functions

// A helper function that hides the ugliness of assinging the
// function definition.
static inline Def&
define_function(Decl& decl, Def& def)
{
  Decl* d = &decl.parameterized_declaration();
  if (Function_decl* f = as<Function_decl>(d))
    return *(f->def = &def);
  lingo_unhandled(*d);
}


// Define a function, class, enum, or entity.
static inline Def&
define_entity(Decl& decl, Def& def)
{
  Decl* d = &decl.parameterized_declaration();
  if (Function_decl* f = as<Function_decl>(d))
    return *(f->def = &def);
  if (Class_decl* c = as<Class_decl>(d))
    return *(c->def = &def);
  lingo_unhandled(*d);
}


Decl&
Parser::on_function_declaration(Name& n, Decl_list& p, Type& t, Stmt& s)
{
  return cxt.make_function(n, p, t, s);

  // Decl& d1 = build.make_function(n, ps, t);
  // Decl& d2 = templatize_declaration(d1);
  // declare(cxt, current_scope(), d2);
  // return d2;
}


Object_parm&
Parser::on_function_parameter(Name& n, Type& t)
{
  Object_parm& parm = build.make_object_parm(n, t);

  // FIXME: Don't declare these yet. Parameters are elaborated
  // as part of the function type. Right?
  // declare(cxt, current_scope(), parm);

  return parm;
}


Def&
Parser::on_function_definition(Decl& d, Stmt& s)
{
  Def& def = build.make_function_definition(s);
  return define_function(d, def);
}


Def&
Parser::on_deleted_definition(Decl& d)
{
  Def& def = build.make_deleted_definition();
  return define_entity(d, def);
}


Def&
Parser::on_defaulted_definition(Decl& d)
{
  Def& def = build.make_defaulted_definition();
  return define_function(d, def);
}


// -------------------------------------------------------------------------- //
// Classes

// FIXME: Differentiate between classes and structures.
Decl&
Parser::on_class_declaration(Token tok, Name& n)
{
  Decl& d1 = build.make_class(n);
  Decl& d2 = templatize_declaration(d1);
  declare(cxt, current_scope(), d2);
  return d2;
}


// FIXME: Analyze the class body and nominate special
// constructors, identify class properties, etc.
Def&
Parser::on_class_definition(Decl& d, Decl_list& ds)
{
  Def& def = build.make_class_definition(ds);
  return define_entity(d, def);
}


// -------------------------------------------------------------------------- //
// Namespaces


Decl&
Parser::on_namespace_declaration(Token, Name&, Decl_list&)
{
  lingo_unimplemented("on namespace-decl");
}


// -------------------------------------------------------------------------- //
// Templates

Type_parm&
Parser::on_type_template_parameter(Name& n)
{
  Type_parm& parm = build.make_type_parameter(n);
  declare(cxt, current_scope(), parm);
  return parm;
}


Type_parm&
Parser::on_type_template_parameter(Name& n, Type& t)
{
  Type_parm& parm = build.make_type_parameter(n, t);
  declare(cxt, current_scope(), parm);
  return parm;
}


// -------------------------------------------------------------------------- //
// Concepts

static inline void
define_concept(Decl& decl, Def& def)
{
  Concept_decl& con = cast<Concept_decl>(decl);
  con.def = &def;
}


Decl&
Parser::on_concept_declaration(Token, Name& n, Decl_list& ps)
{
  Decl& decl = build.make_concept(n, ps);
  declare(cxt, current_scope(), decl);
  return decl;
}


Def&
Parser::on_concept_definition(Decl& decl, Expr& e)
{
  Def& def = build.make_expression_definition(e);
  define_concept(decl, def);
  return def;
}


Def&
Parser::on_concept_definition(Decl& decl, Req_list& ds)
{
  Def& def = build.make_concept_definition(ds);
  define_concept(decl, def);
  return def;
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
