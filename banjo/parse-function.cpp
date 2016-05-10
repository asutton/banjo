// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast-name.hpp"
#include "ast-type.hpp"
#include "ast-decl.hpp"
#include "printer.hpp"

#include <iostream>

namespace banjo
{

// Parse a function definition.
//
//    function-definition:
//      'def' identifier ':' [template-header] parameter-clause expression-statement
//      'def' identifier ':' [template-header] parameter-clause '=' expression-statement
//      'def' identifier ':' [template-header] parameter-clause '->' type function-body
//
//    parameter-spec:
//      '(' [parameter-list] ')'
//
//    function-body:
//      compound-statement
//      '=' delete
//      '=' expression-statement
//
// TODO: Implement deleted functions.
//
// TODO: Allow named return types. That would change the grammar for return
// types to a return declaration.
//
//    def f() -> ret:string { ret = "hello"; }
//
// Note that not all functions can have named returns (e.g., void functions,
// functions returning references, etc.). This also leaks implementation
// details into the interface.
Decl&
Parser::function_declaration()
{
  require(def_tok);
  Name& name = identifier();

  // Optional colon.
  match_if(colon_tok);

  // NOTE: We don't have to enter a scope because we aren't doing
  // lookup in the first pass.
  Decl_list parms = parameter_clause();

  // Match the return type.
  Type* ret;
  if (match_if(arrow_tok))
    ret = &unparsed_return_type();
  else
    ret = &cxt.make_auto_type();

  // Match the function definition.
  if (match_if(eq_tok)) {
    // = expression ;
    Expr& body = unparsed_expression_body();
    match(semicolon_tok);
    return on_function_declaration(name, parms, *ret, body);
  } else {
    // { ... }
    Stmt& body = unparsed_function_body();
    return on_function_declaration(name, parms, *ret, body);
  }
}


// Parse a parameter clause.
//
//    parameter-clause:
//      '(' [parameter-list] ')'
//
Decl_list
Parser::parameter_clause()
{
  Decl_list parms;
  match(lparen_tok);
  if (lookahead() != rparen_tok)
    parms = parameter_list();
  match(rparen_tok);
  return parms;
}


// Parse a parameter list.
//
//    parameter-list:
//      parameter-declaration
//      parameter-list ',' parameter-declaration
Decl_list
Parser::parameter_list()
{
  Decl_list ps;
  while (true) {
    Decl& p = parameter_declaration();
    ps.push_back(p);
    if (match_if(comma_tok))
      continue;
    else
      break;
  }
  return ps;
}


// Parse a parameter declaration.
//
//    parameter-declaration:
//      [parameter-specifier] identifier [':' type] ['=' expression]
//      [parameter-specifier] identifier [':=' expression]
//
// Note that parameter packs and variadics are part of the type system
// and can simply be parsed as part of that type.
Decl&
Parser::parameter_declaration()
{
  // Parse and cache the optional parameter specifier.
  parameter_specifier_seq();

  Name& name = identifier();

  if (match_if(colon_tok)) {
    if (next_token_is(eq_tok))
      lingo_unimplemented("default arguments");

    Type& type = unparsed_parameter_type();

    if (next_token_is(eq_tok))
      lingo_unimplemented("default arguments");

    return on_function_parameter(name, type);
  }

  Type& type = cxt.make_auto_type();

  if (next_token_is(eq_tok))
    lingo_unimplemented("default arguments");

  return on_function_parameter(name, type);
}


Type&
Parser::unparsed_parameter_type()
{
  Token_seq toks;
  Brace_matching_sentinel is_non_nested(*this);
  while (true) {
    if (next_token_is_one_of(comma_tok, rparen_tok) && is_non_nested())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_type(std::move(toks));
}


// Returns an unparsed return type.
Type&
Parser::unparsed_return_type()
{
  Token_seq toks;
  Brace_matching_sentinel is_non_nested(*this);
  while (!is_eof()) {
    if (next_token_is_one_of(lbrace_tok, eq_tok) && is_non_nested())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_type(std::move(toks));
}


// Returns an unparsed expression that defines a function.
Expr&
Parser::unparsed_expression_body()
{
  Token_seq toks;
  Brace_matching_sentinel is_non_nested(*this);
  while (!is_eof()) {
    if (next_token_is(semicolon_tok) && is_non_nested())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_expression(std::move(toks));
}


// Returns an unparsed compound statement that defines a function.
//
// FIXME: This is identical to untyped_type_body, but semantically different.
// There should be no order independence among declarations within a function
// body... Maybe?
Stmt&
Parser::unparsed_function_body()
{
  Token_seq toks;
  toks.push_back(match(lbrace_tok));
  Brace_matching_sentinel is_non_nested(*this);
  while (!is_eof()) {
    if (next_token_is(rbrace_tok) && is_non_nested())
      break;
    toks.push_back(accept());
  }
  toks.push_back(match(rbrace_tok));
  return on_unparsed_statement(std::move(toks));
}


// Parse a function definition.
//
//    function-body:
//      compound-statement
//      '=' 'default' ';'
//      '=' 'delete' ';'
//
// TODO: Allow '= expression' as a viable definition.
Def&
Parser::function_definition(Decl& d)
{
  if (lookahead() == lbrace_tok) {
    Stmt& s = compound_statement();
    return on_function_definition(d, s);
  } else if (match_if(eq_tok)) {
    if (match_if(delete_tok))
      return on_deleted_definition(d);
    if (match_if(default_tok))
      return on_defaulted_definition(d);
  }
  throw Syntax_error("expected function-definition");
}

} // namespace banjo
