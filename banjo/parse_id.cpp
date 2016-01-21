// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>

namespace banjo
{


// -------------------------------------------------------------------------- //
// Ids, names, and aliases
//
// An "id" is a (possibly qualified) declared identifier of
// any kind. Ids are typically prefixed by their syntactic
// form (e.g., operator-id or template-id).
//
// A "name" is an id of a specific kind (e.g., class-name or
// template-name). Note that "names" are always unqualified.
// An "alias" is a "name" that was declared as an alias for a
// declaration of some kind (type-alias e.g. namespace-alias).
//
// Note that we *could* define parsers for variable names,
// function names, and overload names, but the rules that
// govern their use are purely semantic (type checking) and
// do not affect the current parse. For example, for a variable
// `v`, the expression `v(x)`` is valid only if `v` has class
// type and provides a function call operator.


// Parse a name that refers to a declaration of any kind.
//
//    id:
//      unqualified-id
//      qualified-id
Name&
Parser::id()
{
  if (Name* n = match_if(&Parser::qualified_id))
    return *n;
  return unqualified_id();
}


// Parse an unqualified-id.
//
//    unqualified-id:
//      identifier
//      operator-id
//      conversion-id
//      literal-id
//      destructor-id
//      template-id
//
// TODO: Support operator, conversion, and literal ids. Note that
// all of those begin with 'operator', but can be syntactically
// differentiated by lookahead(1). Operator-ids are followed by
// an operator symbol, literal id's are followed by an empty string
// literal, and conversion id's are the rest.
//
// FIXME: Finish implementing me. Note that the interpretation of
// an identifier depends on both lookup and context.
Name&
Parser::unqualified_id()
{
  if (lookahead() == tilde_tok)
    return destructor_id();

  // Use a trial parser to determine if we're looking
  // at a template-id instead of a plain identifier.
  //
  // FIXME: This doesn't really need to be a trial parse.
  if (Name* n = match_if(&Parser::template_id))
    return *n;

  Token tok = match(identifier_tok);
  return on_simple_id(tok);
}


// Parse a destructor-id. A destuctor-id naems the destructor of
// a class or a decltype-type that refers to a class.
//
//    destructor-id:
//      '~' class-name
//      '~' decltype-type
Name&
Parser::destructor_id()
{
  Token tok = match(tilde_tok);
  if (lookahead() == decltype_tok) {
    Type& t = decltype_type();
    return on_destructor_id(tok, t);
  } else {
    Type& t = class_name();
    return on_destructor_id(tok, t);
  }
}


// Parse a template id.
//
//    template-id:
//      simple-template-id
//      operator-id template-argument-clause
//      literal-id template-argument-clause
//
// TODO: Handle the operator and literal cases.
Name&
Parser::template_id()
{
  return simple_template_id();
}


// Parse a simple-template-id.
//
//    simple-template-id:
//      ['template'] template-name '< [template-argument-list] '>'
//
// TODO: Consider merging this with template-id and using
// semantic rules to reject certain valid parses.
Name&
Parser::simple_template_id()
{
  Token tok = match_if(template_tok);

  // FIXME: This is kind of gross.
  Decl* temp;
  {
    Assume_template a(*this, (bool)tok);
    temp = &template_name();
  }

  // FIXME: Accept a '>>' if we're in a nested template argument
  // list. Replace the '>>' with a '>' so that the outer list
  // will match.
  Term_list args;
  match(lt_tok);
  if (lookahead() != gt_tok)
    args = template_argument_list();
  match(gt_tok);
  return on_template_id(tok, *temp, args);
}


// Parse a template argument list.
//
//    template-argument-list:
//      template-argument
//      template-argument-list ',' template-argument
//
// FIXME: Implement me.
Term_list
Parser::template_argument_list()
{
  return {};
}


// Parse a leading-name-specifier. This defines the set of
// terms that can be nested within a nested-name-specifier.
//
//    nested-name-specifier:
//      '::'
//      namespace-name '::'
//      type-name '::'
//      decltype-type '::'
Decl&
Parser::leading_name_specifier()
{
  Decl* scope;
  if (lookahead() == colon_colon_tok)
    scope = &on_nested_name_specifier();
  else if (lookahead() == decltype_tok)
    scope = &on_nested_name_specifier(decltype_type());
  else if (Decl* n = match_if(&Parser::namespace_name))
    scope = &on_nested_name_specifier(*n);
  else if (Type* t = match_if(&Parser::type_name))
    scope = &on_nested_name_specifier(*t);
  else
    throw Syntax_error("expected leading-name-specifier");
  match(colon_colon_tok);
  return *scope;
}


// Parse a nested-name specifier.
//
//    nested-name-specifier:
//      leading-name-specifier
//      nested-name-specifier identifier '::'
//      nested-name-specifier simple-template-id '::'
//
// Here, the identifier and simple-template-id must be either
// namespaces or types. In dependent contexts, those could also
// be members of an unknown specialization.
Decl&
Parser::nested_name_specifier()
{
  Decl* scope = &leading_name_specifier();
  while (true) {
    if (Token id = match_if(identifier_tok))
      scope = &on_nested_name_specifier(*scope, id);
    else if (Name* id = match_if(&Parser::simple_template_id))
      scope = &on_nested_name_specifier(*scope, *id);
    else
      break;
  }
  return *scope;
}


// Parse an optional nested name specifier.
//
// FIXME: Are we using this anywhere?
Decl*
Parser::nested_name_specifier_opt()
{
  return match_if(&Parser::nested_name_specifier);
}


// Parse a qualified id.
//
//    qualified-id ::= nested-name-specifier unqualified-id
Name&
Parser::qualified_id()
{
  // TODO: The parsing of the the nested name specifier needs
  // to adjust the scope so that the resolution of the qualified
  // part succeeds (if it needed).
  Decl& scope = nested_name_specifier();
  Name& id = unqualified_id();
  return on_qualified_id(scope, id);
}



// -------------------------------------------------------------------------- //
// Resolved names

// Parse a class-name. A class-name is a name that refers to
// a class declaration. A simple-template-id can refer to a class
// template specialization.
//
//    class-name:
//      identifier
//      simple-template-id
//
// In full generality a class-name is just a subset of unqualified
// ids. In particular, it's the subset that cannot match a non-type.
Type&
Parser::class_name()
{
  if (Name* n = match_if(&Parser::simple_template_id))
    return on_class_name(*n);
  Token id = match(identifier_tok);
  return on_class_name(id);
}


// Parse a union-name. A union-name is a name that refers to
// a union declaration. A simple-template-id can refer to a union
// template specialization.
//
//    union-name:
//      identifier
//      simple-template-id
Type&
Parser::union_name()
{
  if (Name* n = match_if(&Parser::simple_template_id))
    return on_union_name(*n);
  Token id = match(identifier_tok);
  return on_union_name(id);
}


// Parse an enum-name. An enum-name is a name that refers to
// a union declaration. An enum-id can refer to a union
// template specialization.
//
//    enum-name:
//      identifier
//      simple-template-id
Type&
Parser::enum_name()
{
  if (Name* n = match_if(&Parser::simple_template_id))
    return on_enum_name(*n);
  Token id = match(identifier_tok);
  return on_enum_name(id);
}


// Parse a type name.
//
//    type-name:
//      class-name
//      union-name
//      enum-name
//      type-alias
//
// TODO: Optimize this by factoring the template-id from
// each of the alternatives and performing semantic analysis
// separately.
Type&
Parser::type_name()
{
  if (Type* id = match_if(&Parser::class_name))
    return *id;
  if (Type* id = match_if(&Parser::union_name))
    return *id;
  if (Type* id = match_if(&Parser::enum_name))
    return *id;
  if (Type* id = match_if(&Parser::type_alias))
    return *id;
  throw Syntax_error("expected type-name");
}


// Parse a type-alias.
//
//    type-alias:
//      identifier
//      template-id
Type&
Parser::type_alias()
{
  if (Name* n = match_if(&Parser::template_id))
    return on_type_alias(*n);
  Token id = match(identifier_tok);
  return on_type_alias(id);
}


// Parse a namespace-name. A namespace-name is an identifier
// that refers to a namespace declaration.
//
//    namespace-name:
//      identifier
//      namespace-alias
//
// TODO: Support
Decl&
Parser::namespace_name()
{
  Token id = match(identifier_tok);
  return on_namespace_name(id);
}


// Parse a namespace-alias.
//
//    namespace-alias:
//      identifier
//      template-id
Decl&
Parser::namespace_alias()
{
  if (Name* n = match_if(&Parser::template_id))
    return on_namespace_alias(*n);
  Token id = match(identifier_tok);
  return on_namespace_alias(id);
}


// Parse a template-name. A template-name is an identifier
// that refers to a template declaration.
//
//    template-name:
//      identifier
Decl&
Parser::template_name()
{
  Token id = match(identifier_tok);
  return on_template_name(id);
}


} // namespace banjo
