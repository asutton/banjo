// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_PRINTER_HPP
#define BANJO_PRINTER_HPP

#include "token.hpp"
#include "language.hpp"

#include <iosfwd>


namespace banjo
{

struct Printer
{
  Printer(std::ostream& os)
    : os(os), indent(0)
  { }

  void operator()(Name const& n) { id(n); }
  void operator()(Type const& t) { type(t); }
  void operator()(Expr const& e) { expression(e); }
  void operator()(Stmt const& s) { statement(s); }
  void operator()(Decl const& d) { declaration(d); }
  void operator()(Cons const& c) { constraint(c); }

  // Lexical terms.
  void space();
  void newline();
  void newline_and_indent();
  void newline_and_undent();
  void token(Token_kind);
  void token(Token);
  void token(Symbol const&);
  void token(char const*);
  void token(String const&);
  void token(int);
  void token(Integer const&);
  void tokens(Token_seq const&);

  void binary_operator(Token_kind);

  void identifier(Decl const&);
  void id(Name const&);
  void unqualified_id(Name const&);
  void unqualified_id(Simple_id const&);
  void unqualified_id(Placeholder_id const&);
  void destructor_id(Destructor_id const&);
  void operator_id(Operator_id const&);
  void conversion_id(Conversion_id const&);
  void literal_id(Literal_id const&);
  void template_id(Template_id const&);
  void concept_id(Concept_id const&);
  void qualified_id(Qualified_id const&);
  void nested_name_specifier(Decl const&);

  // Types
  void type(Type const&);
  void type(Unparsed_type const&);
  void suffix_type(Type const&);
  void suffix_type(Pack_type const&);
  void prefix_type(Type const&);
  void prefix_type(Reference_type const&);
  void unary_type(Type const&);
  void unary_type(Pointer_type const&);
  void unary_type(Qualified_type const&);
  void postfix_type(Type const&);
  void postfix_type(Array_type const&);
  void postfix_type(Tuple_type const&);
  void postfix_type(Slice_type const&);
  void postfix_type(Dynarray_type const&);
  void primary_type(Type const&);
  void primary_type(Void_type const&);
  void primary_type(Boolean_type const&);
  void primary_type(Byte_type const&);
  void primary_type(Integer_type const&);
  void primary_type(Float_type const&);
  void primary_type(Auto_type const&);
  void primary_type(Function_type const&);
  void primary_type(Tuple_type const&);
  void primary_type(Type_type const&);
  void id_type(Class_type const&);
  void grouped_type(Type const&);

  // Expressions
  void expression(Expr const&);
  void expression(Unparsed_expr const&);
  void logical_or_expression(Expr const&);
  void logical_and_expression(Expr const&);
  void inclusive_or_expression(Expr const&);
  void exclusive_or_expression(Expr const&);
  void and_expression(Expr const&);
  void equality_expression(Expr const&);
  void relational_expression(Expr const&);
  void shift_expression(Expr const&);
  void additive_expression(Expr const&);
  void multiplicative_expression(Expr const&);
  void unary_expression(Expr const&);
  void postfix_expression(Expr const&);
  void postfix_expression(Call_expr const&);
  void postfix_expression(Dot_expr const&);
  void postfix_expression(Value_conv const&);
  void postfix_expression(Qualification_conv const&);
  void postfix_expression(Boolean_conv const&);
  void postfix_expression(Integer_conv const&);
  void postfix_expression(Float_conv const&);
  void postfix_expression(Numeric_conv const&);
  void postfix_expression(Dependent_conv const&);
  void postfix_expression(Ellipsis_conv const&);
  void subscript_expression(Expr const&);
  void primary_expression(Expr const&);
  void grouped_expression(Expr const&);

  void literal(Boolean_expr const&);
  void literal(Integer_expr const&);
  void literal(Real_expr const&);
  void id_expression(Id_expr const&);
  void id_expression(Decl_expr const&);
  void id_expression(Check_expr const&);
  void id_expression(Synthetic_expr const&);

  void requires_expression(Requires_expr const&);

  Expr_list expression_list();


  // Statements
  void statement(Stmt const&);
  void statement(Unparsed_stmt const&);
  void statement_seq(Stmt_list const&);
  void empty_statement(Empty_stmt const&);
  void translation_statement(Translation_stmt const&);
  void member_statement(Member_stmt const&);
  void compound_statement(Compound_stmt const&);
  void return_statement(Return_stmt const&);
  void if_statement(If_then_stmt const&);
  void if_statement(If_else_stmt const&);
  void while_statement(While_stmt const&);
  void break_statement(Break_stmt const&);
  void continue_statement(Continue_stmt const&);
  void expression_statement(Expression_stmt const&);
  void declaration_statement(Declaration_stmt const&);

  // Initializers
  void initializer(Expr const&);
  void initializer(Init const&);
  void equal_initializer(Copy_init const&);
  void equal_initializer(Bind_init const&);
  void paren_initializer(Direct_init const&);
  void brace_initializer(Aggregate_init const&);

  // Declarations
  void declaration(Decl const&);
  void declaration_seq(Decl_list const&);
  void specifier(Token_kind);
  void specifier_seq(Specifier_set);

  // Supers
  void super_declaration(Super_decl const&);

  // Variables
  void variable_declaration(Variable_decl const&);
  void variable_initializer(Def const&);
  void variable_initializer(Empty_def const&);
  void variable_initializer(Expression_def const&);

  // Functions
  void function_declaration(Function_decl const&);
  void function_definition(Def const&);
  void function_definition(Expression_def const&);
  void function_definition(Function_def const&);
  void function_definition(Deleted_def const&);
  void function_definition(Defaulted_def const&);
  void parameter(Decl const&);
  void parameter(Object_parm const&);
  void parameter_list(Decl_list const&);

  // Classes
  void class_declaration(Class_decl const&);
  void class_definition(Def const&);
  void class_definition(Class_def const&);

  // Concepts
  void concept_declaration(Concept_decl const&);
  void concept_definition(Def const&);
  void concept_definition(Expression_def const&);
  void concept_definition(Concept_def const&);
  void concept_member_seq(Req_list const&);
  void concept_member(Req const&);
  void concept_member(Syntactic_req const&);
  void concept_member(Semantic_req const&);
  void concept_member(Type_req const&);
  void concept_member(Expression_req const&);

  // Templates
  void template_declaration(Template_decl const&);
  void type_template_parameter(Type_parm const&);
  void value_template_parameter(Value_parm const&);
  void template_template_parameter(Template_parm const&);
  void template_parameter(Decl const&);
  void template_parameter_list(Decl_list const&);
  void template_argument(Term const&);
  void template_argument_list(Term_list const&);
  void requires_clause(Expr const&);

  // Requirements
  void usage_seq(Req_list const&);
  void usage_requirement(Req const&);
  void requirement(Basic_req const&);
  void requirement(Conversion_req const&);
  void requirement(Deduction_req const&);

  // Constraints
  // These don't really have an external syntax, but it's
  // helpful for debugging.
  void constraint(Cons const&);
  void constraint(Concept_cons const&);
  void constraint(Predicate_cons const&);
  void constraint(Expression_cons const&);
  void constraint(Conversion_cons const&);
  void constraint(Parameterized_cons const&);
  void constraint(Conjunction_cons const&);
  void constraint(Disjunction_cons const&);
  void grouped_constraint(Cons const&);

  std::ostream& os;     // Output stream
  int           indent; // The current indentation
};





std::ostream& operator<<(std::ostream&, Term const&);
std::ostream& operator<<(std::ostream&, Name const&);
std::ostream& operator<<(std::ostream&, Type const&);
std::ostream& operator<<(std::ostream&, Expr const&);
std::ostream& operator<<(std::ostream&, Stmt const&);
std::ostream& operator<<(std::ostream&, Decl const&);
std::ostream& operator<<(std::ostream&, Cons const&);


} // namespace banjo

#endif
