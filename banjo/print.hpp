// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_PRINT_HPP
#define BANJO_PRINT_HPP

#include "token.hpp"
#include "ast.hpp"

#include <iosfwd>


namespace banjo
{

// A categorization of tokens based on use.
enum Token_use
{
  general_use,
  unary_op_use,
  binary_op_use,
};


// A structure that contains semantic information about the
// use of a token in some context.
struct Token_info
{
  Token_info(Token_kind k, Token_use u = general_use)
    : kind(k), use(u)
  { }

  explicit operator bool() const { return kind != error_tok; }
  operator Token_kind() const    { return kind; }
  operator Token_use() const     { return use; }

  void clear()
  {
    kind = error_tok;
    use = general_use;
  }

  Token_kind kind;
  Token_use  use;
};


struct Printer
{
  Printer(std::ostream& os)
    : os(os), indent(0), prev(error_tok)
  { }

  void operator()(Name const& n) { id(n); }
  void operator()(Type const& t) { type(t); }
  void operator()(Expr const& e) { expression(e); }
  void operator()(Stmt const& s) { statement(s); }
  void operator()(Decl const& d) { declaration(d); }

  // Lexical terms.
  void space(Token_info);
  void space();
  void newline();
  void newline_and_indent();
  void newline_and_undent();
  void token(Token_kind);
  void token(Token_kind, Token_use);
  void token(Symbol const&);
  void token(char const*);
  void token(String const&);

  // Unresolved names
  void id(Name const&);
  void unqualified_id(Name const&);
  void unqualified_id(Simple_id const&);
  void destructor_id(Destructor_id const&);
  void operator_id(Operator_id const&);
  void conversion_id(Conversion_id const&);
  void literal_id(Literal_id const&);
  void template_id(Template_id const&);
  void qualified_id(Qualified_id const&);
  void nested_name_specifier(Decl const&);

  // Types
  void type(Type const&);
  void simple_type(Type const&);
  void simple_type(Void_type const&);
  void simple_type(Boolean_type const&);
  void simple_type(Integer_type const&);
  void simple_type(Float_type const&);
  void simple_type(Auto_type const&);
  void simple_type(Decltype_type const&);
  void simple_type(Declauto_type const&);
  void simple_type(Function_type const&);
  void simple_type(Typename_type const&);
  void grouped_type(Type const&, Type const&);
  void postfix_type(Pointer_type const&);
  void postfix_type(Qualified_type const&);
  void postfix_type(Array_type const&);
  void sequence_type(Sequence_type const&);
  void reference_type(Reference_type const&);
  void return_type(Type const&);

  // Expressions
  void expression(Expr const&);
  void literal(Boolean_expr const&);
  void literal(Integer_expr const&);
  void literal(Real_expr const&);
  void id_expression(Reference_expr const&);
  void grouped_expression(Expr const&, Expr const&);
  void postfix_expression(Call_expr const&);
  void postfix_expression(Value_conv const&);
  void postfix_expression(Qualification_conv const&);
  void postfix_expression(Boolean_conv const&);
  void postfix_expression(Integer_conv const&);
  void postfix_expression(Float_conv const&);
  void postfix_expression(Numeric_conv const&);
  void postfix_expression(Ellipsis_conv const&);
  void unary_expression(Unary_expr const&, Token_kind);
  void binary_expression(Binary_expr const&, Token_kind);

  // Statements
  void statement(Stmt const&);
  void expression_statement(Expression_stmt const&);
  void declaration_statement(Declaration_stmt const&);
  void compound_statement(Compound_stmt const&);

  // Initializers
  void initializer(Expr const&);
  void initializer(Init const&);
  void equal_initializer(Copy_init const&);
  void equal_initializer(Bind_init const&);
  void paren_initializer(Direct_init const&);
  void brace_initializer(Aggregate_init const&);

  // Definitions
  void function_definition(Def const&);
  void function_definition(Function_def const&);
  void function_definition(Deleted_def const&);
  void function_definition(Defaulted_def const&);

  // Declarations
  void declaration(Decl const&);
  void declaration_seq(Decl_list const&);
  void variable_declaration(Variable_decl const&);
  void constant_declaration(Constant_decl const&);
  void function_declaration(Function_decl const&);
  void class_declaration(Class_decl const&);
  void union_declaration(Union_decl const&);
  void enum_declaration(Enum_decl const&);
  void namespace_declaration(Namespace_decl const&);
  void template_declaration(Template_decl const&);

  void parameter(Decl const&);
  void parameter(Object_parm const&);
  void parameter(Variadic_parm const&);
  void parameter_list(Decl_list const&);

  void type_template_parameter(Type_parm const&);
  void value_template_parameter(Value_parm const&);
  void template_template_parameter(Template_parm const&);
  void template_parameter(Decl const&);
  void template_parameter_list(Decl_list const&);
  void template_argument(Term const&);

  void requires_clause(Expr const&);

  std::ostream& os;
  int           indent; // The current indentation
  Token_info    prev;   // The previous token printed

};


std::ostream& operator<<(std::ostream&, Term const&);
std::ostream& operator<<(std::ostream&, Name const&);
std::ostream& operator<<(std::ostream&, Type const&);
std::ostream& operator<<(std::ostream&, Expr const&);
std::ostream& operator<<(std::ostream&, Stmt const&);
std::ostream& operator<<(std::ostream&, Decl const&);


} // namespace banjo

#endif
