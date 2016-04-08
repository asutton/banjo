// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-req.hpp"

#include <iostream>


namespace banjo
{

// Parse a type requirement.
//
//    type-requirement:
//      typename [nested-name-specifier] simple-template-id ';'
//      typename [nested-name-specifier] type-name ';'
//
// FIXME: The syntax isn't right.
Req&
Parser::type_requirement()
{
  require(typename_tok);

  lingo_unimplemented("parse type-requirement");
}


// Parse a syntactic requirement:
//
//    syntactic-requirement:
//      requires-expression
Req&
Parser::syntactic_requirement()
{
  Expr& e = requires_expression();
  return on_syntactic_requirement(e);
}


// Parse an expression requirement.
//
//    expression-requirement:
//      logical-or-expression ';'
Req&
Parser::expression_requirement()
{
  Expr& e = logical_or_expression();
  match(semicolon_tok);
  return on_expression_requirement(e);
}


// Parse a usage requirement.
//
//    usage-requirement:
//      expression ';'
//      expression ':' type ';'
//      expression '->' type ';'
//
// TODO: Allow a type requirement here.
Req&
Parser::usage_requirement()
{
  Expr& e = expression();
  Req* r;
  if (match_if(colon_tok)) {
    Type& t = type();
    r = &on_basic_requirement(e, t);
  } else if (match_if(arrow_tok)) {
    Type& t = type();
    r = &on_conversion_requirement(e, t);
  } else {
    r = &on_basic_requirement(e);
  }
  match(semicolon_tok);
  return *r;
}


// Parse a sequence of usage requirements.
//
//    usage-seq:
//      usage-requirement
//      useage-seq usage-requirement
Req_list
Parser::usage_seq()
{
  Req_list rs;
  do {
    Req& r = usage_requirement();
    rs.push_back(r);
  } while (next_token_is_not(rbrace_tok));
  return rs;
}


} // namespace banjo
