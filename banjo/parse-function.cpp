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
//      'def' identifier ':' parameter-clause ['->' type] ';'
//      'def' identifier ':' parameter-clause ['->' type] function-body
//      'def' identifier ':' parameter-clause ['->' type] '=' expression-statement
//      'def' identifier ':' parameter-clause expression-statement
//
//    parameter-spec:
//      '(' [parameter-list] ')'
//
//    function-body:
//      compound-statement
//
// Note that that the last form cannot have a type annotation because there
// is no token delimiting for the start of the definition.
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
//
// TODO: Allow empty definitions for functions. Functions with empty 
// definitions are either foreign or synthesized.
Decl&
Parser::function_declaration()
{
  // Helper functions
  Match_any_token_pred end_type(*this, lbrace_tok, eq_tok);
  Match_token_pred     end_expr(*this, semicolon_tok);

  require(def_tok);
  Name& name = identifier();

  // Optional colon.
  match_if(colon_tok);

  // NOTE: We don't have to enter a new scope because parameters are 
  // declared as top-level members of the function body. That happens
  // in finish_function_declaration.
  Decl_list parms = parameter_clause();

  // Match the return type.
  Type* ret;
  if (match_if(arrow_tok))
    ret = &unparsed_type(end_type);
  else
    ret = &cxt.make_auto_type();

  // Declare the function and establish the function scope.
  Decl& fn = start_function_declaration(name, parms, *ret);
  Enter_scope scope(cxt, cxt.saved_scope(fn));

  // Match the function definition.
  //
  // TODO: Handle default and deleted functions.
  if (match_if(eq_tok)) { // '=' ...
    Expr& expr = unparsed_expression(end_expr);
    match(semicolon_tok);
    return finish_function_declaration(fn, expr);
  } else if (next_token_is(lbrace_tok)) { // '{' ... '}'
    Stmt& stmt = compound_statement();
    return finish_function_declaration(fn, stmt);
  } else {
    Expr& expr = unparsed_expression(end_expr);
    match(semicolon_tok);
    return finish_function_declaration(fn, expr);
  }
}


// -------------------------------------------------------------------------- //
// Parameters

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
//      [parameter-specifier] [identifier] [':' type] ['=' expression]
//      [parameter-specifier] identifier [':=' expression]
//
// Note that parameter packs and variadics are part of the type system
// and can simply be parsed as part of that type.
Decl&
Parser::parameter_declaration()
{
  Match_any_token_pred end_type(*this, comma_tok, rparen_tok, eq_tok);

  // Parse and cache the optional parameter specifier.
  parameter_specifier_seq();

  // Parse the optional name.
  Name* name;
  if (next_token_is_not(colon_tok))
    name = &identifier();
  else
    name = &cxt.get_id();

  if (match_if(colon_tok)) {
    if (next_token_is(eq_tok))
      lingo_unimplemented("default arguments");

    Type& type = unparsed_type(end_type);

    // TODO: Implement default argument types.
    if (next_token_is(eq_tok))
      lingo_unimplemented("default arguments");

    return on_function_parameter(*name, type);
  }

  Type& type = cxt.make_auto_type();

  // TODO: Implement default arguments.
  if (next_token_is(eq_tok))
    lingo_unimplemented("default arguments");

  return on_function_parameter(*name, type);
}


// -------------------------------------------------------------------------- //
// Coroutines.

// Parse a coroutine.
//
//    coroutine-definition:
//      'codef' identifier ':' parameter-clause '->' type compound-statement
//
// TODO: Allow deduction from yield statements?
Decl&
Parser::coroutine_declaration()
{
  Match_token_pred end_type(*this, lbrace_tok);
  
  require(coroutine_tok);

  // Name
  Name& n = identifier(); // Name of coroutine
  match_if(colon_tok);
  
  // Parameters.
  Decl_list params = parameter_clause(); // (...)

  // Return type.
  match(arrow_tok);
  Type& yield = unparsed_type(end_type);

  // Body.
  Stmt& body = compound_statement();
  
  return on_coroutine_declaration(n, params, yield, body);
}

} // namespace banjo
