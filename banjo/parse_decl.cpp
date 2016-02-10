// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "print.hpp"

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
    case struct_tok:
    case class_tok:
      return class_declaration();
    case enum_tok:
      lingo_unimplemented();
    case namespace_tok:
      return namespace_declaration();
    case template_tok:
      return template_declaration();
    case concept_tok:
      return concept_declaration();
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
  Decl& d = on_variable_declaration(tok, n, t);
  Enter_scope s(cxt, cxt.make_initializer_scope(d));

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

  // Enter function parameter scope and parse function parameters.
  Enter_scope pscope(cxt, cxt.make_function_parameter_scope());
  Decl_list ps;
  match(lparen_tok);
  if (lookahead() != rparen_tok)
    ps = parameter_list();
  match(rparen_tok);
  Type& t = return_type();

  // Point of declaration.
  Decl& fn = on_function_declaration(tok, n, ps, t);

  // Parse the definition, if any.
  if (lookahead() == semicolon_tok) {
    match(semicolon_tok);
  } else {
    // Enter function scope and parse the function definition.
    Enter_scope fscope(cxt, cxt.make_function_scope(fn));
    function_definition(fn);
  }

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


// -------------------------------------------------------------------------- //
// Classes

// Parse a class declaration.
//
//    class-declaration:
//      struct declarator ';'
//      struct class-definition
//      class declarator ';'
//      class class-definition
//
// TODO: Add support for classes.
Decl&
Parser::class_declaration()
{
  lingo_assert(lookahead() == struct_tok || lookahead() == class_tok);
  Token tok = accept();
  Name& n = declarator();

  // Point of declaration.
  Decl& cls = on_class_declaration(tok, n);

  if (!match_if(semicolon_tok)) {
    // FIXME: Enter class scpoe.
    class_definition(cls);
  }

  return cls;
};


// Parse a class definition (it's body).
//
//    class-definition:
//      '=' 'delete' ';'
//      [base-clause] class-body
//
//    class-body:
//      '{' [member-seq] '}'
//
// TODO: Implment base class parsing.
Def&
Parser::class_definition(Decl& d)
{
  // Match deleted definitions.
  if (match_if(eq_tok)) {
    match(delete_tok);
    match(semicolon_tok);
    return on_deleted_definition(d);
  }

  // Match the class body.
  Decl_list ds;
  match(lbrace_tok);
  if (lookahead() != rbrace_tok)
    ds = member_seq();
  match(rbrace_tok);
  return on_class_definition(d, ds);
}


// Parse a sequence of class members.
//
//    member-seq:
//      member-declaration:
//      member-seq member-declaration
Decl_list
Parser::member_seq()
{
  Decl_list ds;
  do {
    Decl& d = member_declaration();
    ds.push_back(d);
  } while (lookahead() != rbrace_tok);
  return ds;
}


// Parse a member-declaration.
//
//    member-declaration:
//      variable-declaration
//      function-declaration
//
// TODO: Support member types and templates.
//
// TODO: Have different declarations for members and non-members?
Decl&
Parser::member_declaration()
{
  switch (lookahead()) {
    case var_tok:
      return variable_declaration();
    default:
      break;
  }
  throw Syntax_error("expected member-declaration");
}


// -------------------------------------------------------------------------- //
// Namespaces

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
// Template declarations

// Parse a template declaration.
//
//    tempate-declaration:
//      'template' '<' template-parameter-list '>' [requires-clause] declaration
//
// FIXME: Support explicit template instantations in one way or
// another.
Decl&
Parser::template_declaration()
{
  require(template_tok);

  // TODO: Allow >> to close the template parameter list in the
  // case of default template arguments.
  Enter_scope scope(cxt, cxt.make_template_parameter_scope());
  match(lt_tok);
  Decl_list ps = template_parameter_list();
  match(gt_tok);

  // Parse the optional requires clause, followed by the
  // definition.
  //
  // The definition is parsed in different branches in case I may,
  // in the future, want to establish a new scope for constrained
  // declarations (i.e., separate checking).
  if (lookahead() == requires_tok) {
    Expr& c = requires_clause();
    Parsing_template save(*this, ps, c);
    return declaration();
  } else {
    Parsing_template save(*this, ps);
    return declaration();
  }

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
  do {
    Decl& d = template_parameter();
    ds.push_back(d);
  } while (match_if(comma_tok));
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


// Parse a type template parameter.
//
//    type-template-parameter:
//        typename [identifier] ['='' type]
//
// Note that the point of declaration for a template parameter is
// past the full definition (after the default argument, if present).
Decl&
Parser::type_template_parameter()
{
  match(typename_tok);

  // Get the optional identifier. Create a placeholder
  // if no name is given.
  Name* n;
  if (Token id = match_if(identifier_tok))
    n = &on_simple_id(id);
  else
    n = &build.get_id();

  // Parse the default argument.
  Type* t = nullptr;
  if (lookahead() == eq_tok)
    t = &type();

  // Point of declaration.
  Decl* d;
  if (t)
    d = &on_type_template_parameter(*n, *t);
  else
    d = &on_type_template_parameter(*n);
  return *d;
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


Expr&
Parser::requires_clause()
{
  require(requires_tok);
  return expression();
}


// -------------------------------------------------------------------------- //
// Concept declarations


Decl&
Parser::concept_declaration()
{
  Token tok = require(concept_tok);
  Name& n = declarator();

  Enter_scope pscope(cxt, cxt.make_template_parameter_scope());
  match(lt_tok);
  Decl_list ps = template_parameter_list();
  match(gt_tok);

  // Point of declaration.
  Decl& con = on_concept_declaration(tok, n, ps);

  // TODO: Isn't this just an equal-init? Probably not since
  // we're not converting (we except bool everywhere).
  match(eq_tok);
  Expr& e = expression();
  on_concept_definition(con, e);
  match(semicolon_tok);
  return con;
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


// Parse a declaration sequence.
//
//    declaration-seq:
//      declaration
//      declaration-seq declaration
//
// Note that declaration-seqs is only referenced from translation-unit
// and namespace-definition. Therefore, it must terminate on EOF or
// a '}'.
//
// NOTE: We also terminate on the first identifier in order to support
// the inspection tool. I wonder if there is a more graceful way of doing
// this.
Decl_list
Parser::declaration_seq()
{
  Decl_list ds;
  // FIXME: Catch declaration errors and continue parsing.
  do {
    Decl& d = declaration();
    ds.push_back(d);
  } while (peek() && lookahead() != rbrace_tok && lookahead() != identifier_tok);
  return ds;
}



} // namespace banjo
