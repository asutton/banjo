// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PARSER_HPP
#define BEAKER_PARSER_HPP

#include "lexer.hpp"
#include "ast.hpp"


namespace beaker
{

// Denotes a syntactic error.
struct Syntax_error : std::runtime_error
{
  using std::runtime_error::runtime_error;
};


// The parser is responsible for transforming a stream of tokens
// into nodes. The parser owns a reference to the buffer for its
// tokens. This supports the resolution of source code locations.
struct Parser
{
  Parser(Token_stream& ts)
    : tokens(ts), state()
  { }

  Term* operator()();

  // Syntax
  // Unresolved names
  Name* id();
  Name* unqualified_id();
  Name* destructor_id();
  Name* operator_id();
  Name* conversion_id();
  Name* literal_id();
  Name* template_id();
  Name* qualified_id();

  // Name helpers
  Name* simple_template_id();
  Term_list template_argument_list();

  // Nested name specifiers
  Name* leading_name_specifier();
  Name* nested_name_specifier();
  Name* nested_name_specifier_opt();

  // Resolved names
  Type* class_name();
  Type* union_name();
  Type* enum_name();
  Type* type_name();
  Type* type_alias();
  Decl* namespace_name();
  Decl* namespace_alias();
  Decl* template_name();

  // Types
  Type* type();
  Type* simple_type();
  Type* decltype_type();
  Type* function_type();
  Type* grouped_type();
  Type* qualified_type();

  // Type helpers
  Type* return_type();
  Type_list type_list();

  // Expressions
  Expr* id_expression();
  Expr* grouped_expression();
  Expr* lambda_expression();
  Expr* primary_expression();
  Expr* postfix_expression();
  Expr* unary_expression();
  Expr* multiplicative_expression();
  Expr* additive_expression();
  Expr* binary_expression();
  Expr* expression();

  // Declarations
  Name* declarator();
  Decl* declaration();
  Decl* empty_declaration();
  Decl* variable_declaration();
  Decl* function_declaration();
  Decl* parameter_declaration();
  Decl* namespace_declaration();
  Decl* class_declaration();
  Decl* enum_declaration();
  Decl* template_declaration();
  Decl_list declaration_seq();

  // Initializers
  Init* initializer();
  Init* value_initializer();
  Init* direct_initializer();
  Init* aggregate_initializer();

  Term* translation_unit();

  // Semantics
  // Names
  Name* on_simple_id(Token);
  Name* on_destructor_id(Token, Type*);
  Name* on_operator_id();
  Name* on_conversion_id();
  Name* on_literal_id();
  Name* on_template_id(Token, Decl*, Term_list const&);
  Name* on_qualified_id(Name*, Name*);

  Term_list on_template_argument_list();
  Name* on_nested_name_specifier(Token);
  Name* on_nested_name_specifier(Decl*);
  Name* on_nested_name_specifier(Type*);
  Name* on_nested_name_specifier(Name*, Token);
  Name* on_nested_name_specifier(Name*, Name*);

  Type* on_class_name(Token);
  Type* on_class_name(Name*);
  Type* on_union_name(Token);
  Type* on_union_name(Name*);
  Type* on_enum_name(Token);
  Type* on_enum_name(Name*);
  Type* on_type_alias(Token);
  Type* on_type_alias(Name*);
  Decl* on_namespace_name(Token);
  Decl* on_namespace_name(Name*);
  Decl* on_namespace_alias(Token);
  Decl* on_namespace_alias(Name*);
  Decl* on_template_name(Token);

  Type* on_simple_type(Token);
  Type* on_decltype_type(Token, Expr*);
  Type* on_function_type(Type_list const&, Type*);
  Type* on_pointer_type(Token, Type*);
  Type* on_reference_type(Token, Type*);
  Type_list on_type_list();

  // Expressions
  Expr* on_id_expression(Name*);
  Expr* on_integer_literal(Token);

  // Declarations
  Decl* on_variable_declaration(Token, Name*, Type*, Init*);
  Decl* on_function_declaration(Token, Name*, Decl_list const&, Type*, Init*);
  Decl* on_parameter_declaration(Name*, Type*, Init*);
  Decl* on_namespace_declaration(Token, Name*, Decl_list const&);
  Decl_list on_declaration_seq();

  Name* on_declarator(Name*);
  Init* on_default_initializer();
  Init* on_value_initializer(Expr*);
  Init* on_direct_initializer(Expr_list const&);
  Init* on_aggregate_initializer(Expr_list const&);

  // Miscellaneous
  Term* start_translation_unit();
  Term* finish_translation_unit();

  // Token matching.
  Token      peek() const;
  Token_kind lookahead() const;
  Token_kind lookahead(int) const;
  Token      match(Token_kind);
  Token      match_if(Token_kind);
  Token      require(Token_kind);
  Token      accept();

  // Tree matching.
  template<typename T> T* match_if(T* (Parser::* p)());

  // Maintains the current parse state.
  struct State
  {
    // True if parsing a declarator.
    bool parsing_declarator = false;

    // True if the following term is a type.
    bool assume_typename = false;

    // True if the next identifier is a template.
    bool assume_template = false;
  };

  struct Assume_template;

  Token_stream& tokens;
  State         state;
};


// An RAII helper that sets or clears the flag controlling
// whether or not the 'template' keyword was seen before
// a template-name.
struct Parser::Assume_template
{
  Assume_template(Parser& p, bool b)
    : parser(p), saved(p.state.assume_template)
  {
    parser.state.assume_template = b;
  }

  ~Assume_template()
  {
    parser.state.assume_template = saved;
  }

  Parser& parser;
  bool    saved;
};


// The trial parser provides recovery information for the parser
// class. If the trial parse fails, then the state of the underlying
// parser is rewound to the state cached bythe trial parser.
//
// TODO: Can we automatically detect failures without needing
// an explicit indication of failure?
struct Trial_parser
{
  using Position = Token_stream::Position;
  using State = Parser::State;

  Trial_parser(Parser& p)
    : parser(p), pos(p.tokens.position()), state(p.state), fail(false)
  { }

  void failed() { fail = true; }

  ~Trial_parser()
  {
    // TODO: Manage diagnostics as part of the parser in order to
    // detet failures?
    if (fail) {
      parser.state = state;
      parser.tokens.reposition(pos);
    }
  }

  Parser&  parser;
  Position pos;
  State    state;
  bool     fail;
};


// -------------------------------------------------------------------------- //
// Implementation

// Match a given tree.
template<typename R>
inline R*
Parser::match_if(R* (Parser::* f)())
{
  Trial_parser p(*this);
  try {
    return (this->*f)();
  } catch(Syntax_error&) {
    p.failed();
  }
  return nullptr;
}


} // nammespace beaker


#endif
