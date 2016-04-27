// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>

namespace banjo
{


// Parse a type.
//
//    type:
//      suffix-type
//
// TODO: Somewhere in this grammar, add general support for packed 
// types (e.g., int...). 
Type&
Parser::type()
{
  return prefix_type();
}


// Parse a suffix type specifier.
//
//    suffix-type:
//      prefix-type ['...']
Type&
Parser::suffix_type()
{
  Type& t = prefix_type();
  if (match_if(ellipsis_tok))
    return on_pack_type(t);
  return t;
}


// Parse a unary type specifier.
//
//    unary-type:
//      unary-type
//      '&' unary-type
//
// FIXME: Rvalue references have not been implemented. Do we actually
// need them, or can we get by with parameter passing types.
//
// NOTE: The parameter passing types (in, out, etc.) do not apply to
// cv-qualified types, but can apply to pointers (presumably). It would
// be nice if we could make the grammar reflect this, but it means
// making lots of weird branches.
Type&
Parser::prefix_type()
{
  switch (lookahead()) {
    case amp_tok: {
      accept();
      Type& t = unary_type();
      return on_reference_type(t);
    }
    default:
      break;
  }
  return unary_type();
}


// Parse a qualified type.
//
//    unary-type:
//      postfix-type
//      '*' unary-type 
//      'const' unary-type
//      'volatile' unary-type
Type&
Parser::unary_type()
{
  if (match_if(const_tok)) {
    Type& t = unary_type();
    return on_const_type(t);
  }
  if (match_if(volatile_tok)) {
    Type& t = unary_type();
    return on_volatile_type(t);
  }
  if (match_if(star_tok)) {
    Type& t = unary_type();
    return on_pointer_type(t);
  }
  return postfix_type();
}


// Parse a postfix-type.
//
//    postfix-type
//      primary-type
//      postfix-type '[]'
//      postfix-type '[' expression ']'
//
Type&
Parser::postfix_type()
{
  Type* t = &primary_type();
  while (true) {
    if (match_if(lbracket_tok))
      t = &array_type(*t);   
    else
      break;
  }
  return *t;
}


// Parse an array of slice type.
Type&
Parser::array_type(Type& t)
{
 //match(lbracket_tok);
 if (match_if(rbracket_tok))
    return on_slice_type(t);
 Expr& e = expression();
 match(rbracket_tok);
 return on_array_type(t, e);
}

 
Type&
Parser::tuple_type()
{  
  Type_list types;
  match(lbrace_tok);
  if (lookahead() != rbrace_tok)
    types = type_list();
  match(rbrace_tok);
  return on_tuple_type(types);
}


// Parse a primary type.
//
//    primary-type:
//      'void'
//      'byte'
//      'bool'
//      'char'
//      'int'
//      'float'
//      'auto'
//      id-type
//      decltype-type
//      function-type
//      '( unary-type )'
//      '{ type-list }'
//
// FIXME: Design a better integer and FP type suite.
Type&
Parser::primary_type()
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
      lingo_unimplemented("char type");
    
    case float_tok:
      lingo_unimplemented("float type");
    
    case auto_tok:
      lingo_unimplemented("auto type");
    
    case decltype_tok:
      return decltype_type();

    case class_tok:
      return on_class_type(accept());

    case lparen_tok: {
      // FIXME: We shouldn't need a tentative parse for this.
      if (Type* t = match_if(&Parser::function_type))
        return *t;
      return grouped_type();
    }
    
    case lbrace_tok:
      return tuple_type();

    default:
      return id_type();
  }
}


// Parse an id-type type.
//
//    id-type:
//      id
Type&
Parser::id_type()
{
  Name& n = id();
  return on_id_type(n);
}


// Parse a function type.
//
//    function-type:
//      '(' [type-list] ')' type
//
// TODO: A function should not be able to return a pack type.
Type&
Parser::function_type()
{
  Type_list types;
  match(lparen_tok);
  if (lookahead() != rparen_tok)
    types = type_list();
  match(rparen_tok);
  match(arrow_tok);
  Type& ret = type();
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
  Type& t = unary_type();
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


} // namespace banjo
