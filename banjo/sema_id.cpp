// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

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
Parser::on_operator_id()
{
  lingo_unimplemented();
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


// TODO: As we
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
Parser::on_type_alias(Token)
{
  throw Lookup_error("not a type alias");
}


Type&
Parser::on_type_alias(Name&)
{
  throw Lookup_error("not a type alias");
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
Parser::on_template_name(Token)
{
  throw Lookup_error("not a template");
}


} // namespace banjo
