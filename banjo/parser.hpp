// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_PARSER_HPP
#define BANJO_PARSER_HPP

#include "lexer.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "scope.hpp"
#include "builder.hpp"


namespace banjo
{

// The parser is responsible for transforming a stream of tokens
// into nodes. The parser owns a reference to the buffer for its
// tokens. This supports the resolution of source code locations.
struct Parser
{
  Parser(Context& cxt, Token_stream& ts)
    : cxt(cxt), build(cxt), tokens(ts), state()
  { }

  Term& operator()();

  // Syntax
  // Unresolved names
  Name& id();
  Name& unqualified_id();
  Name& destructor_id();
  Name& operator_id();
  Name& conversion_id();
  Name& literal_id();
  Name& template_id();
  Name& qualified_id();

  // Name helpers
  Name& simple_template_id();
  Term_list template_argument_list();

  // Nested name specifiers
  Decl& leading_name_specifier();
  Decl& nested_name_specifier();
  Decl* nested_name_specifier_opt();

  // Resolved names
  Type& class_name();
  Type& union_name();
  Type& enum_name();
  Type& type_name();
  Type& type_alias();
  Decl& namespace_name();
  Decl& namespace_alias();
  Decl& template_name();

  // Types
  Type& type();
  Type& simple_type();
  Type& decltype_type();
  Type& function_type();
  Type& grouped_type();
  Type& postfix_type();
  Type& sequence_type();
  Type& reference_type();

  // Type helpers
  Type& return_type();
  Type_list type_list();

  // Expressions
  Expr& id_expression();
  Expr& grouped_expression();
  Expr& lambda_expression();
  Expr& primary_expression();
  Expr& postfix_expression();
  Expr& unary_expression();
  Expr& multiplicative_expression();
  Expr& additive_expression();
  Expr& binary_expression();
  Expr& expression();

  // Declarations
  Name& declarator();
  Decl& declaration();
  Decl& empty_declaration();
  Decl& variable_declaration();
  Decl& function_declaration();
  Decl& parameter_declaration();
  Decl& class_declaration();
  Decl& enum_declaration();
  Decl& namespace_declaration();
  Decl& template_declaration();

  Decl_list parameter_list();
  Def& function_definition();

  Decl& type_template_parameter();
  Decl& value_template_parameter();
  Decl& template_template_parameter();
  Decl& template_parameter();
  Decl_list template_parameter_list();
  Decl_list declaration_seq();

  // Initializers
  Expr& initializer(Decl&);
  Expr& equal_initializer(Decl&);
  Expr& paren_initializer(Decl&);
  Expr& brace_initializer(Decl&);

  Term& translation_unit();

  // Semantics

  // Identifiers
  Name& on_simple_id(Token);
  Name& on_destructor_id(Token, Type&);
  Name& on_operator_id();
  Name& on_conversion_id();
  Name& on_literal_id();
  Name& on_template_id(Token, Decl&, Term_list const&);
  Name& on_qualified_id(Decl&, Name&);

  Decl& on_nested_name_specifier();
  Decl& on_nested_name_specifier(Decl&);
  Decl& on_nested_name_specifier(Type&);
  Decl& on_nested_name_specifier(Decl&, Token);
  Decl& on_nested_name_specifier(Decl&, Name&);

  // Names
  Type& on_class_name(Token);
  Type& on_class_name(Name&);
  Type& on_union_name(Token);
  Type& on_union_name(Name&);
  Type& on_enum_name(Token);
  Type& on_enum_name(Name&);
  Type& on_type_alias(Token);
  Type& on_type_alias(Name&);
  Decl& on_namespace_name(Token);
  Decl& on_namespace_name(Name&);
  Decl& on_namespace_alias(Token);
  Decl& on_namespace_alias(Name&);
  Decl& on_template_name(Token);

  // Types
  Type& on_void_type(Token);
  Type& on_bool_type(Token);
  Type& on_int_type(Token);
  Type& on_decltype_type(Token, Expr&);
  Type& on_function_type(Type_list&, Type&);
  Type& on_pointer_type(Token, Type&);
  Type& on_qualified_type(Token, Type&, Qualifier_set);
  Type& on_const_type(Token, Type&);
  Type& on_volatile_type(Token, Type&);
  Type& on_sequence_type(Type&);
  Type& on_reference_type(Token, Type&);

  // Expressions
  Expr& on_id_expression(Name&);
  Expr& on_boolean_literal(Token, bool);
  Expr& on_integer_literal(Token);

  // Declarations
  Variable_decl& on_variable_declaration(Token, Name&, Type&);
  Function_decl& on_function_declaration(Token, Name&, Decl_list&, Type&);
  Namespace_decl& on_namespace_declaration(Token, Name&, Decl_list&);

  Object_parm& on_function_parameter(Name&, Type&);

  Decl_list on_declaration_seq();

  Name& on_declarator(Name&);
  Expr& on_default_initialization(Decl&);
  Expr& on_equal_initialization(Decl&, Expr&);
  Expr& on_paren_initialization(Decl&, Expr_list&);
  Expr& on_brace_initialization(Decl&, Expr_list&);

  // Token matching.
  Token      peek() const;
  Token_kind lookahead() const;
  Token_kind lookahead(int) const;
  Token      match(Token_kind);
  Token      match_if(Token_kind);
  Token      require(Token_kind);
  Token      accept();

  // Tree matching.
  template<typename T> T* match_if(T& (Parser::* p)());

  // Scope management
  void   enter_scope(Scope&);
  Scope& current_scope();
  Decl&  current_context();

  // Maintains the current parse state.
  struct State
  {
    // True if parsing a declarator.
    bool parsing_declarator = false;

    // True if the following term is a type.
    bool assume_typename = false;

    // True if the next identifier is a template.
    bool assume_template = false;

    // The current scope.
    Scope* scope;
  };

  struct Enter_scope;
  struct Assume_template;

  Context&      cxt;
  Builder       build;
  Token_stream& tokens;
  State         state;
};


// An RAII helper that manages the entry and exit of scopes.
//
// TODO: Handle scopes for more declarations.
struct Parser::Enter_scope
{
  Enter_scope(Parser& p, Namespace_decl& ns)
    : parser(p), prev(p.state.scope), alloc(nullptr)
  {
    parser.enter_scope(*ns.scope());
  }

  Enter_scope(Parser& p, Variable_decl& var)
    : parser(p), prev(p.state.scope), alloc(new Initializer_scope(*prev, var))
  {
    parser.enter_scope(*alloc);
  }

  ~Enter_scope()
  {
    parser.enter_scope(*prev);
    delete alloc;
  }

  Parser& parser;
  Scope* prev;  // The previous socpe.
  Scope* alloc; // Only set when locally allocated.
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
Parser::match_if(R& (Parser::* f)())
{
  Trial_parser p(*this);
  try {
    return &(this->*f)();
  } catch(Translation_error&) {
    p.failed();
  }
  return nullptr;
}


} // nammespace banjo


#endif
