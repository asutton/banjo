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
    case typename_tok:
      lingo_unreachable();
    case concept_tok:
      lingo_unreachable();
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
//      'var' identifier ':' [template-header] type ';'
//      'var' identifier ':' [template-header] type '=' initializer ';'
//      'var' identifier ':' '=' initializer ';'
//
// TODO: Are there other forms of variable declaration?
Decl&
Parser::variable_declaration()
{
  require(var_tok);
  Name& name = identifier();
  match(colon_tok);

  // Match the ":=" form.
  if (match_if(eq_tok)) {
    Type& type = cxt.get_auto_type();
    Expr& init = unparsed_variable_initializer();
    match(semicolon_tok);
    return on_variable_declaration(name, type, init);
  }

  // Match the type.
  Type& type = unparsed_variable_type();

  // Match the "name : type =" form.
  if (match_if(eq_tok)) {
    Expr& init = unparsed_variable_initializer();
    match(semicolon_tok);
    return on_variable_declaration(name, type, init);
  }

  // Otherwise, match the "name : type ;" form.
  match(semicolon_tok);
  return on_variable_declaration(name, type);
}


// Return an unparsed type for the variable's type specification.
Type&
Parser::unparsed_variable_type()
{
  Token_seq toks;
  while (true) {
    if (next_token_is_one_of(semicolon_tok, eq_tok) && !in_braces())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_type(std::move(toks));
}


// Returns an unparsed variable initializer.
Expr&
Parser::unparsed_variable_initializer()
{
  Token_seq toks;
  while (true) {
    if (next_token_is(semicolon_tok) && !in_braces())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_expression(std::move(toks));
}


// Parse a variable initializer.
//
//    initializer:
//      value-initializer
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
  lingo_unimplemented("parse paren-initializer");
}


// Parse a brace initializer.
//
//    brace-initializer:
//      '{' expression-list '}'
Expr&
Parser::brace_initializer(Decl&)
{
  lingo_unimplemented("parse brace-initializer");
}


// -------------------------------------------------------------------------- //
// Function declarations

// Parse a function declaration or definition.
//
//    function-declaration:
//      'def' identifier ':' [template-header] parameter-clause expression-statement
//      'def' identifier ':' [template-header] parameter-clause '->' type function-body
//
//    parameter-spec:
//      '(' [parameter-list] ')'
//
//    function-body:
//      compound-statement
//      '=' expression-statement
//
// TODO: Allow named return types. That would change the grammar for return
// types to a return declaration. Note that not all functions can have named
// returns (e.g., void functions, functions returning references, etc.). This
// also leaks implementation details into the interface.
//
//    def f() -> ret:string { ret = "hello"; }
Decl&
Parser::function_declaration()
{
  require(def_tok);
  Name& name = identifier();
  match(colon_tok);

  // NOTE: We don't have to enter a scope because we aren't doing
  // lookup in the first pass.
  Decl_list parms = parameter_clause();

  // -> type function-definition.
  if (match_if(arrow_tok)) {
    Type& ret = unparsed_return_type();

    // = expression ;
    if (match_if(eq_tok)) {
      Expr& body = unparsed_expression_body();
      match(semicolon_tok);
      return on_function_declaration(name, parms, ret, body);
    }

    // { ... }
    else {
      Stmt& body = unparsed_function_body();
      return on_function_declaration(name, parms, ret, body);
    }
  }

  // Othersise, the return type is unspecified, allowing for
  // anonymous expressions.
  // Type& ret = cxt.get_auto_type();

  lingo_unreachable();

  // // Point of declaration.
  // Decl& fn = on_function_declaration(tok, n, ps, t);
  //
  // // Parse the definition, if any.
  // if (lookahead() == semicolon_tok) {
  //   match(semicolon_tok);
  // } else {
  //   // Enter function scope and parse the function definition.
  //   Enter_scope fscope(cxt, cxt.make_function_scope(fn));
  //   function_definition(fn);
  // }
  //
  // return fn;
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
//      identifier [':' type] ['=' expression]
//      identifier [':=' expression]
//
// TODO: Extend the grammar to support (named?) variadics and function
// argument packs.
Decl&
Parser::parameter_declaration()
{
  Name& name = identifier();

  if (match_if(colon_tok)) {
    if (next_token_is(eq_tok))
      lingo_unimplemented("default arguments");

    Type& type = unparsed_parameter_type();

    if (next_token_is(eq_tok))
      lingo_unimplemented("default arguments");

    return on_function_parameter(name, type);
  }

  Type& type = cxt.get_auto_type();

  if (next_token_is(eq_tok))
    lingo_unimplemented("default arguments");

  return on_function_parameter(name, type);
}


Type&
Parser::unparsed_parameter_type()
{
  Token_seq toks;
  Brace_matching_sentinel in_level(*this);
  while (true) {
    if (next_token_is_one_of(comma_tok, rparen_tok) && in_level())
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
  Brace_matching_sentinel in_level(*this);
  while (true) {
    if (next_token_is_one_of(lbrace_tok) && in_level())
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
  Brace_matching_sentinel in_level(*this);
  while (true) {
    if (next_token_is(semicolon_tok) && in_level())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_expression(std::move(toks));
}


// Returns an unparsed compound statement that defines a function.
Stmt&
Parser::unparsed_function_body()
{
  Token_seq toks;
  toks.push_back(match(lbrace_tok));
  Brace_matching_sentinel in_level(*this);
  while (true) {
    if (next_token_is(rbrace_tok) && in_level())
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
  lingo_unimplemented("parse namespace");
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

  // Build a psuedo-scope.
  //
  // FIXME: Merge this with template parameter scope. I don't think
  // that it's serving a very useful purpose.
  Template_scope& tmp = cxt.make_template_scope();
  Enter_scope tscope(cxt, tmp);

  // TODO: Allow >> to close the template parameter list in the
  // case of default template arguments.
  Enter_template_parameter_scope pscope(cxt);
  match(lt_tok);
  tmp.parms = template_parameter_list();
  match(gt_tok);

  // Parse the optional requires clause, followed by the parameterized
  // declaration.
  if (next_token_is(requires_tok)) {
    // TODO: How are dependent names resolved in a requires clause?
    tmp.cons = &requires_clause();
    Enter_scope cscope(cxt, cxt.make_constrained_scope(*tmp.cons));
    Parsing_template save(*this, &tmp.parms, tmp.cons);
    return declaration();
  } else {
    Parsing_template save(*this, &tmp.parms);
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
      lingo_unimplemented("parse constrained-parameter");
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
  lingo_unimplemented("parse value-template-parameter");
}


Decl&
Parser::template_template_parameter()
{
  lingo_unimplemented("parse template-template-parameter");
}


// Parse a where clause:
//
//    where-clause:
//      'requires' logical-or-expression
Expr&
Parser::requires_clause()
{
  require(requires_tok);
  return logical_or_expression();
}


// -------------------------------------------------------------------------- //
// Concept declarations


// Parse a concept definition.
//
//    concept-declaration:
//      'concept' identifier '<' template-parameter-list '>' concept-definition
//
// TODO: I'm not sure that I like this syntax. Maybe we should make
// concepts look like any other template:
//
//    template<typename T> concept C = ...
//
// Note that this would allow for non-template concepts as well:
//
//    concept C = X && Y && Z;
//
// The benefit to doing this is that C would now be usable as a constraint
// in non-function templates.
//
// FIXME: Make this happen.
Decl&
Parser::concept_declaration()
{
  Token tok = require(concept_tok);
  Name& n = declarator();

  Enter_template_parameter_scope pscope(cxt);
  match(lt_tok);
  Decl_list ps = template_parameter_list();
  match(gt_tok);

  // Point of declaration. Enter the associated context prior
  // to defininging the concept.
  Decl& con = on_concept_declaration(tok, n, ps);
  Enter_scope cscope(cxt, cxt.make_concept_scope(con));
  concept_definition(con);
  return con;
}


// Parse a concept definition.
//
//    concept-definition:
//      '=' logical-or-expression
//      '{' concept-member-seq '}'
//
// TODO: Not all statements are valid within a concept definition.
// Certain declarations are valid, and certain expression statements
// are valid, but not the entire set.
//
// It would be useful to define a grammatical subset of those things
// that should be valid.
Def&
Parser::concept_definition(Decl& d)
{
  if (match_if(eq_tok)) {
    Expr& e = expression();
    Def& def = on_concept_definition(d, e);
    match(semicolon_tok);
    return def;
  } else {
    match(lbrace_tok);
    Req_list rs = concept_member_seq();
    Def& def = on_concept_definition(d, rs);
    match(rbrace_tok);
    return def;
  }
}


// Parse a sequence of concept members.
//
//    concept-member-seq:
//      concept-member
//      concept-member-seq concept-member
Req_list
Parser::concept_member_seq()
{
  Req_list rs;
  do {
    Req& r = concept_member();
    rs.push_back(r);
  } while (next_token_is_not(rbrace_tok));
  return rs;
}



// Parse a concept member.
//
//    concept-member:
//      type-requirement
//      syntactic-requirement
//      semantic-requirement
//      expression-requirement
//
// TODO: When we adopt shorthand for concepts, we'll need to
// disambiguate some type requirements from expression
// requirements. For example:
//
//    C X<T>; // A type requirement, X<T> must satsify C
//
//    C<T>; // An expression requirement, C<T> must be satisfied.
//
// We'll need a tentative parse to disambiguate.
Req&
Parser::concept_member()
{
  switch (lookahead()) {
    case typename_tok:
      return type_requirement();
    case requires_tok:
      return syntactic_requirement();
    case axiom_tok:
      return semantic_requirement();
    default:
      return expression_requirement();
  }
}


Decl&
Parser::axiom_declaration()
{
  lingo_unimplemented("parse axiom-declaration");
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
