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
// Declaration specifiers
//
// Declaration specifiers are an optional sequence of terms parsed
// before a declaration's type. They look like part of the type but
// are distinct.

static inline void
accept_specifier(Parser& p, Specifier_set s)
{
  p.accept();
  p.decl_specs() |= s;
}


// Parse a sequence of declaration specifiers.
//
//    specifier:
//      storage-specifier
//      function-specifier
//      access-specifier
//
//    storage-specifier:
//      static
//      dynamic
//
//    function-specifier:
//      implicit
//      explicit
//      inline
//
//    access-specifier:
//      public
//      private
//      protected
//
// TODO: Implement external storage? Note that foreign declarations
// should be a declaration kind because they parse a little differently
// than a simple specifier.
Specifier_set
Parser::specifier_seq()
{
  // Reset the declaration specifiers (they should be empty).
  decl_specs() = Specs();

  // Match specifiers.
  while (true) {
    switch (lookahead()) {
      case static_tok:
        accept_specifier(*this, static_spec);
        break;

      case dynamic_tok:
        accept_specifier(*this, static_spec);
        break;

      case inline_tok:
        accept_specifier(*this, inline_spec);
        break;

      case explicit_tok:
        accept_specifier(*this, explicit_spec);
        break;

      case implicit_tok:
        accept_specifier(*this, implicit_spec);
        break;

      case public_tok:
        accept_specifier(*this, public_spec);
        break;

      case private_tok:
        accept_specifier(*this, private_spec);
        break;

      case protected_tok:
        accept_specifier(*this, protected_spec);
        break;

      default:
        return decl_specs();
    }
  }
  lingo_unreachable();
}


// Parse a sequence of parameter specifiers.
//
//    parameter-specifier:
//      in
//      out
//      mutable
//      consume
//      forward
//      virtual
//      const
Specifier_set
Parser::parameter_specifier_seq()
{
  // Reset the declaration specifiers (they should be empty).
  decl_specs() = Specs();

  // Match specifiers.
  while (true) {
    switch (lookahead()) {
      case in_tok:
        accept_specifier(*this, in_spec);
        break;

      case out_tok:
        accept_specifier(*this, out_spec);
        break;

      case mutable_tok:
        accept_specifier(*this, mutable_spec);
        break;

      case consume_tok:
        accept_specifier(*this, consume_spec);
        break;

      case forward_tok:
        accept_specifier(*this, forward_spec);
        break;

      case virtual_tok:
        accept_specifier(*this, virtual_spec);
        break;

      case const_tok:
        accept_specifier(*this, const_spec);
        break;

      default:
        return decl_specs();
    }
  }
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Declarations

// Parse a declaration.
//
//    declaration:
//      [specifier-seq] basic-declaration
//
//    basic-declaration:
//      variable-declaration
//      constant-declaration
//      function-declaration
//      type-declaration
//      concept-declaration
Decl&
Parser::declaration()
{
  // Parse and cache the specifier sequences.
  specifier_seq();

  switch (lookahead()) {
    case var_tok:
      return variable_declaration();

    case const_tok:
      return constant_declaration();
    
    case def_tok:
      return function_declaration();

    case coroutine_tok:
      return coroutine_declaration();

    case class_tok:
      return class_declaration();

    case concept_tok:
      lingo_unreachable();
    case super_tok:
      return super_declaration();
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
  // Helper functions.
  Match_any_token_pred end_type(*this, eq_tok, semicolon_tok);
  Match_token_pred     end_init(*this, semicolon_tok);

  require(var_tok);
  Name& name = identifier();
  match(colon_tok);

  // Match the ":=" form.
  if (match_if(eq_tok)) {
    Type& type = cxt.make_auto_type();
    Expr& init = unparsed_expression(end_init);
    match(semicolon_tok);
    return on_variable_declaration(name, type, init);
  }

  // Match the type.
  Type& type = unparsed_type(end_type);

  // Match the "name : type =" form.
  if (match_if(eq_tok)) {
    Expr& init = unparsed_expression(end_init);
    match(semicolon_tok);
    return on_variable_declaration(name, type, init);
  }

  // Otherwise, match the "name : type ;" form.
  match(semicolon_tok);
  return on_variable_declaration(name, type);
}


// -------------------------------------------------------------------------- //
// Variable declarations

// Parse a constant declaration.
//
//    variable-declaration:
//      'const' identifier ':' [template-header] type '=' initializer ';'
//      'const' identifier ':' '=' initializer ';'
Decl&
Parser::constant_declaration()
{
  // Helper functions.
  Match_any_token_pred end_type(*this, eq_tok, semicolon_tok);
  Match_token_pred     end_init(*this, semicolon_tok);

  require(const_tok);
  Name& name = identifier();
  match(colon_tok);

  // Match the ":=" form.
  if (match_if(eq_tok)) {
    Type& type = cxt.make_auto_type();
    Expr& init = unparsed_expression(end_init);
    match(semicolon_tok);
    return on_constant_declaration(name, type, init);
  }

  // Match the type.
  Type& type = unparsed_type(end_type);

  // Match the "name : type =" form.
  match_if(eq_tok);
  Expr& init = unparsed_expression(end_init);
  match(semicolon_tok);
  return on_constant_declaration(name, type, init);
}


// -------------------------------------------------------------------------- //
// Base class declarations

// Parse a base class (super) declaration.
//
//    super-declaration:
//      super [identifier] : type;
Decl&
Parser::super_declaration()
{
  Match_token_pred end_type(*this, semicolon_tok);

  require(super_tok);

  // Match the optional identifier.
  Name* name;
  if (next_token_is(identifier_tok))
    name = &identifier();
  else
    name = &build.get_id();

  // Match type type.
  match(colon_tok);
  Type& type = unparsed_type(end_type);
  match(semicolon_tok);

  return on_super_declaration(*name, type);
}


// -------------------------------------------------------------------------- //
// Template declarations

// Parse a template declaration.
//
//    tempate-declaration:
//      'template' '<' template-parameter-list '>' [requires-clause] declaration
//
// FIXME: Support explicit template instantiations in one way or
// another.
Decl&
Parser::template_declaration()
{
  #if 0
  require(template_tok);

  // Build a psuedo-scope.
  //
  // FIXME: Merge this with template parameter scope. I don't think
  // that it's serving a very useful purpose.
  // Template_scope& tmp = cxt.make_template_scope();
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
    // Enter_scope cscope(cxt, cxt.make_constrained_scope(*tmp.cons));
    Parsing_template save(*this, &tmp.parms, tmp.cons);
    return declaration();
  } else {
    Parsing_template save(*this, &tmp.parms);
    return declaration();
  }
  #endif
  lingo_unreachable();
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

  // Enter_template_parameter_scope pscope(cxt);
  match(lt_tok);
  Decl_list ps = template_parameter_list();
  match(gt_tok);

  // Point of declaration. Enter the associated context prior
  // to defininging the concept.
  Decl& con = on_concept_declaration(tok, n, ps);
  // Enter_scope cscope(cxt, cxt.make_concept_scope(con));
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
    default:
      return expression_requirement();
  }
}


// -------------------------------------------------------------------------- //
// Names of declarations

// Parse a declarator.
//
//    declarator:
//      identifier
//
// A declarator is the declared name of an entity. If a declarator
// is a qualified-id, then the qualification explicitly gives the
// context in which the declaration was previously declared.
//
// TODO: Rename this to declaration-name.
//
// FIXME: Allow non-identifiers?
Name&
Parser::declarator()
{
  return identifier();
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
