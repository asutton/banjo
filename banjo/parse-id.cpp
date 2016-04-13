// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast-name.hpp"
#include "ast-type.hpp"
#include "ast-expr.hpp"
#include "ast-decl.hpp"
#include "printer.hpp"

#include <iostream>

namespace banjo
{


static inline Operator_kind
consume_op(Parser& p, Operator_kind op)
{
  p.accept();
  return op;
}


static inline Operator_kind
consume_op(Parser& p, Token_kind tok, Operator_kind op)
{
  p.accept();
  p.match(tok);
  return op;
}


// Match any operator.
//
//    operator: one_of
//      + - * / %
//      == != < > <= >= <=>
//      && || !
//      () []
//      =
//
// TODO: Match bitwise operators.
Operator_kind
Parser::any_operator()
{
  switch (lookahead()) {
    case plus_tok: return consume_op(*this, add_op);
    case minus_tok: return consume_op(*this, sub_op);
    case star_tok: return consume_op(*this, mul_op);
    case slash_tok: return consume_op(*this, div_op);
    case percent_tok: return consume_op(*this, rem_op);
    case eq_eq_tok: return consume_op(*this, eq_op);
    case bang_eq_tok: return consume_op(*this, ne_op);
    case lt_tok: return consume_op(*this, lt_op);
    case gt_tok: return consume_op(*this, gt_op);
    case lt_eq_tok: return consume_op(*this, le_op);
    case gt_eq_tok: return consume_op(*this, ge_op);
    case lt_eq_gt_tok: return consume_op(*this, cmp_op);
    case amp_amp_tok: return consume_op(*this, and_op);
    case bar_bar_tok: return consume_op(*this, or_op);
    case bang_tok: return consume_op(*this, not_op);
    case eq_tok: return consume_op(*this, assign_op);
    case lparen_tok: return consume_op(*this, rparen_tok, call_op);
    case lbrace_tok: return consume_op(*this, rbrace_tok, index_op);
    default: throw Syntax_error("expected operator");
  }
}


// Parse an identifier.
Name&
Parser::identifier()
{
  Token tok = match(identifier_tok);
  return on_simple_id(tok);
}


// -------------------------------------------------------------------------- //
// Ids, names, and aliases
//
// An "id" is a (possibly qualified) declared identifier of
// any kind. Ids are typically prefixed by their syntactic
// form (e.g., operator-id or template-id).
//
// A "name" is an id of a specific kind (e.g., class-name or
// template-name). Note that "names" are always unqualified.
// An "alias" is a "name" that was declared as an alias for a
// declaration of some kind (type-alias e.g. namespace-alias).
//
// Note that we *could* define parsers for variable names,
// function names, and overload names, but the rules that
// govern their use are purely semantic (type checking) and
// do not affect the current parse. For example, for a variable
// `v`, the expression `v(x)`` is valid only if `v` has class
// type and provides a function call operator.


// Parse a name that refers to a declaration of any kind.
//
//    id:
//      unqualified-id
//      qualified-id
Name&
Parser::id()
{
  // FIXME: Re-enable this when I add scoping stuff.
  // if (Name* n = match_if(&Parser::qualified_id))
  //   return *n;
  return unqualified_id();
}


// Parse an unqualified-id.
//
//    unqualified-id:
//      identifier
//      operator-id
//      conversion-id
//      literal-id
//      destructor-id
//      template-id
//      concept-id
//
// TODO: Support operator, conversion, and literal ids. Note that
// all of those begin with 'operator', but can be syntactically
// differentiated by lookahead(1). Operator-ids are followed by
// an operator symbol, literal id's are followed by an empty string
// literal, and conversion id's are the rest.
//
// FIXME: Finish implementing me. Note that the interpretation of
// an identifier depends on both lookup and context.
Name&
Parser::unqualified_id()
{
  if (next_token_is(tilde_tok))
    return destructor_id();

  if (Token tok = match_if(operator_tok)) {
    Operator_kind op = any_operator();
    return on_operator_id(tok, op);
  }

  // Use a trial parser to determine if we're looking at a template-id
  // or concept-id instead of a plain old identifier.
  //
  // FIXME: This doesn't really need to be a trial parse. Just get
  // the identifier, and depending on a) its resolution and b) whether
  // we assume it should be interpreted as a template or concept via
  // a prior keyword, handle the template argument.
  if (Name* n = match_if(&Parser::template_id))
    return *n;
  if (Name* n = match_if(&Parser::concept_id))
    return *n;

  Token tok = match(identifier_tok);
  return on_simple_id(tok);
}


// Parse a destructor-id. A destructor-id names the destructor of
// a class or a decltype-type that refers to a class.
//
//    destructor-id:
//      '~' primary-type
//
// TODO: Does the destructor name realal
Name&
Parser::destructor_id()
{
  Token tok = match(tilde_tok);
  Type& type = primary_type();
  return on_destructor_id(tok, type);
}


// Parse a template id.
//
//    template-id:
//      simple-template-id
//      operator-id template-argument-clause
//      literal-id template-argument-clause
//
// TODO: Handle the operator and literal cases.
Name&
Parser::template_id()
{
  Decl& temp = template_name();

  // FIXME: Accept a '>>' if we're in a nested template argument
  // list. Replace the '>>' with a '>' so that the outer list
  // will match.
  Term_list args;
  match(lt_tok);
  if (lookahead() != gt_tok)
    args = template_argument_list();
  match(gt_tok);
  return on_template_id(temp, args);
}


// Parse a template id.
//
//    concept-id:
//      concept-name '< [template-argument-list] '>'
Name&
Parser::concept_id()
{
  Decl& con = concept_name();
  Term_list args;
  match(lt_tok);
  if (lookahead() != gt_tok)
    args = template_argument_list();
  match(gt_tok);
  return on_concept_id(con, args);
}


// Parse a template argument list.
//
//    template-argument-list:
//      template-argument
//      template-argument-list ',' template-argument
//
// FIXME: Implement me.
Term_list
Parser::template_argument_list()
{
  Term_list args;
  do {
    Term& arg = template_argument();
    args.push_back(arg);
  } while(lookahead() == comma_tok);
  return args;
}


// Parse a template argument.
//
//    template-argument:
//      type
//      expression
//      template-name
//
// FIXME: The expression must be a constant expression.
//
// FIXME: In the last instance, the template name can be qualified.
Term&
Parser::template_argument()
{
  if (Type* t = match_if(&Parser::type))
    return *t;
  if (Expr* e = match_if(&Parser::expression))
    return *e;
  if (Decl* d = match_if(&Parser::template_name))
    return *d;
  throw Syntax_error("expected template-argument");
}


// Parse a nested-name specifier.
//
//    nested-name-specifier:
//
// FIXME: Rethink the grammar for nested name specifiers.
Decl&
Parser::nested_name_specifier()
{
  lingo_unimplemented("nested-name-specifier");
  // Decl* scope = &leading_name_specifier();
  // while (true) {
  //   if (Token id = match_if(identifier_tok))
  //     scope = &on_nested_name_specifier(*scope, id);
  //   else if (Name* id = match_if(&Parser::simple_template_id))
  //     scope = &on_nested_name_specifier(*scope, *id);
  //   else
  //     break;
  // }
  // return *scope;
}


// Parse a qualified id.
//
//    qualified-id ::= nested-name-specifier unqualified-id
Name&
Parser::qualified_id()
{
  lingo_unimplemented("qualified-id");

  // FIXME: Actually implement this.
  // Decl& decl = nested_name_specifier();

  // FIXME: This is correct in spirit.
  // Enter_scope scope(decl.scope());

  // Name& id = unqualified_id();
  // return on_qualified_id(scope, id);
}


// -------------------------------------------------------------------------- //
// Resolved names

// Parse a template-name. A template-name is an identifier
// that refers to a template declaration.
//
//    template-name:
//      identifier
Decl&
Parser::template_name()
{
  Token id = match(identifier_tok);
  return on_template_name(id);
}


// Parse a concept-name. A concept-name is an identifier
// that refers to a template declaration.
//
//    concept-name:
//      identifier
Decl&
Parser::concept_name()
{
  Token id = match(identifier_tok);
  return on_concept_name(id);
}


} // namespace banjo
