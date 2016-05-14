// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_PARSER_HPP
#define BANJO_PARSER_HPP

#include "lexer.hpp"
#include "token.hpp"
#include "scope.hpp"
#include "language.hpp"
#include "context.hpp"


namespace banjo
{

// Maintains a stack of braces. Note that "braces" is meant to imply
// any kind of bracketing characters.
struct Braces : Token_seq
{
  void open(Token tok) { push_back(tok); }
  void close()         { pop_back(); }
};


// The parser is responsible for transforming a stream of tokens
// into nodes. The parser owns a reference to the buffer for its
// tokens. This supports the resolution of source code locations.
struct Parser
{
  using Specs = Specifier_set; // For brevity

  Parser(Context& cxt, Token_stream& ts)
    : cxt(cxt), build(cxt), tokens(ts), state()
  { }

  Stmt& operator()();

  // Syntactic forms
  Operator_kind any_operator();

  // Syntax

  // Declaration names
  Name& identifier();

  // Unresolved names
  Name& id();
  Name& unqualified_id();
  Name& destructor_id();
  Name& operator_id();
  Name& conversion_id();
  Name& literal_id();
  Name& template_id();
  Name& concept_id();
  Name& qualified_id();

  // Name helpers
  Term_list template_argument_list();
  Term& template_argument();

  // Nested name specifiers
  Decl& nested_name_specifier();

  // Resolved names
  Decl& template_name();
  Decl& concept_name();

  // Types
  Type& type();
  Type& suffix_type();
  Type& prefix_type();
  Type& unary_type();
  Type& postfix_type();
  Type& array_type(Type&);
  Type& tuple_type();
  Type& primary_type();
  Type& id_type();
  Type& grouped_type();
  Type& function_type();
  Type& decltype_type();
  Type_list type_list();

  // Expressions
  Expr& expression();
  Expr& logical_or_expression();
  Expr& logical_and_expression();
  Expr& inclusive_or_expression();
  Expr& exclusive_or_expression();
  Expr& and_expression();
  Expr& equality_expression();
  Expr& relational_expression();
  Expr& shift_expression();
  Expr& additive_expression();
  Expr& multiplicative_expression();
  Expr& unary_expression();
  Expr& postfix_expression();
  Expr& call_expression(Expr&);
  Expr& tuple_expression();
  Expr& dot_expression(Expr&);
  Expr& subscript_expression(Expr&);
  Expr& primary_expression();
  Expr& id_expression();
  Expr& grouped_expression();
  Expr& lambda_expression();
  Expr& requires_expression();
  Expr_list expression_list();

  // Statements
  Stmt& statement();
  Stmt& compound_statement();
  Stmt& empty_statement();
  Stmt& return_statement();
  Stmt& yield_statement();
  Stmt& if_statement();
  Stmt& while_statement();
  Stmt& for_statement();
  Stmt& break_statement();
  Stmt& continue_statement();
  Stmt& declaration_statement();
  Stmt& expression_statement();
  Stmt_list statement_seq();

  // Declarations
  Name& declarator();
  Decl& declaration();
  Decl_list declaration_seq();
  Decl& empty_declaration();

  // Specifiers
  Specifier_set specifier_seq();
  Specifier_set parameter_specifier_seq();

  // Variables
  Decl& variable_declaration();
  Decl& super_declaration();
  Type& unparsed_variable_type();
  Expr& unparsed_variable_initializer();
  Expr& initializer(Decl&);
  
  // Functions
  Decl& function_declaration();
  Decl& coroutine_declaration();
  Decl_list parameter_clause();
  Decl_list parameter_list();
  Decl& parameter_declaration();
  Type& unparsed_parameter_type();
  Type& unparsed_return_type();
  Expr& unparsed_expression_body();

  // Classes
  Decl& class_declaration();
  Def& class_body();
  Stmt_list member_statement_seq();
  Stmt& member_statement();

  // Templates
  Decl& template_declaration();
  Decl& template_parameter();
  Decl& type_template_parameter();
  Decl& value_template_parameter();
  Decl& template_template_parameter();
  Decl_list template_parameter_list();

  // Constraints, preconditions, and postconditions
  Expr& requires_clause();

  // Concepts
  Decl& concept_declaration();
  Def& concept_definition(Decl&);
  Req& concept_member();
  Req_list concept_member_seq();

  // Requirements
  Req& requirement();
  Req& type_requirement();
  Req& syntactic_requirement();
  Req& expression_requirement();
  Req& usage_requirement();
  Req_list usage_seq();

  // Toplevel structure
  Stmt& translation_unit();
  Stmt_list toplevel_statement_seq();
  Stmt& toplevel_statement();

  // Unparsed terms
  template<typename P> 
  Type& unparsed_type(P);
  
  template<typename P>
  Expr& unparsed_expression(P);


  // Type elaboration
  void elaborate_declarations(Stmt_list&);
  void elaborate_declaration(Stmt&);
  void elaborate_declaration(Decl&);
  void elaborate_variable_declaration(Variable_decl&);
  void elaborate_super_declaration(Super_decl&);
  void elaborate_function_declaration(Function_decl&);
  void elaborate_parameter_declaration(Object_parm&);
  void elaborate_class_declaration(Class_decl&);
  void elaborate_coroutine_declaration(Coroutine_decl&);
  Type& elaborate_type(Type&);


  // Semantics actions

  // Identifiers
  Name& on_simple_id(Token);
  Name& on_destructor_id(Token, Type&);
  Name& on_operator_id(Token, Operator_kind);
  Name& on_conversion_id();
  Name& on_literal_id();
  Name& on_template_id(Decl&, Term_list const&);
  Name& on_concept_id(Decl&, Term_list const&);
  Name& on_qualified_id(Decl&, Name&);

  // Names
  Decl& on_template_name(Token);
  Decl& on_concept_name(Token);

  // Types
  Type& on_class_type(Token);
  Type& on_void_type(Token);
  Type& on_bool_type(Token);
  Type& on_int_type(Token);
  Type& on_byte_type(Token);
  Type& on_id_type(Name&);
  Type& on_decltype_type(Token, Expr&);
  Type& on_function_type(Type_list&, Type&);
  Type& on_slice_type(Type&);
  Type& on_pointer_type(Type&);
  Type& on_const_type(Type&);
  Type& on_volatile_type(Type&);
  Type& on_reference_type(Type&);
  Type& on_pack_type(Type&);
  Type& on_unparsed_type(Token_seq&&);
  Type& on_array_type(Type&, Expr&);
  Type& on_tuple_type(Type_list&);
  Type& on_dynarray_type(Type&, Expr&);

  // Expressions
  Expr& on_logical_and_expression(Token, Expr&, Expr&);
  Expr& on_logical_or_expression(Token, Expr&, Expr&);
  Expr& on_logical_not_expression(Token, Expr&);
  Expr& on_or_expression(Token, Expr&, Expr&);
  Expr& on_xor_expression(Token, Expr&, Expr&);
  Expr& on_and_expression(Token, Expr&, Expr&);
  Expr& on_lsh_expression(Token, Expr&, Expr&);
  Expr& on_rsh_expression(Token, Expr&, Expr&);
  Expr& on_compl_expression(Token, Expr&);
  Expr& on_eq_expression(Token, Expr&, Expr&);
  Expr& on_ne_expression(Token, Expr&, Expr&);
  Expr& on_lt_expression(Token, Expr&, Expr&);
  Expr& on_gt_expression(Token, Expr&, Expr&);
  Expr& on_le_expression(Token, Expr&, Expr&);
  Expr& on_ge_expression(Token, Expr&, Expr&);
  Expr& on_cmp_expression(Token, Expr&, Expr&);
  Expr& on_add_expression(Token, Expr&, Expr&);
  Expr& on_sub_expression(Token, Expr&, Expr&);
  Expr& on_mul_expression(Token, Expr&, Expr&);
  Expr& on_div_expression(Token, Expr&, Expr&);
  Expr& on_rem_expression(Token, Expr&, Expr&);
  Expr& on_neg_expression(Token, Expr&);
  Expr& on_pos_expression(Token, Expr&);
  Expr& on_call_expression(Expr&, Expr_list&);
  Expr& on_tuple_expression(Expr_list&);
  Expr& on_dot_expression(Expr&, Name&);
  Expr& on_id_expression(Name&);
  Expr& on_boolean_literal(Token, bool);
  Expr& on_integer_literal(Token);
  Expr& on_requires_expression(Token, Decl_list&, Decl_list&, Req_list&);

  Expr& on_unparsed_expression(Token_seq&&);

  // Statements
  Stmt& on_compound_statement(Stmt_list&&);
  Stmt& on_empty_statement();
  Stmt& on_return_statement(Token, Expr&);
  Stmt& on_yield_statement(Token, Expr&);
  Stmt& on_if_statement(Expr&, Stmt&);
  Stmt& on_if_statement(Expr&, Stmt&, Stmt&);
  Stmt& on_while_statement(Expr&, Stmt&);
  Stmt& on_break_statement();
  Stmt& on_continue_statement();
  Stmt& on_declaration_statement(Decl&);
  Stmt& on_expression_statement(Expr&);
  Stmt& on_unparsed_statement(Token_seq&&);
  void on_statement_seq(Stmt_list&);


  // Variables
  Decl& on_variable_declaration(Name&, Type&);
  Decl& on_variable_declaration(Name&, Type&, Expr&);
  Decl& on_super_declaration(Name&, Type&);

  // Functions
  Decl& on_function_declaration(Name&, Decl_list&, Type&, Expr&);
  Decl& on_function_declaration(Name&, Decl_list&, Type&, Stmt&);

  // Classes
  Decl& on_class_declaration(Name&, Type&);
  Decl& on_class_definition(Decl&, Def&);
  Def& on_class_body(Stmt_list&&);

  // Coroutine declarations
  Decl& on_coroutine_declaration(Name&, Decl_list&, Type&, Stmt&);

  // Concept declarations
  Decl& on_concept_declaration(Token, Name&, Decl_list&);

  // Function parameters
  Decl& on_function_parameter(Name&, Type&);

  // Template parameters
  Decl& on_type_template_parameter(Name&, Type&);
  Decl& on_type_template_parameter(Name&);

  // Initializers
  Expr& on_default_initialization(Decl&);
  Expr& on_equal_initialization(Decl&, Expr&);
  Expr& on_paren_initialization(Decl&, Expr_list&);
  Expr& on_brace_initialization(Decl&, Expr_list&);

  // Definitions
  Def& on_function_definition(Decl&, Stmt&);
  Def& on_concept_definition(Decl&, Expr&);
  Def& on_concept_definition(Decl&, Req_list&);
  Def& on_deleted_definition(Decl&);
  Def& on_defaulted_definition(Decl&);

  // Requirements
  Req& on_type_requirement(Expr&);
  Req& on_syntactic_requirement(Expr&);
  Req& on_semantic_requirement(Decl&);
  Req& on_expression_requirement(Expr&);
  Req& on_basic_requirement(Expr&);
  Req& on_basic_requirement(Expr&, Type&);
  Req& on_conversion_requirement(Expr&, Type&);
  Req& on_deduction_requirement(Expr&, Type&);

  // Toplevel structure
  Stmt& on_translation_unit(Stmt_list&&);


  // Token matching.
  Token      peek() const;
  Token_kind lookahead() const;
  Token_kind lookahead(int) const;
  bool       is_eof() const;
  bool       next_token_is(Token_kind);
  bool       next_token_is(char const*);
  bool       next_token_is_not(Token_kind);
  bool       next_token_is_not(char const*);
  Token      match(Token_kind);
  Token      match_if(Token_kind);
  Token      require(Token_kind);
  Token      require(char const*);
  void       expect(Token_kind);
  Token      accept();

  template<typename... Kinds>
  bool next_token_is_one_of(Token_kind, Kinds...);

  bool next_token_is_one_of();

  // Braces
  void open_brace(Token);
  void close_brace(Token);
  bool in_braces() const;
  bool in_level(int) const;
  int  brace_level() const;

  // Specifiers
  Specs  decl_specs() const { return state.specs; }
  Specs& decl_specs()       { return state.specs; }
  Specs  take_decl_specs();
  void   clear_decl_specs();

  // Tree matching.
  template<typename T> T* match_if(T& (Parser::* p)());

  // Resources
  Symbol_table& symbols();
  Context&      context();

  // Scope management
  Scope& current_scope();


  // Maintains the current parse state. This is used to provide context for
  // various parsing routines, and is used by the trial parser for caching
  // and restoring parse state.
  struct State
  {
    Braces  braces; // Current nesting level
    Specs   specs;  // Parsed specifiers

    Decl_list implicit_parms; // Implicit template parameters
  };

  struct Parsing_template;

  Context&      cxt;
  Builder       build;
  Token_stream& tokens;
  State         state;
};


template<typename... Kinds>
inline bool
Parser::next_token_is_one_of(Token_kind k, Kinds... ks)
{
  if (next_token_is(k))
    return true;
  else
    return next_token_is_one_of(ks...);
}


inline bool
Parser::next_token_is_one_of()
{
  return false;
}


// Return the current specifiers, resetting them to their default value.
inline Parser::Specs
Parser::take_decl_specs()
{
  Specs s = decl_specs();
  decl_specs() = {};
  return s;
}


// Reset declaration specifiers.
inline void
Parser::clear_decl_specs()
{
  decl_specs() = {};
}


// This class defines a predicate that can be tested to determine if the
// current token is in the same nesting level as when this object is
// constructed.
struct Match_braces
{
  Match_braces(Parser& p)
    : parser(p), level(p.brace_level())
  { }

  bool operator()() const
  {
    return parser.in_level(level);
  }

  Parser& parser;
  int     level;
};


struct Match_token_pred
{
  Match_token_pred(Parser& p, Token_kind k)
    : p(p), k(k)
  { }

  bool operator()() const
  {
    return p.next_token_is(k);
  }

  Parser&    p;
  Token_kind k;
};


// TODO: Handle the 4 case.
struct Match_any_token_pred
{
  Match_any_token_pred(Parser& p, Token_kind k1, Token_kind k2)
    : p(p), n(2), k1(k1), k2(k2)
  { 
  }

  Match_any_token_pred(Parser& p, Token_kind k1, Token_kind k2, Token_kind k3)
    : p(p), n(2), k1(k1), k2(k2), k3(k3)
  { 
  }

  bool operator()() const
  {
    if (n == 2)
      return p.next_token_is_one_of(k1, k2);
    if (n == 3)
      return p.next_token_is_one_of(k1, k2, k3);
    lingo_unreachable();
  }

  Parser&    p;
  int        n;
  Token_kind k1, k2, k3;
};


// Consume tokens until we reach the current token satisfied pred
// at the same level brace nesting level.
template<typename P>
inline Type&
Parser::unparsed_type(P pred)
{
  Token_seq toks;
  Match_braces is_non_nested(*this);
  while (!is_eof()) {
    if (pred() && is_non_nested())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_type(std::move(toks));
}


// Consume tokens until we reach the current token satisfied pred
// at the same level brace nesting level.
template<typename P>
inline Expr&
Parser::unparsed_expression(P pred)
{
  Token_seq toks;
  Match_braces is_non_nested(*this);
  while (!is_eof()) {
    if (pred() && is_non_nested())
      break;
    toks.push_back(accept());
  }
  return on_unparsed_expression(std::move(toks));
}


// The trial parser provides recovery information for the parser
// class. If the trial parse fails, then the state of the underlying
// parser is rewound to the state cached by the trial parser.
//
// TODO: Can we automatically detect failures without needing
// an explicit indication of failure?
struct Trial_parser
{
  using Position = Token_stream::Position;
  using State = Parser::State;

  Trial_parser(Parser& p)
    : parser(p)
    , pos(p.tokens.position())
    , state(p.state)
    , scope(&p.current_scope())
    , fail(false)
  { }

  void failed() { fail = true; }

  ~Trial_parser()
  {
    // TODO: Manage diagnostics as part of the parser in order to
    // detet failures?
    if (fail) {
      parser.tokens.reposition(pos);
      parser.cxt.set_scope(*scope);
      parser.state = state;
    }
  }

  Parser&  parser;
  Position pos;
  State    state;
  Scope*   scope;
  bool     fail;
};


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
