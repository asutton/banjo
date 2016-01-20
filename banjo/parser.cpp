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
// Scopes


// Enter the given scope. Unless `s` is the scope of the global
// namespace, `s` must be linked through its enclosing scopes
// to the global namespace.
//
// Use Parser::Scope_sentinel to enter a new scope.
void
Parser::enter_scope(Scope& s)
{
  state.scope = &s;
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
Type_list
Parser::type_list()
{
  Type_list types;
  types.push_back(type());
  while (match_if(comma_tok))
    types.push_back(type());
  return types;
}


// Parse a function type.
//
//    function-type:
//      '(' [type-list] ')' return-type
Type&
Parser::function_type()
{
  Type_list types;
  match(lparen_tok);
  if (lookahead() != rparen_tok)
    types = type_list();
  match(rparen_tok);
  Type& ret = return_type();
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
      return on_void_type(accept());
    case bool_tok:
      return on_bool_type(accept());
    case int_tok:
      return on_int_type(accept());
    // TODO: Implement me.
    case char_tok:
    case uint_tok:
    case float_tok:
    case double_tok:
    case auto_tok:
      // FIXME: Match on each type.
      lingo_unimplemented();
    case decltype_tok:
      return decltype_type();
    case lparen_tok: {
      if (Type* t = match_if(&Parser::function_type))
        return *t;
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
  Name& n = id();
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
Name&
Parser::declarator()
{
  Name& n = id();
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
  Name& n = declarator();

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

  Name& n = nullptr;
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
  Name& n = declarator();
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
Term&
Parser::translation_unit()
{
  Scope_sentinel scope(cxt.global_namespace());
  if (peek())
    declaration_seq();
}


Term&
Parser::operator()()
{
  return translation_unit();
}


} // namespace banjo
