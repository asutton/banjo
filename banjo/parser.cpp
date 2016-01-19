// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{

namespace
{

// Returns a spelling for the current token. If the token
// stream is at the end of input, then the spelling will
// reflect that state.
String const&
token_spelling(Token_stream& ts)
{
  static String end = "end-of-input";
  if (ts.eof())
    return end;
  else
    return ts.peek().spelling();
}

} // namespace


// Returns the current token.
Token
Parser::peek() const
{
  return tokens.peek();
}


// Returns the first token of lookahead.
Token_kind
Parser::lookahead() const
{
  return Token_kind(peek().kind());
}


// Returns the nth token of lookahead.
Token_kind
Parser::lookahead(int n) const
{
  return Token_kind(tokens.peek(n).kind());
}


Token
Parser::match(Token_kind k)
{
  if (lookahead() == k)
    return tokens.get();
  String msg = format("expected '{}' but got '{}'",
                      get_spelling(k),
                      token_spelling(tokens));
  error(tokens.location(), msg);
  throw Syntax_error("match");
}


// If the current token matches k, return the token
// and advance the stream. Otherwise, return an
// invalid token.
//
// Note that invalid tokens evaluate to false.
Token
Parser::match_if(Token_kind k)
{
  if (lookahead() == k)
    return tokens.get();
  else
    return Token();
}


// Require a token of the given kind. Behavior is
// udefined if the token does not match.
Token
Parser::require(Token_kind k)
{
  assert(lookahead() == k);
  return tokens.get();
}


// Returns the current token and advances the
// underlying token stream.
Token
Parser::accept()
{
  return tokens.get();
}


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
Name*
Parser::unqualified_id()
{
  if (lookahead() == tilde_tok)
    return destructor_id();

  // Use a trial parser to determine if we're looking
  // at a template-id instead of a plain identifier.
  //
  // FIXME: This doesn't really need to be a trial parse.
  if (Name* n = match_if(&Parser::template_id))
    return n;

  Token tok = match(identifier_tok);
  return on_simple_id(tok);
}


// Parse a destructor-id. A destuctor-id naems the destructor of
// a class or a decltype-type that refers to a class.
//
//    destructor-id:
//      '~' class-name
//      '~' decltype-type
Name*
Parser::destructor_id()
{
  Token tok = match(tilde_tok);
  if (lookahead() == decltype_tok) {
    Type* t = decltype_type();
    return on_destructor_id(tok, t);
  } else {
    Type* t = class_name();
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
Name*
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
Name*
Parser::simple_template_id()
{
  Token tok = match_if(template_tok);

  Decl* temp;
  {
    Assume_template a(*this, (bool)tok);
    temp = template_name();
  }

  // FIXME: Accept a '>>' if we're in a nested template argument
  // list. Replace the '>>' with a '>' so that the outer list
  // will match.
  List* args;
  match(lt_tok);
  if (lookahead() == gt_tok)
    args = on_template_argument_list();
  else
    args = template_argument_list();
  match(gt_tok);
  return on_template_id(tok, temp, args);
}


// Parse a template argument list.
//
//    template-argument-list:
//      template-argument
//      template-argument-list ',' template-argument
//
// FIXME: Implement me.
List*
Parser::template_argument_list()
{
  return nullptr;
}


// Parse a leading-name-specifier. This defines the set of
// terms that can be nested within a nested-name-specifier.
//
//    nested-name-specifier:
//      '::'
//      namespace-name '::'
//      type-name '::'
//      decltype-type '::'
Name*
Parser::leading_name_specifier()
{
  Name* nns;
  if (lookahead() == colon_colon_tok)
    nns = on_nested_name_specifier(peek());
  else if (lookahead() == decltype_tok)
    nns = on_nested_name_specifier(decltype_type());
  else if (Decl* n = match_if(&Parser::namespace_name))
    nns = on_nested_name_specifier(n);
  else if (Type* t = match_if(&Parser::type_name))
    nns = on_nested_name_specifier(t);
  else
    throw Syntax_error("expected leading-name-specifier");
  match(colon_colon_tok);
  return nns;
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
Name*
Parser::nested_name_specifier()
{
  Name* nns = leading_name_specifier();
  while (true) {
    if (Token id = match_if(identifier_tok))
      nns = on_nested_name_specifier(nns, id);
    else if (Name* n = match_if(&Parser::simple_template_id))
      nns = on_nested_name_specifier(nns, n);
    else
      break;
  }
  return nns;
}


// Parse an optional nested name specifier.
Name*
Parser::nested_name_specifier_opt()
{
  return match_if(&Parser::nested_name_specifier);
}


// Parse a qualified id.
//
//    qualified-id ::= nested-name-specifier unqualified-id
Name*
Parser::qualified_id()
{
  // TODO: The parsing of the the nested name specifier needs
  // to adjust the scope so that the resolution of the qualified
  // part succeeds (if it needed).
  Name* nns = nested_name_specifier();
  Name* id = unqualified_id();
  return on_qualified_id(nns, id);
}


// Parse a name that refers to a declaration of any kind.
//
//    id:
//      unqualified-id
//      qualified-id
Name*
Parser::id()
{
  Trial_parser p(*this);
  try {
    return qualified_id();
  } catch (Syntax_error&) {
    p.failed();
  }
  return unqualified_id();
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
Type*
Parser::class_name()
{
  if (Name* n = match_if(&Parser::simple_template_id))
    return on_class_name(n);
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
Type*
Parser::union_name()
{
  if (Name* n = match_if(&Parser::simple_template_id))
    return on_union_name(n);
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
Type*
Parser::enum_name()
{
  if (Name* n = match_if(&Parser::simple_template_id))
    return on_enum_name(n);
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
Type*
Parser::type_name()
{
  if (Type* id = match_if(&Parser::class_name))
    return id;
  if (Type* id = match_if(&Parser::union_name))
    return id;
  if (Type* id = match_if(&Parser::enum_name))
    return id;
  if (Type* id = match_if(&Parser::type_alias))
    return id;
  throw Syntax_error("expected type-name");
}


// Parse a type-alias.
//
//    type-alias:
//      identifier
//      template-id
Type*
Parser::type_alias()
{
  if (Name* n = match_if(&Parser::template_id))
    return on_type_alias(n);
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
Decl*
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
Decl*
Parser::namespace_alias()
{
  if (Name* n = match_if(&Parser::template_id))
    return on_namespace_alias(n);
  Token id = match(identifier_tok);
  return on_namespace_alias(id);
}


// Parse a template-name. A template-name is an identifier
// that refers to a template declaration.
//
//    template-name:
//      identifier
Decl*
Parser::template_name()
{
  Token id = match(identifier_tok);
  return on_template_name(id);
}


// -------------------------------------------------------------------------- //
// Types

// Parse a decltype type.
//
//    decltype-type:
//      decltype '(' expression ')'
//
// TODO: Support decltype(auto).
Type*
Parser::decltype_type()
{
  Token tok = require(decltype_tok);
  match(lparen_tok);
  Expr* expr = expression();
  match(rparen_tok);
  return on_decltype_type(tok, expr);
}


// Parse a return-type.
//
//      return-type:
//        '->' type
Type*
Parser::return_type()
{
  require(arrow_tok);
  return type();
}


// Parse a type list.
//
//    type-list:
//      type
//      type-list ',' type
List*
Parser::type_list()
{
  List* types = on_type_list();
  types->push_back(type());
  while (match_if(comma_tok))
    types->push_back(type());
  return types;
}


// Parse a function type.
//
//    function-type:
//      '(' [type-list] ')' return-type
Type*
Parser::function_type()
{
  List* types;
  match(lparen_tok);
  if (lookahead() == rparen_tok)
    types = on_type_list();
  else
    types = type_list();
  match(rparen_tok);
  Type* ret = return_type();
  return on_function_type(types, ret);
}


// Parse a grouped type.
//
//    grouped-type:
//      '(' type ')'
Type*
Parser::grouped_type()
{
  match(lparen_tok);
  Type* t = type();
  match(rparen_tok);
  return t;
}


// Parse a simple type.
//
//    simple-type:
//      'void'
//      'char'  | 'char16' | 'char32'
//      'short' | 'ushort'
//      'int'   | 'uint'
//      'long'  | 'ulong'
//      'int8'  | 'int16'  | 'int32'  | 'int64'  | ...
//      'uint8' | 'uint16' | 'uint32' | 'uint64' | ...
//      'float'
//      'double'
//      'float16' | 'float32' | 'float64'
//      'auto'
//      type-name
//      decltype-type
//      function-type
//      grouped-type
//
// TODO: Add the other specifiers.
Type*
Parser::simple_type()
{
  switch (lookahead()) {
    case void_tok:
    case char_tok:
    case int_tok:
    case uint_tok:
    case float_tok:
    case double_tok:
    case auto_tok:
      // FIXME: Match on each type.
      return on_simple_type(accept());
    case decltype_tok:
      return decltype_type();
    case lparen_tok: {
      if (Type* t = match_if(&Parser::function_type))
        return t;
      return grouped_type();
    }
    default:
      return type_name();
  }
}


// Parse a postfix-type.
//
//    postfix-type:
//      simple-type
//      postfix-type 'const'
//      postfix-type 'volatile'
//      postfix-type '*'
//      postfix-type '[' expression ']'
//
// TODO: Implement array types.
Type*
Parser::postfix_type()
{
  Type* t = simple_type();
  while (true) {
    if (Token tok = match_if(star_tok))
      t = on_pointer_type(tok, t);
    else if (Token tok = match_if(const_tok))
      t = on_const_type(tok, t);
    else if (Token tok = match_if(volatile_tok))
      t = on_volatile_type(tok, t);
    else if (Token tok = match_if(amp_tok))
      t = on_reference_type(tok, t);
    else
      break;
  }
  return t;
}


// Parse a sequence type. This is a type type followed
// by empty brackets.
//
//    sequence-type:
//      postfix-type ['[' ']']
Type*
Parser::sequence_type()
{
  Type* t = postfix_type();
  if (match_if(lbracket_tok)) {
    match(rbracket_tok());
    return on_sequence_type(t);
  }
}


// Parse a reference type.
//
//    reference-type:
//      sequence-type ['&']
//      sequence-type ['&&']
//
// FIXME: Implement rvalue and forwarding references.
Type*
Parser::reference_type()
{
  Type* t = sequence_type();
  if (match_if(amp_tok))
    return on_reference_type(t);
  return t;
}


// Parse a type.
//
//    type:
//      reference-type
//
// TODO: Add general support for packed types (e.g.: int...). This
// has lower precedence than reference types.
Type*
Parser::type()
{
  return reference_type;
}


// -------------------------------------------------------------------------- //
// Expressions
//
// TODO: Be sure that function arguments produce initializers.

// Parse an id-expression.
//
//    id-expression:
//      id
Expr*
Parser::id_expression()
{
  Name* n = id();
  return on_id_expression(n);
}


// Parse a paren-enclosed expression.
//
//    grouped-expr ::= '(' expr ')'
Expr*
Parser::grouped_expression()
{
  require(lparen_tok);
  Expr* e = expression();
  match(rparen_tok);
  return e;
}


// Parse a primary expression.
//
//    primary-expression:
//      literal
//      '(' expression ')'
Expr*
Parser::primary_expression()
{
  if (Token tok = match_if(integer_tok))
    return on_integer_literal(tok);
  if (lookahead() == lparen_tok)
    return grouped_expression();

  error(tokens.location(), "expected primary-expression");
  throw Syntax_error("primary");
}


Expr*
Parser::postfix_expression()
{
  return nullptr;
}

#if 0
// Parse a unary epxression. A unary expressions is one
// that begins with an operator and is followed by a
// unary expression.
//
//    unary-expression ::=
//        primary-expression
//      | unary-operator unary-expression.
Expr const*
Parser::unary_expression()
{
  if (Token tok = match_if(plus_tok))
    return on_unary(tok, unary());
  if (Token tok = match_if(minus_tok))
    return on_unary(tok, unary());
  return primary();
}


// Parse a multiplicative expression.
//
//    multiplicative-expression:
//      unary-expression
//      multiplicative-expression multiplicative-operator unary-expression
Expr const*
Parser::multiplicative_expression()
{
  Expr const* e = unary();
  while (true) {
    if (Token tok = match_if(star_tok))
      e = on_binary(tok, e, unary());
    else if (Token tok = match_if(slash_tok))
      e = on_binary(tok, e, unary());
    else if (Token tok = match_if(percent_tok))
      e = on_binary(tok, e, unary());
    else
      break;
  }
  return e;
}


// Parse an additive expression.
//
//    additive-expression ::=
//        multiplicative-expression
//      | additive-expression additive-operator multiplicative-expression
Expr const*
Parser::additive_expression()
{
  Expr const* e = multiplicative();
  while (true) {
    if (Token tok = match_if(plus_tok))
      e = on_binary(tok, e, multiplicative());
    else if (Token tok = match_if(minus_tok))
      e = on_binary(tok, e, multiplicative());
    else
      break;
  }
  return e;
}


// Parse a binary expression. This is the top-level entry point
// for the binary precedence parser.
inline Expr const*
Parser::binary_expression()
{
  return additive();
}
#endif

// Parse an expression.
Expr*
Parser::expression()
{
  return nullptr;
}


// -------------------------------------------------------------------------- //
// Declarations


// Returns true if the current token kind indicates
// the start of a declaration.
inline bool
starts_declaration(Parser& p)
{
  switch (p.lookahead()) {
    case var_tok:
    case def_tok:
    case struct_tok:
    case class_tok:
    case union_tok:
    case enum_tok:
    case namespace_tok:
    case template_tok:
      return true;
    default:
      return false;
  }
}


// Parse a declarator.
//
//    declarator:
//      id
//
// A declarator is the declared name of an entity. If a declarator
// is a qualified-id, then the qualification explicitly gives the
// context in which the declaration was previously declared.
//
// TODO: If the id contains a nested name specifier, then
// we probably need to validate that that id is defined
// within that scope (always?).
Name*
Parser::declarator()
{
  Name* n = id();
  return on_declarator(n);
}


// Initializers

// Parse a value initializer.
//
//    equal-initializer:
//      '=' expression
Init*
Parser::equal_initializer()
{
  require(eq_tok);
  Expr* expr = expression();
  return on_equal_initializer(expr);
}


// Parse a direct initializer.
//
//    paren-initializer:
//      '(' expression-list ')'
Init*
Parser::paren_initializer()
{
  throw std::runtime_error("not implemented");
}


// Parse a brace initializer.
//
//    brace-initializer:
//      '{' expression-list '}'
Init*
Parser::brace_initializer()
{
  throw std::runtime_error("not implemented");
}


// Parse a variable initializer.
//
//    initializer:
//      equal-initializer
//      paren-initializer
//      brace-initializer
//
// Note that C++ refers to the equal-initializer form of initialization
// as copy-initialization. This term also applies to object initialization
// that occurs in argument passing, initialiation of condition variables,
// exception construction and catching and aggregate member initialization.
// Copy initialization may invoke a move.
//
// The paren- and brace-initializer foms are called direct initialization.
// This term also applies to object initialization in new expressions,
// static casts, functional conversion, and member initializers.
Init*
Parser::initializer()
{
  if (lookahead() == eq_tok)
    return value_initializer();
  else if (lookahead() == lparen_tok)
    return paren_initializer();
  else if (lookahead() == lbrace_tok)
    return brace_initializer();
  throw Syntax_error("expected initializer");
}


// Parse a variable declaration.
//
//    variable-declaration:
//      'var' type declarator [initializer] ';'
Decl*
Parser::variable_declaration()
{
  Token tok = require(var_tok);
  Type* t = type();
  Name* n = declarator();

  Init* i;
  if (lookahead() == semicolon_tok)
    i = on_default_initializer();
  else
    i = initializer();
  match(semicolon_tok);

  return on_variable_declaration(tok, n, t, i);
}


// Functions

// Parse a parameter declaration.
//
//    parameter-declaration:
//      type [identifier] [value-initializer]
Decl*
Parser::parameter_declaration()
{
  Type* t = type();

  Name* n = nullptr;
  if (Token id = match_if(identifier_tok))
    n = on_simple_id(id);

  Init* i = nullptr;
  if (lookahead() == eq_tok) {
    i = value_initializer();
  }

  return on_parameter_declaration(n, t, i);
}

#if 0
// Parse a parameter list.
//
//    parameter-list:
//      ...
//      parameter-declaration
//      parameter-list ',' parameter-declaration
//
// Only the last parameter in the list shall be an ellipsis.
List*
Parser::parameter_list()
{
  Decl_list ps;
  while (true) {
    // FIXME: Handle ellispes... Is an ellipsis a parameter
    // or not?
    Decl* p = parameter_declaration();
    ps.push_back(p);
    if (match_if(comma_tok))
      continue;
    else
      break;
  }
  return on_parameter_list(ps);
}


// Parse an optional parameter list.
Node*
Parser::parameter_list
{
  match(lparen_tok);
  if (match_if(rparen_tok))
    return on_parameter_list();
  Tree* p = parameter_list();
  match(rparen_tok);
  return p;
}


// Parse a function body.
//
//    function-body:
//      compound-statement
//      '=' expression ';'
//      '=' 'default' ';'
//      '=' 'delete' ';'
Node*
Parser::function_body()
{
  throw std::runtime_error("not implemeneted");
}


// Parse a function definition.
//
//    function-declaration:
//      'def' declarator '(' [parameter-list] ')' return-type function-definition
//
//    return-type:
//      '->' type
//
// TODO: Handle variadics better. Maybe have an optional
// ', ...' at the end of the parameter list.
//
// TODO: Allow declared but not defeined functios?
//
// TODO: Allow for additional specifiers after the ')' and before
// the '->'. This would support qualifiers on member function
// declarations.
Decl*
Parser::function_declaration()
{
  Token tok = require(def_tok);
  Tree* n = declarator();
  Tree* p = parameter_list_opt();
  Type* t = type();
  Tree* def = function_definition();
  return on_function_declaration(tok, name, parms, type, def);
}
#endif

// FIXME: This is garbage.
Decl*
Parser::function_declaration()
{
  return nullptr;
}


// Parse a namespace declaration.
//
//    namespace-declaration:
//      'namespace' [declarator] '{' declaration-seq '}'
Decl*
Parser::namespace_declaration()
{
  Token tok = require(namespace_tok);
  Name* n = declarator();
  List* d = declaration_seq();
  return on_namespace_declaration(tok, n, d);
}


// Parse a typename declaration.
//
//    typename-declaration:
//      'typename' identifier ['=' type]
Decl*
Parser::typename_declaration()
{
  return nullptr;
}


// Parse a template parameter declaration:
//
//    template-parameter:
//      typename-declaration
//      constant-declaration
//      template-declaration
Decl*
Parser::template_parameter()
{
  return nullptr;
}


// Parse a template parameter list.
//
//    template-parameter-list:
//      template-parameter
//      template-parameter-list template parameter
Decl_seq
Parser::template_parameter_list()
{
  return {};
}


// Parse a template declaration.
//
//    tempate-declaration:
//      'template' '<' template-parameter-list '>' declaration
Decl*
Parser::template_declaration()
{
    return nullptr;
}


Decl*
Parser::declaration()
{
  switch (lookahead()) {
    case var_tok:
      return variable_declaration();
    case def_tok:
      return function_declaration();
    // case struct_tok:
    // case class_tok:
    //   return class_declaration();
    // case enum_tok:
    //   return enum_declaration();
    case namespace_tok:
      return namespace_declaration();
    // case template_tok:
    //   return template_declaration();
    default:
      break;
  }
  throw Syntax_error("invalid declaration");
}


// Parse a declration sequence.
//
//    declaration-seq:
//      declaration
//      declaration-seq declaration
//
Decl_list
Parser::declaration_seq()
{
  // FIXME: Catch declaration errors and continue parsing.
  declaration();
  while (starts_declaration(*this))
    declaration();

  return {};
}


// Parse a translation unit.
//
//    translation-unit:
//      [declaration-seq]
Term*
Parser::translation_unit()
{
  start_translation_unit();
  if (peek())
    declaration_seq();
  return finish_translation_unit();
}


Term*
Parser::operator()()
{
  return translation_unit();
}


} // namespace banjo
