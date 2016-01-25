// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>

namespace banjo
{

// -------------------------------------------------------------------------- //
// Declarations



Decl&
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


// -------------------------------------------------------------------------- //
// Variable declarations

// Parse a variable declaration.
//
//    variable-declaration:
//      'var' type declarator [initializer] ';'
Decl&
Parser::variable_declaration()
{
  Token tok = require(var_tok);
  Type& t = type();
  Name& n = declarator();

  // Point of declaration.
  Variable_decl& d = on_variable_declaration(tok, n, t);
  Enter_scope s(*this, d);

  // Initialization
  if (lookahead() == semicolon_tok) {
    on_default_initialization(d);
    match(semicolon_tok);
  } else {
    initializer(d);
    match(semicolon_tok);
  }

  return d;
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
Expr&
Parser::initializer(Decl& d)
{
  if (lookahead() == eq_tok)
    return equal_initializer(d);
  else if (lookahead() == lparen_tok)
    return paren_initializer(d);
  else if (lookahead() == lbrace_tok)
    return brace_initializer(d);
  throw Syntax_error("expected initializer");
}


// Parse a value initializer.
//
//    equal-initializer:
//      '=' expression
Expr&
Parser::equal_initializer(Decl& d)
{
  require(eq_tok);
  Expr& expr = expression();
  return on_equal_initialization(d, expr);
}


// Parse a direct initializer.
//
//    paren-initializer:
//      '(' expression-list ')'
Expr&
Parser::paren_initializer(Decl&)
{
  // on_paren
  lingo_unimplemented();
}


// Parse a brace initializer.
//
//    brace-initializer:
//      '{' expression-list '}'
Expr&
Parser::brace_initializer(Decl&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Function declarations

// Parse a parameter declaration.
//
//    parameter-declaration:
//      type [identifier] [value-initializer]
Decl&
Parser::parameter_declaration()
{
  lingo_unimplemented();
  /*
  Type& t = type();

  Name* n = nullptr;
  if (Token id = match_if(identifier_tok))
    n = &on_simple_id(id);

  if (lookahead() == eq_tok) {
    Expr& e = equal_initializer();
    return on_parameter_declaration(*n, t, e);
  } else {
    return on_parameter_declaration(*n, t);
  }
  */
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
Decl&
Parser::function_declaration()
{
  lingo_unimplemented();
}


// Parse a namespace declaration.
//
//    namespace-declaration:
//      'namespace' [declarator] '{' declaration-seq '}'
Decl&
Parser::namespace_declaration()
{
  lingo_unimplemented();
  // Token tok = require(namespace_tok);
  // Name& n = declarator();
  // declaration_seq();
  // return on_namespace_declaration(tok, n);
}


// -------------------------------------------------------------------------- //
// Template parameters

// Parse a template parameter list.
//
//    template-parameter-list:
//      template-parameter
//      template-parameter-list template parameter
Decl_list
Parser::template_parameter_list()
{
  lingo_unimplemented();
}


// Parse a template parameter declaration:
//
//    template-parameter:
//      type-template-parameter
//      value-template-parameter
//      template-template-declaration
Decl&
Parser::template_parameter()
{
  lingo_unimplemented();
}


// Parse a typename declaration.
//
//    typename-declaration:
//      'typename' identifier ['=' type]
Decl&
Parser::type_template_parameter()
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Template declarations

// Parse a template declaration.
//
//    tempate-declaration:
//      'template' '<' template-parameter-list '>' declaration
Decl&
Parser::template_declaration()
{
    lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Names of declarations

// Parse a declarator.
//
//    declarator:
//      id
//
// A declarator is the declared name of an entity. If a declarator
// is a qualified-id, then the qualification explicitly gives the
// context in which the declaration was previously declared.
//
// TODO: Rename this to declaration-name.
//
// TODO: Handle declaration names.
Name&
Parser::declarator()
{
  Name& n = id();
  return on_declarator(n);
}


// -------------------------------------------------------------------------- //
// Miscellaneous



// Returns true if the current token kind indicates
// the start of a declaration.
//
// TODO: There shoud be a simpler formulation.
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

// Parse a declaration sequence.
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


} // namespace banjo
