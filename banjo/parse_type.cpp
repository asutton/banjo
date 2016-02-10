// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>

namespace banjo
{


// Parse a type.
//
//    type:
//      reference-type
//
// TODO: Add general support for packed types (e.g.: int...). This
// has lower precedence than reference types.
Type&
Parser::type()
{
  return reference_type();
}


// Parse a reference type.
//
//    reference-type:
//      sequence-type ['&']
//      sequence-type ['&&']
//
// FIXME: Implement rvalue and forwarding references.
Type&
Parser::reference_type()
{
  Type& t = sequence_type();
  if (Token tok = match_if(amp_tok))
    return on_reference_type(tok, t);
  return t;
}


// Parse a sequence type. This is a type type followed
// by empty brackets.
//
//    sequence-type:
//      postfix-type ['[' ']']
Type&
Parser::sequence_type()
{
  Type& t = postfix_type();
  if (match_if(lbracket_tok)) {
    match(rbracket_tok);
    return on_sequence_type(t);
  }
  return t;
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
Type&
Parser::postfix_type()
{
  Type* t = &simple_type();
  while (true) {
    if (Token tok = match_if(star_tok))
      t = &on_pointer_type(tok, *t);
    else if (Token tok = match_if(const_tok))
      t = &on_const_type(tok, *t);
    else if (Token tok = match_if(volatile_tok))
      t = &on_volatile_type(tok, *t);
    else if (Token tok = match_if(amp_tok))
      t = &on_reference_type(tok, *t);
    else
      break;
  }
  return *t;
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
Type&
Parser::simple_type()
{
  switch (lookahead()) {
    case void_tok:
      return on_void_type(accept());
    case bool_tok:
      return on_bool_type(accept());
    case int_tok:
      return on_int_type(accept());
    case byte_tok:
      return on_byte_type(accept());
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


// Parse a grouped type.
//
//    grouped-type:
//      '(' type ')'
Type&
Parser::grouped_type()
{
  match(lparen_tok);
  Type& t = type();
  match(rparen_tok);
  return t;
}


// Parse a decltype type.
//
//    decltype-type:
//      decltype '(' expression ')'
//
// TODO: Support decltype(auto).
Type&
Parser::decltype_type()
{
  Token tok = require(decltype_tok);
  match(lparen_tok);
  Expr& expr = expression();
  match(rparen_tok);
  return on_decltype_type(tok, expr);
}


// Parse a return-type.
//
//      return-type:
//        '->' type
Type&
Parser::return_type()
{
  match(arrow_tok);
  return type();
}


} // namespace banjo
