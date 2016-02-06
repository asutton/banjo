// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <banjo/context.hpp>
#include <banjo/lexer.hpp>
#include <banjo/parser.hpp>
#include <banjo/template.hpp>
#include <banjo/normalization.hpp>
#include <banjo/satisfaction.hpp>
#include <banjo/subsumption.hpp>

#include <lingo/file.hpp>
#include <lingo/io.hpp>
#include <lingo/error.hpp>

#include <iostream>
#include <iomanip>


void directive_seq(Parser&);
void type_directive(Parser&);
void resolve_directive(Parser&);
void instantiate_directive(Parser&);
void satisfy_directive(Parser&);
void order_directive(Parser&);
void order_concept_directive(Parser&);
void order_template_directive(Parser&);


// This tool parses a sequence of declarations followed by a
// sequence of directives. Each directive computes some operation
// on the declarations previously parsed.
//
//    script:
//      translation unit
//      directive-seq
int
main(int argc, char* argv[])
{
  Context cxt;

  if (argc != 2) {
    std::cerr << "usage: test_parse <input-file>\n";
    return -1;
  }

  File input(argv[1]);
  Character_stream cs(input);
  Token_stream ts(input);
  Lexer lex(cxt, cs, ts);
  Parser parse(cxt, ts);

  // Transform characters into tokens.
  lex();
  if (error_count())
    return 1;

  // Parse the translation unit.
  parse();
  if (error_count())
    return 1;

  // Parse and interpret directives.
  directive_seq(parse);
  return 0;
}


// Parse a directive sequence.
//
//    directive-seq
//      'type' expression
//      'resolve' postscript-expression
//      'instantiate' template-id
//      'satisfy' check-expr
//      'order' '.' 'template' template-id template-id
//      'order' '.' 'concept' concept-id concept-id
void
directive_seq(Parser& p)
{
  // Parse the directive sequence as if in the global namespace.
  Parser::Enter_scope scope(p, p.cxt.global_namespace());
  while (p.peek()) {
    if (p.next_token_is("type"))
      type_directive(p);
    if (p.next_token_is("resolve"))
      resolve_directive(p);
    if (p.next_token_is("instantiate"))
      instantiate_directive(p);
    if (p.next_token_is("satisfy"))
      satisfy_directive(p);
    if (p.next_token_is("order"))
      order_directive(p);
  }
}


void
type_directive(Parser& p)
{
  p.require("type");
  Expr& e = p.expression();
  p.match(semicolon_tok);
  std::cout << e.type() << '\n';
}


void
resolve_directive(Parser& p)
{
  p.require("reolve");
  Expr& e = p.postfix_expression();
  std::cout << e << '\n';

  // TODO: Actually show which function was resolved.
  lingo_unimplemented();
}

void
instantiate_directive(Parser& p)
{
  p.require("instantiate");
  // Name& n = p.template_id();
  // Actually show the instantated definition.
  lingo_unimplemented();
}


void
satisfy_directive(Parser& p)
{
  p.require("satisfy");
  Expr& e = p.primary_expression();
  p.match(semicolon_tok);

  if (!has_bool_type(e)) {
    error("'{}' does not have type bool");
    return;
  }

  // TODO: Can I realistically make any assumptions about
  // what e can and cannot be? It must be a logical
  // proposition.

  // Test for satisfaction.
  bool result = is_satisfied(p.cxt, e);
  std::cout << std::boolalpha << result << '\n';
}


void
order_directive(Parser& p)
{
  p.require("order");
  p.match(dot_tok);
  if (p.next_token_is(concept_tok))
    return order_concept_directive(p);
  if (p.next_token_is(template_tok))
    return order_template_directive(p);

  // FIXME: Be a little more polite.
  lingo_unreachable();
}


void
order_concept_directive(Parser& p)
{
  p.require(concept_tok);
  Concept_decl& c1 = cast<Concept_decl>(p.concept_name());
  Concept_decl& c2 = cast<Concept_decl>(p.concept_name());
  p.match(semicolon_tok);

  // TODO: Determine which subsumes the other by synthesizing
  // arguments to create check (and validate) check expressions,
  // normalize those, and then run the subsumption algorithm.
  Context& cxt = p.cxt;
  Builder build(cxt);

  Term_list a1 = synthesize_template_arguments(cxt, c1.parameters());
  Term_list a2 = synthesize_template_arguments(cxt, c2.parameters());

  Cons& con1 = build.get_concept_constraint(c1, a1);
  Cons& con2 = build.get_concept_constraint(c2, a2);
  bool result = subsumes(cxt, con1, con2);
  std::cout << std::boolalpha << result << '\n';
}


// FIXME: How do I compare two templates with the same name?
// Just give them different names?
void
order_template_directive(Parser& p)
{
  // p.require(template_tok);
  // Name& n1 = p.template_id();
  // Name& n2 = p.template_id();

  // TODO: Resolve the function templates referred to by these
  // ids and determine which is more specialized.
  lingo_unimplemented();
}
