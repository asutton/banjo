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
    case var_tok: return variable_declaration();
    case def_tok: return function_declaration();

    // case struct_tok:
    // case class_tok:
    //   return class_declaration();

    // case enum_tok: //   return enum_declaration();

    case namespace_tok: return namespace_declaration();
    // case template_tok:return template_declaration();
    default: break;
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


// Parse a function declaration or definition.
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
Decl&
Parser::function_declaration()
{
  Token tok = require(def_tok);
  Name& n = declarator();

  // FIXME: Create a scope for parameters so that they can
  // be resolved during the parsing of the return type.
  Decl_list p;
  match(lparen_tok);
  if (lookahead() != rparen_tok)
    p = parameter_list();
  match(rparen_tok);
  Type& t = return_type();

  // Point of declaration. Enter function scope.
  Function_decl& fn = on_function_declaration(tok, n, p, t);

  // Parse the definition, if any.
  if (lookahead() == semicolon_tok)
    match(semicolon_tok);
  else
    function_definition(fn);

  return fn;
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
//      ... [identifier]
//      type [identifier] [value-initializer]
//
// TODO: Consider two different parses. Default arguments are
// not allowed for the ... parameter.
Decl&
Parser::parameter_declaration()
{
  // If ... precedes the type, then this will be the variadic
  // parameter.
  Type* t;
  if (match_if(ellipsis_tok))
    lingo_unimplemented();
  else
    t = &type();

  // Guarantee a parameter name. This could be DeBruijn-numbered.
  Name* n;
  if (Token id = match_if(identifier_tok))
    n = &on_simple_id(id);
  else
    n = &build.get_id();

  // Point of declaration.
  Object_parm& parm = on_function_parameter(*n, *t);

  // Parse the default argument.
  if (lookahead() == eq_tok)
    equal_initializer(parm);

  return parm;
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
//
// FIXME: Support explicit template instantations in one way or
// another.
Decl&
Parser::template_declaration()
{
  require(template_tok);

  // TODO: Allow >> to close the template parameter list in the
  // case of default template arguments.
  match(lt_tok);
  Decl_list p = template_parameter_list();
  match(gt_tok);

  Decl& d = declaration();

  // FIXME: Semantic analysis?
  return d;
}


// Parse a template parameter list.
//
//    template-parameter-list:
//      template-parameter
//      template-parameter-list ',' template-parameter
Decl_list
Parser::template_parameter_list()
{
  Decl_list ds;

  // TOOD: Could terminate on >>.
  while (lookahead() != gt_tok) {
    Decl& d = template_parameter();
    ds.push_back(d);
  }

  return ds;
}


// Parse a template parameter.
//
//    template-parameter:
//      type-template-parameter
//      value-template-parameter
//      template-template-parameter
Decl&
Parser::template_parameter()
{
  switch (lookahead()) {
    case typename_tok: return type_template_parameter();
    case const_tok: return value_template_parameter();
    case template_tok: return template_template_parameter();

    default:
      // FIXME: Concepts!
      lingo_unimplemented();
  }
}


// Parse a template parameter.
Decl&
Parser::type_template_parameter()
{
  match(typename_tok);
  Tok id = identifier();
  if (lookahead() == eq_tok) {

  }
}


// Parse a value template parameter.
//
//    value-template-parameter:
//      'const' type [identifier] [equal-initializer]
Decl&
Parser::value_template_parameter()
{
  lingo_unimplemented();
}


Decl&
Parser::template_template_parameter()
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
  //
  // FIXME: I don't like starts_declaration. What are the different
  // ways that we can end the loop. EOF or '}'. Anything else?
  declaration();
  while (starts_declaration(*this))
    declaration();

  return {};
}


} // namespace banjo
