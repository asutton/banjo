// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"


namespace banjo
{

namespace fe
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
  // Type& t = prefix_type();
  // if (match_if(tk::ellipsis_tok))
  //   return on_pack_type(t);
  // return t;

  return prefix_type();
}


// Parse a unary type specifier.
//
//    unary-type:
//      unary-type
//      '&' unary-type
//
// NOTE: The parameter passing types (in, out, etc.) do not apply to
// cv-qualified types, but can apply to pointers (presumably). It would
// be nice if we could make the grammar reflect this, but it means
// making lots of weird branches.
Type&
Parser::prefix_type()
{
  switch (lookahead()) {
    case tk::amp_tok: {
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
  if (match_if(tk::const_tok)) {
    Type& t = unary_type();
    return on_const_type(t);
  }
  if (match_if(tk::volatile_tok)) {
    Type& t = unary_type();
    return on_volatile_type(t);
  }
  if (match_if(tk::star_tok)) {
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
    if (match_if(tk::lbracket_tok))
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
 // // match(lbracket_tok);
 // if (match_if(tk::rbracket_tok))
 //    return on_slice_type(t);

 Expr& e = expression();
 match(tk::rbracket_tok);
 return on_array_type(t, e);
}

 
Type&
Parser::tuple_type()
{  
  Type_list types;
  match(tk::lbrace_tok);
  if (lookahead() != tk::rbrace_tok)
    types = type_list();
  match(tk::rbrace_tok);
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
    case tk::void_tok:
      return on_void_type(accept());
    
    case tk::bool_tok:
      return on_bool_type(accept());
    
    case tk::int_tok:
      return on_int_type(accept());
    
    case tk::byte_tok:
      return on_byte_type(accept());

    // TODO: Implement me.
    case tk::char_tok:
      lingo_unimplemented("char type");
    
    case tk::float_tok:
      lingo_unimplemented("float type");
    
    case tk::auto_tok:
      lingo_unimplemented("auto type");
    
    case tk::decltype_tok:
      return decltype_type();

    case tk::lparen_tok: {
      // FIXME: We shouldn't need a tentative parse for this.
      if (Type* t = match_if(&Parser::function_type))
        return *t;
      return grouped_type();
    }
    
    case tk::lbrace_tok:
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
  match(tk::lparen_tok);
  if (lookahead() != tk::rparen_tok)
    types = type_list();
  match(tk::rparen_tok);
  match(tk::arrow_tok);
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
  while (match_if(tk::comma_tok))
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
  match(tk::lparen_tok);
  Type& t = unary_type();
  match(tk::rparen_tok);
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
  Token tok = require(tk::decltype_tok);
  match(tk::lparen_tok);
  Expr& expr = expression();
  match(tk::rparen_tok);
  return on_decltype_type(tok, expr);
}


} // namespace fe

} // namespace banjo
