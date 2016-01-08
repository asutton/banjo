// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PRINT_HPP
#define BEAKER_PRINT_HPP

#include "ast.hpp"
#include "lexer.hpp"

#include <iosfwd>


namespace beaker
{

struct Printer
{
  Printer(std::ostream& os)
    : os(os), prev(error_tok)
  { }

  void operator()(Name const& n) { id(n); }
  void operator()(Type const& t) { type(t); }
  void operator()(Decl const& d) { declaration(d); }

  // Lexical terms.
  void token(Token_kind);
  void token(Symbol const&);
  void token(char const*);
  void space(Token_kind);
  void space();
  void newline();

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
  void nested_name_specifier(Qualified_id const&);

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
  void simple_type(Typename_type const&);
  void return_type(Type const&);

  // Expressions
  void expression(Expr const&);
  void literal(Boolean_expr const&);
  void literal(Integer_expr const&);
  void literal(Real_expr const&);
  void id_expression(Reference_expr const&);
  void grouped_expression(Expr const&, Expr const&);
  void binary_expression(Binary_expr const&, Token_kind);
  void unary_expression(Unary_expr const&, Token_kind);

  // Declarations
  void declaration(Decl const&);
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


  std::ostream& os;
  Token_kind    prev; // The previous token printed
};

std::ostream& operator<<(std::ostream&, Name const&);
std::ostream& operator<<(std::ostream&, Type const&);
std::ostream& operator<<(std::ostream&, Decl const&);
std::ostream& operator<<(std::ostream&, Init const&);


} // namespace beaker

#endif
