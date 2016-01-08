// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "print.hpp"

#include <iterator>
#include <iostream>


namespace beaker
{

// -------------------------------------------------------------------------- //
// Lexical items


// Conditionally print whitespace based on this and the
// previous token.
void
Printer::space(Token_kind k)
{
  if (prev != error_tok) {
    // Always put space after a comma.
    if (prev == comma_tok)
      space();

    // Identifers and keywods must be separated.
    else if (is_keyword(prev) && k == identifier_tok)
      space();
    else if (is_keyword(k) && prev == identifier_tok)
      space();

    // FIXME: The rules for operators are going to be
    // a bit tricky since they may merge.
  }
}


// Unconditionally print whitespace.
void
Printer::space()
{
  os << ' ';

  // Reset the previous token.
  prev = error_tok;
}


// TODO: Indent after newline?
void
Printer::newline()
{
  os << '\n';

  // Reset the previous token.
  prev = error_tok;
}



// Print the token. Print an extra space after keywords
// and commas.
void
Printer::token(Token_kind k)
{
  space(k);
  os << get_spelling(k);
  prev = k;
}


void
Printer::token(Symbol const& sym)
{
  Token_kind k = (Token_kind)sym.token();
  space(k);
  os << sym;
  prev = k;
}


// Print a string as an identifier token.
void
Printer::token(char const* str)
{
  space(identifier_tok);
  os << str;
  prev = identifier_tok;
}


// -------------------------------------------------------------------------- //
// Names

void
Printer::id(Name const& n)
{
  if (Qualified_id const* q = as<Qualified_id>(&n))
    qualified_id(*q);
  else
    unqualified_id(n);
}


void
Printer::unqualified_id(Name const& n)
{
  struct fn
  {
    Printer& p;
    void operator()(Simple_id const& n)      { p.unqualified_id(n); }
    void operator()(Global_id const& n)      { }
    void operator()(Placeholder_id const& n) { }
    void operator()(Operator_id const& n)    { p.operator_id(n); }
    void operator()(Conversion_id const& n)  { p.conversion_id(n); }
    void operator()(Literal_id const& n)     { p.literal_id(n); }
    void operator()(Destructor_id const& n)  { p.destructor_id(n); }
    void operator()(Template_id const& n)    { p.template_id(n); }
    void operator()(Qualified_id const& n)   { lingo_unreachable(); }
  };
  apply(n, fn{*this});
}


void
Printer::unqualified_id(Simple_id const& n)
{
  token(n.symbol());
}


void
Printer::destructor_id(Destructor_id const& n)
{
  token(tilde_tok);
  // type(n.type());
}


// TODO: Implement me.
void
Printer::operator_id(Operator_id const&)
{
  token("operator");
}


// TODO: Implement me.
void
Printer::conversion_id(Conversion_id const&)
{
  token("operator");
}


// TODO: Implement me.
void
Printer::literal_id(Literal_id const&)
{
  token("operator");
}


// TODO: Implement me.
void
Printer::template_id(Template_id const& n)
{
  os << "<|template-decl|>" << '<' << '>';
}


// TODO: This is sufficiently complicated that it may warrant
// a re-thinking of how qualified ids are represneted (i.e.,
// right-to-left instead of left-to-right). See nested name
// specifier as well.
void
Printer::qualified_id(Qualified_id const& n)
{
  nested_name_specifier(n);
  Qualified_id const* p = &n;
  while (is<Qualified_id>(&p->name()))
    p = cast<Qualified_id>(&p->name());
  unqualified_id(p->name());
}


void
Printer::nested_name_specifier(Qualified_id const& n)
{
  unqualified_id(n.context().name());
  os << "::";
  if (Qualified_id const* q = as<Qualified_id>(&n.name()))
    nested_name_specifier(*q);
}


// -------------------------------------------------------------------------- //
// Printing of types

void
Printer::type(Type const& t)
{
  struct fn
  {
    Printer& p;
    void operator()(Void_type const& t)      { p.simple_type(t); }
    void operator()(Boolean_type const& t)   { p.simple_type(t); }
    void operator()(Integer_type const& t)   { p.simple_type(t); }
    void operator()(Float_type const& t)     { p.simple_type(t); }
    void operator()(Auto_type const& t)      { p.simple_type(t); }
    void operator()(Decltype_type const& t)  { p.simple_type(t); }
    void operator()(Declauto_type const& t)  { p.simple_type(t); }
    void operator()(Qualified_type const& t) { lingo_unreachable(); }
    void operator()(Pointer_type const& t)   { lingo_unreachable(); }
    void operator()(Reference_type const& t) { lingo_unreachable(); }
    void operator()(Array_type const& t)     { lingo_unreachable(); }
    void operator()(Sequence_type const& t)  { lingo_unreachable(); }
    void operator()(Class_type const& t)     { lingo_unreachable(); }
    void operator()(Union_type const& t)     { lingo_unreachable(); }
    void operator()(Enum_type const& t)      { lingo_unreachable(); }
    void operator()(Typename_type const& t)  { p.simple_type(t); }
  };
  apply(t, fn{*this});
}


void
Printer::simple_type(Void_type const& t)
{
  os << "void";
}


void
Printer::simple_type(Boolean_type const& t)
{
  os << "bool";
}


void
Printer::simple_type(Integer_type const& t)
{
  if (t.is_unsigned())
    os << 'u';
  os << "int" << t.precision();
}


void
Printer::simple_type(Float_type const& t)
{
  os << "float" << t.precision();
}


void
Printer::simple_type(Auto_type const& t)
{
  os << "auto";
}


// TODO: Implement me.
void
Printer::simple_type(Decltype_type const& t)
{
  os << "decltype(<|expr|>)";
}


void
Printer::simple_type(Declauto_type const& t)
{
  os << "decltype(auto)";
}


void
Printer::simple_type(Typename_type const& t)
{
  os << t.declaration().name();
}


void
Printer::return_type(Type const& t)
{
  os << "->" << ' ' << t;
}


// -------------------------------------------------------------------------- //
// Expressions


// Returns the precedence of the operator for an expression.
int
precedence(Expr const& e)
{
  struct fn
  {
    int operator()(Boolean_expr const& e)   { return 0; }
    int operator()(Integer_expr const& e)   { return 0; }
    int operator()(Real_expr const& e)      { return 0; }
    int operator()(Reference_expr const& e) { return 0; }
    int operator()(Add_expr const& e)       { return 6; }
    int operator()(Sub_expr const& e)       { return 6; }
    int operator()(Mul_expr const& e)       { return 5; }
    int operator()(Div_expr const& e)       { return 5; }
    int operator()(Rem_expr const& e)       { return 5; }
    int operator()(Neg_expr const& e)       { return 3; }
    int operator()(Pos_expr const& e)       { return 3; }
    int operator()(Eq_expr const& e)        { return 9; }
    int operator()(Ne_expr const& e)        { return 9; }
    int operator()(Lt_expr const& e)        { return 8; }
    int operator()(Gt_expr const& e)        { return 8; }
    int operator()(Le_expr const& e)        { return 8; }
    int operator()(Ge_expr const& e)        { return 8; }
    int operator()(And_expr const& e)       { return 13; }
    int operator()(Or_expr const& e)        { return 14; }
    int operator()(Not_expr const& e)       { return 3; }
    int operator()(Assign_expr const& e)    { return 15; }
  };
  return apply(e, fn{});
}


void
Printer::expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Boolean_expr const& e)   { p.literal(e); }
    void operator()(Integer_expr const& e)   { p.literal(e); }
    void operator()(Real_expr const& e)      { p.literal(e); }
    void operator()(Reference_expr const& e) { p.id_expression(e); }
    void operator()(Add_expr const& e)       { p.binary_expression(e, plus_tok); }
    void operator()(Sub_expr const& e)       { p.binary_expression(e, minus_tok); }
    void operator()(Mul_expr const& e)       { p.binary_expression(e, star_tok); }
    void operator()(Div_expr const& e)       { p.binary_expression(e, slash_tok); }
    void operator()(Rem_expr const& e)       { p.binary_expression(e, percent_tok); }
    void operator()(Neg_expr const& e)       { p.unary_expression(e, minus_tok); }
    void operator()(Pos_expr const& e)       { p.unary_expression(e, plus_tok); }
    void operator()(Eq_expr const& e)        { p.binary_expression(e, eq_eq_tok); }
    void operator()(Ne_expr const& e)        { p.binary_expression(e, bang_eq_tok); }
    void operator()(Lt_expr const& e)        { p.binary_expression(e, lt_tok); }
    void operator()(Gt_expr const& e)        { p.binary_expression(e, gt_tok); }
    void operator()(Le_expr const& e)        { p.binary_expression(e, lt_eq_tok); }
    void operator()(Ge_expr const& e)        { p.binary_expression(e, gt_eq_tok); }
    void operator()(And_expr const& e)       { p.binary_expression(e, amp_amp_tok); }
    void operator()(Or_expr const& e)        { p.binary_expression(e, bar_bar_tok); }
    void operator()(Not_expr const& e)       { p.unary_expression(e, bang_tok); }
    void operator()(Assign_expr const& e)    { p.binary_expression(e, eq_tok); }
  };
  apply(e, fn{*this});
}


// TODO: All of the literal expressions can be unified into
// a single function.
void
Printer::literal(Boolean_expr const& e)
{
  token(e.symbol());
}


void
Printer::literal(Integer_expr const& e)
{
  token(e.symbol());
}


void
Printer::literal(Real_expr const& e)
{
  token(e.symbol());
}


void
Printer::id_expression(Reference_expr const& e)
{
  id(e.name());
}


// Possibly print a grouped expression s as a subexpression
// of e.
void
Printer::grouped_expression(Expr const& e, Expr const& s)
{
  if (precedence(e) <= precedence(s)) {
    token(lparen_tok);
    expression(s);
    token(rparen_tok);
  } else {
    expression(s);
  }
}


void
Printer::unary_expression(Unary_expr const& e, Token_kind k)
{
  token(k);
  grouped_expression(e, e.operand());
}


void
Printer::binary_expression(Binary_expr const& e, Token_kind k)
{
  grouped_expression(e, e.left());
  token(k);
  grouped_expression(e, e.right());
}


// -------------------------------------------------------------------------- //
// Declarations


// Dispatch for declarations (as in declaration statements).
struct declaration_fn
{
  Printer& p;

  template<typename T>
  void operator()(T const&) { lingo_unreachable(); }

  void operator()(Variable_decl const& d)  { p.variable_declaration(d); }
  void operator()(Constant_decl const& d)  { p.constant_declaration(d); }
  void operator()(Function_decl const& d)  { p.function_declaration(d); }
  void operator()(Class_decl const& d)     { p.class_declaration(d); }
  void operator()(Union_decl const& d)     { p.union_declaration(d); }
  void operator()(Enum_decl const& d)      { p.enum_declaration(d); }
  void operator()(Namespace_decl const& d) { p.namespace_declaration(d); }
  void operator()(Template_decl const& d)  { p.template_declaration(d); }
};


void
Printer::declaration(Decl const& d)
{
  apply(d, declaration_fn{*this});
}


// FIXME: Print the initializer.
void
Printer::variable_declaration(Variable_decl const& d)
{
  os << "var " << d.type() << ' ' << d.name() << d.initializer() << ';';
}


// FIXME: Print the initializer.
void
Printer::constant_declaration(Constant_decl const& d)
{
  os << "const " << d.type() << ' ' << d.name() << d.initializer() << ';';
}


// FIXME: Print the definition.
void
Printer::function_declaration(Function_decl const& d)
{
  os << "def " << d.name();
  os << '(';
  if (!d.parameters().empty())
    parameter_list(d.parameters());
  os << ')' << ' ';
  return_type(d.return_type());
}


void
Printer::class_declaration(Class_decl const& d)
{
  lingo_unreachable();
}


void
Printer::union_declaration(Union_decl const& d)
{
  lingo_unreachable();
}


void
Printer::enum_declaration(Enum_decl const& d)
{
  lingo_unreachable();
}


// FIXME: Handle the global namespace and anonymous namespaces.
//
// FIXME: Print the list of members.
void
Printer::namespace_declaration(Namespace_decl const& d)
{
  os << "namespace " << d.name() << " {";
  os << "}";
}


void
Printer::template_declaration(Template_decl const& d)
{
  token(template_tok);
  token(lt_tok);
  template_parameter_list(d.parameters());
  token(gt_tok);
  newline();
  declaration(d.pattern());
}


// Dispatch function for printing parameters. This combines
// the printing of all parameters into the same framework
// for convenience.
struct parameter_fn
{
  Printer& p;

  template<typename T>
  void operator()(T const&) { lingo_unreachable(); }

  void operator()(Object_parm const& d)   { p.parameter(d); }
  void operator()(Variadic_parm const& d) { p.parameter(d); }
  void operator()(Value_parm const& d)    { p.value_template_parameter(d); }
  void operator()(Type_parm const& d)     { p.type_template_parameter(d); }
  void operator()(Template_parm const& d) { p.template_template_parameter(d); }
};


void
Printer::parameter(Decl const& d)
{
  apply(d, parameter_fn{*this});
}


// TODO: Print the default argument.
void
Printer::parameter(Object_parm const& p)
{
  type(p.type());
  id(p.name());
}


void
Printer::parameter(Variadic_parm const& p)
{
  token(ellipsis_tok);
}


// TODO: Rename to parameter-clause?
void
Printer::parameter_list(Decl_list const& d)
{
  for (auto iter = d.begin(); iter != d.end(); ++iter) {
    parameter(*iter);
    if (std::next(iter) != d.end())
      token(comma_tok);
  }
}


void
Printer::template_parameter(Decl const& d)
{
  apply(d, parameter_fn{*this});
}


// FIXME: Emit a default argument.
void
Printer::type_template_parameter(Type_parm const& d)
{
  token(typename_tok);
  id(d.name());
}


// FIXME: Emit a default argument.
void
Printer::value_template_parameter(Value_parm const& d)
{
  token(const_tok);
  id(d.name());
}


// FIXME: Implement me!
void
Printer::template_template_parameter(Template_parm const& d)
{
  token(template_tok);
  id(d.name());
}


void
Printer::template_parameter_list(Decl_list const& ps)
{
  for (auto iter = ps.begin(); iter != ps.end(); ++iter) {
    template_parameter(*iter);
    if (std::next(iter) != ps.end())
      token(comma_tok);
  }
}



// -------------------------------------------------------------------------- //
// Streaming

std::ostream&
operator<<(std::ostream& os, Name const& n)
{
  Printer print(os);
  print(n);
  return os;
}


std::ostream&
operator<<(std::ostream& os, Type const& t)
{
  Printer print(os);
  print(t);
  return os;
}


std::ostream&
operator<<(std::ostream& os, Expr const& e)
{
  Printer print(os);
  print(e);
  return os;
}


std::ostream&
operator<<(std::ostream& os, Decl const& d)
{
  Printer print(os);
  print(d);
  return os;
}


// TODO: Implement me.
std::ostream&
operator<<(std::ostream& os, Init const& i)
{
  return os;
}


} // namespace beaker
