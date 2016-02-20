// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "lookup.hpp"
#include "template.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Identifiers

Name&
Parser::on_simple_id(Token tok)
{
  return build.get_id(*tok.symbol());
}


Name&
Parser::on_destructor_id(Token, Type& t)
{
  return build.get_destructor_id(t);
}


Name&
Parser::on_operator_id(Token tok, Operator_kind op)
{
  return build.get_id(op);
}


Name&
Parser::on_conversion_id()
{
  lingo_unimplemented();
}


Name&
Parser::on_literal_id()
{
  lingo_unimplemented();
}


Name&
Parser::on_template_id(Token, Decl& d, Term_list const& a)
{
  return build.get_template_id(cast<Template_decl>(d), a);
}


Name&
Parser::on_concept_id(Decl& d, Term_list const& a)
{
  return build.get_concept_id(cast<Concept_decl>(d), a);
}


Name&
Parser::on_qualified_id(Decl& d, Name& n)
{
  return build.get_qualified_id(d, n);
}


// -------------------------------------------------------------------------- //
// Nested name specifiers


// Returns the declaration for the global nested name specifier.
// This is just the global namespace.
Decl&
Parser::on_nested_name_specifier()
{
  return cxt.global_namespace();
}


// Returns the declaration for a leading identifier that names
// a namespace.
Decl&
Parser::on_nested_name_specifier(Decl&)
{
  lingo_unimplemented();
}


// Returns the type declaration for a leading identifier that
// names a type name.
Decl&
Parser::on_nested_name_specifier(Type& t)
{
  lingo_unimplemented();
}


// Returns the declaration for a nested name specifier of
// an identifier.
Decl&
Parser::on_nested_name_specifier(Decl&, Token)
{
  lingo_unimplemented();
}


// Returns the declaration for a nested name specifier of
// a template id.
Decl&
Parser::on_nested_name_specifier(Decl&, Name&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Resolved names
//
// All of these functions perform lookup on their id and
// check that the resolved declaration matches the specified
// name.

Type&
Parser::on_class_name(Token)
{
  throw Lookup_error("not a class");
}


Type&
Parser::on_class_name(Name&)
{
  throw Lookup_error("not a class");
}


Type&
Parser::on_union_name(Token)
{
  throw Lookup_error("not a union");
}


Type&
Parser::on_union_name(Name&)
{
  throw Lookup_error("not a union");
}


Type&
Parser::on_enum_name(Token)
{
  throw Lookup_error("not an enum");
}


Type&
Parser::on_enum_name(Name&)
{
  throw Lookup_error("not an enum");
}


Type&
Parser::on_type_alias(Token tok)
{
  Simple_id& id = build.get_id(tok);
  Decl& decl = simple_lookup(current_scope(), id);

  if (Type_parm* d = as<Type_parm>(&decl))
    return build.get_typename_type(*d);

  // TODO: Actually support type aliases.

  throw Lookup_error("'{}' does not name a type", id);
}


Type&
Parser::on_type_alias(Name&)
{
  throw Lookup_error("not a type alias");
}


// Returns a type pointer if the declaraiton declares a type.
// Otherwise, returns nullptr.
//
// TODO: Perhpas we should have a base class for all type
// declarations?
inline Type*
get_type_for_decl(Context& cxt, Decl& decl)
{
  Builder build(cxt);

  if (Class_decl* d = as<Class_decl>(&decl))
    return &build.get_class_type(*d);

  if (Union_decl* d = as<Union_decl>(&decl))
    return &build.get_union_type(*d);

  if (Enum_decl* d = as<Enum_decl>(&decl))
    return &build.get_enum_type(*d);

  if (Type_parm* d = as<Type_parm>(&decl))
    return &build.get_typename_type(*d);

  // TODO: Handle type aliases.

  return nullptr;
}


Type&
Parser::on_type_name(Token tok)
{
  Simple_id& id = build.get_id(tok);
  Decl& decl = simple_lookup(current_scope(), id);
  if (Type* type = get_type_for_decl(cxt, decl))
    return *type;
  throw Lookup_error("'{}' does not name a type", id);
}


// Check if the template-id n refers to a type.
Type&
Parser::on_type_name(Name& n)
{
  Template_id& id = cast<Template_id>(n);
  Template_decl& tmp = id.declaration();
  Term_list& args = id.arguments();
  Decl& decl = specialize_template(cxt, tmp, args);
  if (Type* type = get_type_for_decl(cxt, decl))
    return *type;
  throw Lookup_error("not a type name");
}


Decl&
Parser::on_namespace_name(Token id)
{
  throw Lookup_error("not a namespace");
}


Decl&
Parser::on_namespace_name(Name&)
{
  throw Lookup_error("not a namespace");
}


Decl&
Parser::on_namespace_alias(Token)
{
  throw Lookup_error("not a namespace alias");
}


Decl&
Parser::on_namespace_alias(Name&)
{
  throw Lookup_error("not a namespace alias");
}


Decl&
Parser::on_template_name(Token tok)
{
  Simple_id& id = build.get_id(tok);
  Decl& decl = simple_lookup(current_scope(), id);
  if (is<Template_decl>(&decl))
    return decl;
  throw Lookup_error("'{}' does not name a template", id);
}


Decl&
Parser::on_concept_name(Token tok)
{
  Simple_id& id = build.get_id(tok);
  Decl& decl = simple_lookup(current_scope(), id);
  if (is<Concept_decl>(&decl))
    return decl;
  throw Lookup_error("'{}' does not name a concept", id);
}


} // namespace banjo
