// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "printer.hpp"
#include "ast.hpp"

#include <iterator>
#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Lexical items

// Unconditionally print whitespace. Clear the previous token.
void
Printer::space()
{
  os << ' ';
}


// Print a new line and indent to the current depth. Clear the previous token.
void
Printer::newline()
{
  os << '\n' << std::string(2 * indent, ' ');
}


// Print a newline and indent one level.
void
Printer::newline_and_indent()
{
  ++indent;
  newline();
}


// Print a newline and undent (back up) one level.
void
Printer::newline_and_undent()
{
  --indent;
  newline();
}


// Print the token.
void
Printer::token(Token_kind k)
{
  os << get_spelling(k);
}


// Print the given token.
void
Printer::token(Token k)
{
  os << k.spelling();
}


// Print the text of the given symbol.
void
Printer::token(Symbol const& sym)
{
  os << sym.spelling();
}


// Print a string as an identifier token.
void
Printer::token(char const* str)
{
  os << str;
}


// Print a string as an identifier.
void
Printer::token(String const& str)
{
  os << str;
}


// Print n as an integer token.
void
Printer::token(int n)
{
  os << n;
}


void
Printer::token(Integer const& n)
{
  os << n;
}


void
Printer::tokens(Token_seq const& toks)
{
  for (auto iter = toks.begin(); iter != toks.end(); ++iter) {
    token(*iter);
    if (std::next(iter) != toks.end())
      space();
  }
}


// -------------------------------------------------------------------------- //
// Helper functions

// Print the token with space around it.
void
Printer::binary_operator(Token_kind k)
{
  space();
  token(k);
  space();
}


// -------------------------------------------------------------------------- //
// Names

void
Printer::identifier(Decl const& d)
{
  unqualified_id(d.name());
}


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
    void operator()(Placeholder_id const& n) { p.unqualified_id(n); }
    void operator()(Operator_id const& n)    { p.operator_id(n); }
    void operator()(Conversion_id const& n)  { p.conversion_id(n); }
    void operator()(Literal_id const& n)     { p.literal_id(n); }
    void operator()(Destructor_id const& n)  { p.destructor_id(n); }
    void operator()(Template_id const& n)    { p.template_id(n); }
    void operator()(Concept_id const& n)     { p.concept_id(n); }
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
Printer::unqualified_id(Placeholder_id const& n)
{
  token(lt_tok);
  token("invented-id-");
  token(n.number());
  token(gt_tok);
}


void
Printer::destructor_id(Destructor_id const& n)
{
  token(tilde_tok);
  // type(n.type());
}


void
Printer::operator_id(Operator_id const& n)
{
  token("operator");
  switch (n.kind()) {
    case add_op: token(plus_tok); break;
    case sub_op: token(minus_tok); break;
    case mul_op: token(star_tok); break;
    case div_op: token(slash_tok); break;
    case rem_op: token(percent_tok); break;
    case eq_op: token(eq_eq_tok); break;
    case ne_op: token(bang_eq_tok); break;
    case lt_op: token(lt_tok); break;
    case gt_op: token(gt_tok); break;
    case le_op: token(lt_eq_tok); break;
    case ge_op: token(gt_eq_tok); break;
    case and_op: token(amp_amp_tok); break;
    case or_op: token(bar_bar_tok); break;
    case not_op: token(bang_tok); break;
    case call_op:
      token(lparen_tok);
      token(rparen_tok);
      break;
    case index_op:
      token(lbracket_tok);
      token(rbracket_tok);
      break;
    case assign_op: token(eq_tok); break;
    default: lingo_unreachable();
  }
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


void
Printer::template_id(Template_id const& n)
{
  // FIXME: Use a template-name production?
  id(n.declaration().name());

  // FIXME: Don't insert spaces after the template name.
  token(lt_tok);
  template_argument_list(n.arguments());
  token(gt_tok);
}


void
Printer::concept_id(Concept_id const& n)
{
  // FIXME: Use a concept-name production?
  id(n.declaration().name());
  token(lt_tok);
  template_argument_list(n.arguments());
  token(gt_tok);
}


// TODO: This is sufficiently complicated that it may warrant
// a re-thinking of how qualified ids are represneted (i.e.,
// right-to-left instead of left-to-right). See nested name
// specifier as well.
void
Printer::qualified_id(Qualified_id const& n)
{
  nested_name_specifier(n.scope());
  unqualified_id(n.name());
}


// TODO: Find the least qualification needed to make
// the qualified-id valid. This currently generates
void
Printer::nested_name_specifier(Decl const& d)
{
  // Generate the list of enclosing scopes.
  std::vector<Decl const*> scopes;
  Decl const* p = &d;
  while (p) {
    scopes.push_back(p);
    p = p->context();
  }

  // Print out the nested name specifier.
  for (auto iter = scopes.rbegin(); iter != scopes.rend(); ++iter) {
    Decl const* d = *iter;
    id(d->name());
    token(colon_colon_tok);
  }
}


// -------------------------------------------------------------------------- //
// Printing of types

// Print a type.
//
//    type:
//      primary-type
//      unparsed-type
//
// FIXME: Actually implement the type grammar.
void
Printer::type(Type const& t)
{
  if (Unparsed_type const* t1 = as<Unparsed_type>(&t))
    return type(*t1);
  return suffix_type(t);
}


void
Printer::type(Unparsed_type const& t)
{
  token(lt_tok);
  token(bar_tok);
  tokens(t.tokens());
  token(bar_tok);
  token(gt_tok);
}


void
Printer::suffix_type(Type const& t)
{
  if (Pack_type const* t1 = as<Pack_type>(&t))
    suffix_type(*t1);
  prefix_type(t);
}


void
Printer::suffix_type(Pack_type const& t)
{
  type(t.type());
  token(ellipsis_tok);
}


void
Printer::prefix_type(Type const& t)
{
  struct fn
  {
    Printer& p;
    void operator()(Type const& t)           { p.unary_type(t); }
    void operator()(Reference_type const& t) { p.prefix_type(t); }
  };
  return apply(t, fn{*this});
}


void
Printer::prefix_type(Reference_type const& t)
{
  token(amp_tok);
  unary_type(t.type());
}


void
Printer::unary_type(Type const& t)
{
  struct fn
  {
    Printer& p;
    void operator()(Type const& t)           { p.postfix_type(t); }
    void operator()(Pointer_type const& t)   { p.unary_type(t); }
    void operator()(Qualified_type const& t) { p.unary_type(t); }
  };
  return apply(t, fn{*this});
}


void
Printer::unary_type(Pointer_type const& t)
{
  token(star_tok);
  unary_type(t.type());
}


void
Printer::unary_type(Qualified_type const& t)
{
  if (t.is_const()) {
    token(const_tok);
    space();
  }
  if (t.is_volatile()) {
    token(volatile_tok);
    space();
  }
  unary_type(t.type());
}


void
Printer::postfix_type(Type const& t)
{
  struct fn
  {
    Printer& p;
    void operator()(Type const& t)          { p.primary_type(t); }
    void operator()(Array_type const& t)    { p.postfix_type(t); }
    void operator()(Slice_type const& t)    { p.postfix_type(t); }
    void operator()(Dynarray_type const& t) { p.postfix_type(t); }
  };
  return apply(t, fn{*this});
}


void
Printer::postfix_type(Array_type const& t)
{
  postfix_type(t.type());
  token(lbracket_tok);
  expression(t.extent());
  token(rbracket_tok);
}


void
Printer::postfix_type(Slice_type const& t)
{
  postfix_type(t.type());
  token(lbracket_tok);
  token(rbracket_tok);
}


// FIXME: Unify with array-type.
void
Printer::postfix_type(Dynarray_type const& t)
{
  postfix_type(t.type());
  token(lbracket_tok);
  expression(t.extent());
  token(rbracket_tok);
}


void
Printer::primary_type(Type const& t)
{
  struct fn
  {
    Printer& p;
    void operator()(Type const& t)           { p.grouped_type(t); }
    void operator()(Void_type const& t)      { p.primary_type(t); }
    void operator()(Boolean_type const& t)   { p.primary_type(t); }
    void operator()(Byte_type const& t)      { p.primary_type(t); }
    void operator()(Integer_type const& t)   { p.primary_type(t); }
    void operator()(Float_type const& t)     { p.primary_type(t); }
    void operator()(Auto_type const& t)      { p.primary_type(t); }
    void operator()(Function_type const& t)  { p.primary_type(t); }
    void operator()(Tuple_type const& t)     { p.primary_type(t); }
    void operator()(Type_type const& t)      { p.primary_type(t); }
    void operator()(Class_type const& t)     { p.id_type(t); }
    void operator()(Typename_type const& t)  { p.id_type(t); }
  };
  apply(t, fn{*this});
}


void
Printer::primary_type(Void_type const& t)
{
  token(void_tok);
}


void
Printer::primary_type(Boolean_type const& t)
{
  token(bool_tok);
}


void
Printer::primary_type(Byte_type const& t)
{
  token(byte_tok);
}


// FIXME: Map this back to a token.
void
Printer::primary_type(Integer_type const& t)
{
  token(int_tok);
}


// FIXME: Map this back to a token.
void
Printer::primary_type(Float_type const& t)
{
  token(float_tok);
}


void
Printer::primary_type(Auto_type const& t)
{
  token(auto_tok);
}


void
Printer::primary_type(Function_type const& t)
{
  token(lparen_tok);
  Type_list const& p = t.parameter_types();
  for (auto iter = p.begin(); iter != p.end(); ++iter) {
    type(*iter);
    if (std::next(iter) != p.end())
      token(comma_tok);
  }
  token(rparen_tok);
  space();
  token(arrow_tok);
  space();
  type(t.return_type());
}


void
Printer::primary_type(Tuple_type const& t)
{
  token(lbrace_tok);
  Type_list const& p = t.type_list();
  for (auto iter = p.begin(); iter != p.end(); ++iter) {
    type(*iter);
    if (std::next(iter) != p.end())
      token(comma_tok);
  }
  token(rbrace_tok);
}


// FIXME: We can't really spell this type, so leave it empty.
void
Printer::primary_type(Type_type const& t)
{
}


// Print the name of the class type.
void
Printer::id_type(Class_type const& t)
{
  identifier(t.declaration());
}


// Print the name of the type parameter.
void
Printer::id_type(Typename_type const& t)
{
  identifier(t.declaration());
}


void
Printer::grouped_type(Type const& t)
{
  token(lparen_tok);
  type(t);
  token(rparen_tok);
}


// -------------------------------------------------------------------------- //
// Expressions

// Print an expression.
//
//    expression:
//      logical-or-expression -- FIXME: Wrong!
//      unparsed-expression
void
Printer::expression(Expr const& e)
{
  if (Unparsed_expr const* e1 = as<Unparsed_expr>(&e))
    expression(*e1);
  else
    logical_or_expression(e);
}


void
Printer::expression(Unparsed_expr const& e)
{
  token(lt_tok);
  token(bar_tok);
  tokens(e.tokens());
  token(bar_tok);
  token(gt_tok);
}


void
Printer::logical_or_expression(Expr const& e)
{
  if (Or_expr const* expr = as<Or_expr>(&e)) {
    logical_or_expression(expr->left());
    space();
    token(bar_bar_tok);
    space();
    logical_and_expression(expr->right());
  } else {
    logical_and_expression(e);
  }
}


void
Printer::logical_and_expression(Expr const& e)
{
  if (And_expr const* expr = as<And_expr>(&e)) {
    logical_and_expression(expr->left());
    space();
    token(amp_amp_tok);
    space();
    inclusive_or_expression(expr->right());
  } else {
    inclusive_or_expression(e);
  }
}


void
Printer::inclusive_or_expression(Expr const& e)
{
  if (Bit_or_expr const* expr = as<Bit_or_expr>(&e)) {
    inclusive_or_expression(expr->left());
    space();
    token(amp_amp_tok);
    space();
    exclusive_or_expression(expr->right());
  } else {
    exclusive_or_expression(e);
  }
}


void
Printer::exclusive_or_expression(Expr const& e)
{
  if (Bit_xor_expr const* expr = as<Bit_xor_expr>(&e)) {
    exclusive_or_expression(expr->left());
    space();
    token(amp_amp_tok);
    space();
    and_expression(expr->right());
  } else {
    and_expression(e);
  }
}


void
Printer::and_expression(Expr const& e)
{
  if (Bit_and_expr const* expr = as<Bit_and_expr>(&e)) {
    and_expression(expr->left());
    space();
    token(amp_amp_tok);
    space();
    equality_expression(expr->right());
  } else {
    equality_expression(e);
  }
}


void
Printer::equality_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e) { p.relational_expression(e); }
    void operator()(Eq_expr const& e) { print(e, eq_eq_tok); }
    void operator()(Ne_expr const& e) { print(e, bang_eq_tok); }

    void print(Binary_expr const& e, Token_kind k)
    {
      p.equality_expression(e.left());
      p.binary_operator(k);
      p.relational_expression(e.right());
    }
  };
  apply(e, fn{*this});
}


void
Printer::relational_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e)     { p.shift_expression(e); }
    void operator()(Lt_expr const& e)  { print(e, lt_tok); }
    void operator()(Gt_expr const& e)  { print(e, gt_tok); }
    void operator()(Le_expr const& e)  { print(e, gt_eq_tok); }
    void operator()(Ge_expr const& e)  { print(e, gt_eq_tok); }
    void operator()(Cmp_expr const& e) { print(e, lt_eq_gt_tok); }

    void print(Binary_expr const& e, Token_kind k)
    {
      p.relational_expression(e.left());
      p.binary_operator(k);
      p.shift_expression(e.right());
    }
  };
  apply(e, fn{*this});
}


void
Printer::shift_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e) { p.additive_expression(e); }
    void operator()(Bit_lsh_expr const& e) { print(e, lt_lt_tok); }
    void operator()(Bit_rsh_expr const& e) { print(e, gt_gt_tok); }

    void print(Binary_expr const& e, Token_kind k)
    {
      p.shift_expression(e.left());
      p.binary_operator(k);
      p.additive_expression(e.right());
    }
  };
  apply(e, fn{*this});
}


void
Printer::additive_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e) { p.multiplicative_expression(e); }
    void operator()(Add_expr const& e) { print(e, plus_tok); }
    void operator()(Sub_expr const& e) { print(e, minus_tok); }

    void print(Binary_expr const& e, Token_kind k)
    {
      p.additive_expression(e.left());
      p.binary_operator(k);
      p.multiplicative_expression(e.right());
    }
  };
  apply(e, fn{*this});
}


void
Printer::multiplicative_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e) { p.unary_expression(e); }
    void operator()(Mul_expr const& e) { print(e, star_tok); }
    void operator()(Div_expr const& e) { print(e, slash_tok); }
    void operator()(Rem_expr const& e) { print(e, percent_tok); }

    void print(Binary_expr const& e, Token_kind k)
    {
      p.multiplicative_expression(e.left());
      p.binary_operator(k);
      p.unary_expression(e.right());
    }
  };
  apply(e, fn{*this});
}


void
Printer::unary_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e)         { p.postfix_expression(e); }
    void operator()(Not_expr const& e)     { print(e, bang_tok); }
    void operator()(Bit_not_expr const& e) { print(e, tilde_tok); }
    void operator()(Neg_expr const& e)     { print(e, minus_tok); }
    void operator()(Pos_expr const& e)     { print(e, plus_tok); }

    void print(Unary_expr const& e, Token_kind k)
    {
      p.token(k);
      p.space();  // Print +(+x) as "+ + x" and not "++x".
      p.unary_expression(e.operand());
    }
  };
  apply(e, fn{*this});
}


void
Printer::postfix_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e)               { p.primary_expression(e); }
    void operator()(Dot_expr const& e)           { p.postfix_expression(e); }
    void operator()(Call_expr const& e)          { p.postfix_expression(e); }
    void operator()(Tuple_expr const& e)         { p.postfix_expression(e); }
    void operator()(Value_conv const& e)         { p.postfix_expression(e); }
    void operator()(Qualification_conv const& e) { p.postfix_expression(e); }
    void operator()(Boolean_conv const& e)       { p.postfix_expression(e); }
    void operator()(Integer_conv const& e)       { p.postfix_expression(e); }
    void operator()(Float_conv const& e)         { p.postfix_expression(e); }
    void operator()(Numeric_conv const& e)       { p.postfix_expression(e); }
    void operator()(Dependent_conv const& e)     { p.postfix_expression(e); }
    void operator()(Ellipsis_conv const& e)      { p.postfix_expression(e); }
  };
  apply(e, fn{*this});
}


// TODO: We may need to specialize the printing for resolved declarations
// in case of ambiguous names from different base classes.
void
Printer::postfix_expression(Dot_expr const& e)
{
  postfix_expression(e.object());
  token(dot_tok);
  id(e.member());
}


void
Printer::postfix_expression(Call_expr const& e)
{
  postfix_expression(e.function());
  token(lparen_tok);
  Expr_list const& p = e.arguments();
  for (auto iter = p.begin(); iter != p.end(); ++iter) {
    expression(*iter);
    if (std::next(iter) != p.end())
      token(comma_tok);
  }
  token(rparen_tok);
}


void
Printer::postfix_expression(Tuple_expr const& e)
{
  token(lbrace_tok);
  Expr_list const& p = e.elements();
  for (auto iter = p.begin(); iter != p.end(); ++iter) {
    expression(*iter);
    if (std::next(iter) != p.end())
      token(comma_tok);
  }
  token(rbrace_tok);
}


void
Printer::postfix_expression(Value_conv const& e)
{
  token("__convert_to_value");
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::postfix_expression(Qualification_conv const& e)
{
  // TODO: Be more specific about the qualification added.
  token("__adjust_qualification");
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::postfix_expression(Boolean_conv const& e)
{
  token("__convert_to_bool");
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::postfix_expression(Integer_conv const& e)
{
  token("__widen_integer");
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::postfix_expression(Float_conv const& e)
{
  token("__widen_float");
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::postfix_expression(Numeric_conv const& e)
{
  token("__convert_to_float");
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::postfix_expression(Dependent_conv const& e)
{
  token("__dependent_conversion");
  token(lt_tok);
  type(e.type());
  token(gt_tok);
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::postfix_expression(Ellipsis_conv const& e)
{
  token("__convert_to_ellipsis");
  token(lparen_tok);
  expression(e.source());
  token(rparen_tok);
}


void
Printer::primary_expression(Expr const& e)
{
  struct fn
  {
    Printer& p;

    void operator()(Expr const& e)           { p.grouped_expression(e); }
    void operator()(Boolean_expr const& e)   { p.literal(e); }
    void operator()(Integer_expr const& e)   { p.literal(e); }
    void operator()(Real_expr const& e)      { p.literal(e); }
    void operator()(Id_expr const& e)        { p.id_expression(e); }
    void operator()(Decl_expr const& e)      { p.id_expression(e); }
    void operator()(Check_expr const& e)     { p.id_expression(e); }
    void operator()(Synthetic_expr const& e) { p.id_expression(e); }
    void operator()(Requires_expr const& e)  { p.requires_expression(e); }
  };
  apply(e, fn{*this});
}


void
Printer::grouped_expression(Expr const& e)
{
  token(lparen_tok);
  std::cout << "HERE\n";
  expression(e);
  token(rparen_tok);
}


void
Printer::literal(Boolean_expr const& e)
{
  if (e.value())
    token(true_tok);
  else
    token(false_tok);
}


void
Printer::literal(Integer_expr const& e)
{
  // FIXME: Provide a to_string for the Integer class. Also, it might be
  // nice to track radixes as part of the type so we don't have to print
  // everything in base 10.
  Integer_type const& t = cast<Integer_type>(e.type());
  Integer const& n = e.value();
  String const& s = n.impl().toString(10, t.is_signed());
  token(s);
}


void
Printer::literal(Real_expr const& e)
{
  token("<real>");
}


// Write the identifier used in the unresolved id-expression.
void
Printer::id_expression(Id_expr const& e)
{
  id(e.id());
}


// Write the qualified name of the referenced declaration.
void
Printer::id_expression(Decl_expr const& e)
{
  id(e.declaration().name());
}


void
Printer::id_expression(Check_expr const& e)
{
  id(e.declaration().name());
  token(lt_tok);
  template_argument_list(e.arguments());
  token(gt_tok);
}


void
Printer::id_expression(Synthetic_expr const& e)
{
  id(e.declaration().name());
}


void
Printer::requires_expression(Requires_expr const& e)
{
  token(requires_tok);
  space();

  // FIXME: Print template parameters.

  Decl_list const& nparms = e.normal_parameters();
  if (!nparms.empty()) {
    token(lparen_tok);
    parameter_list(nparms);
    token(rparen_tok);
    space();
  }

  token(lbrace_tok);
  newline_and_indent();
  usage_seq(e.requirements());
  newline_and_undent();
  token(rbrace_tok);
}


// -------------------------------------------------------------------------- //
// Printing of statements

void
Printer::statement(Stmt const& s)
{
  struct fn
  {
    Printer& p;
    void operator()(Stmt const& s)             { lingo_unhandled(s); }
    void operator()(Unparsed_stmt const& s)    { p.statement(s); }
    void operator()(Translation_stmt const& s) { p.translation_statement(s); }
    void operator()(Member_stmt const& s)      { p.member_statement(s); }
    void operator()(Compound_stmt const& s)    { p.compound_statement(s); }
    void operator()(Return_stmt const& s)      { p.return_statement(s); }
    void operator()(Yield_stmt const& s)       { p.yield_statement(s); }
    void operator()(If_then_stmt const& s)     { p.if_statement(s); }
    void operator()(If_else_stmt const& s)     { p.if_statement(s); }
    void operator()(While_stmt const& s)       { p.while_statement(s); }
    void operator()(Break_stmt const& s)       { p.break_statement(s); }
    void operator()(Continue_stmt const& s)    { p.continue_statement(s); }
    void operator()(Expression_stmt const& s)  { p.expression_statement(s); }
    void operator()(Declaration_stmt const& s) { p.declaration_statement(s); }
  };
  apply(s, fn{*this});
}


void
Printer::statement(Unparsed_stmt const& s)
{
  token(lt_tok);
  token(bar_tok);
  tokens(s.tokens());
  token(bar_tok);
  token(gt_tok);
}


void
Printer::statement_seq(Stmt_list const& ss)
{
  for (auto iter = ss.begin(); iter != ss.end(); ++iter) {
    statement(*iter);
    if (std::next(iter) != ss.end())
      newline();
  }
}


void
Printer::empty_statement(Empty_stmt const& s)
{
  token(semicolon_tok);
}


void
Printer::translation_statement(Translation_stmt const& s)
{
  statement_seq(s.statements());
}


void
Printer::member_statement(Member_stmt const& s)
{
  token(lbrace_tok);
  newline_and_indent();
  statement_seq(s.statements());
  newline_and_undent();
  token(rbrace_tok);
}


void
Printer::compound_statement(Compound_stmt const& s)
{
  token(lbrace_tok);
  if (!s.statements().empty()) {
    newline_and_indent();
    statement_seq(s.statements());
    newline_and_undent();
  }
  token(rbrace_tok);
}


void
Printer::return_statement(Return_stmt const& s)
{
  token(return_tok);
  space();
  expression(s.expression());
  token(semicolon_tok);
}

void
Printer::yield_statement(Yield_stmt const& s)
{
  token(yield_tok);
  space();
  expression(s.expression());
  token(semicolon_tok);
}

// TODO: If the branch is not compound statement, then drop to the next
// line and indent, so it prints like this:
//
//    if (expr)
//      stmt;
void
Printer::if_statement(If_then_stmt const& s)
{
  token(if_tok);
  space();
  token(lparen_tok);
  expression(s.condition());
  token(rparen_tok);
  space();
  statement(s.true_branch());
}


// TODO: See notes above.
void
Printer::if_statement(If_else_stmt const& s)
{
  token(if_tok);
  space();
  token(lparen_tok);
  expression(s.condition());
  token(rparen_tok);
  space();
  statement(s.true_branch());
  newline();
  token(else_tok);
  space();
  statement(s.false_branch());
}


void
Printer::while_statement(While_stmt const& s)
{
  token(while_tok);
  space();
  token(lparen_tok);
  expression(s.condition());
  token(rparen_tok);
  space();
  statement(s.body());
}


void
Printer::break_statement(Break_stmt const& s)
{
  token(break_tok);
  token(semicolon_tok);
}


void
Printer::continue_statement(Continue_stmt const& s)
{
  token(continue_tok);
  token(semicolon_tok);
}


void
Printer::expression_statement(Expression_stmt const& s)
{
  expression(s.expression());
  token(semicolon_tok);
}


void
Printer::declaration_statement(Declaration_stmt const& s)
{
  declaration(s.declaration());
}


// -------------------------------------------------------------------------- //
// Initializers


// FIXME: handle brace forms of the initializer.
void
Printer::initializer(Expr const& e)
{
  expression(e);
}




// -------------------------------------------------------------------------- //
// Declarations

void
Printer::declaration(Decl const& d)
{
  struct fn
  {
    Printer& p;

    void operator()(Decl const& d)           { lingo_unhandled(d); }
    void operator()(Super_decl const& d)     { p.super_declaration(d); }
    void operator()(Variable_decl const& d)  { p.variable_declaration(d); }
    void operator()(Function_decl const& d)  { p.function_declaration(d); }
    void operator()(Class_decl const& d)     { p.class_declaration(d); }
    void operator()(Coroutine_decl const& d) { p.coroutine_delcaration(d); }

    // Support emitting these here so we can print parameters without
    // an appropriate context.
    void operator()(Object_parm const& d)    { p.parameter(d); }
    void operator()(Value_parm const& d)     { p.value_template_parameter(d); }
    void operator()(Type_parm const& d)      { p.type_template_parameter(d); }
    void operator()(Template_parm const& d)  { p.template_template_parameter(d); }
  };

  // Print specifiers before the declaration.
  specifier_seq(d.specifiers());
  apply(d, fn{*this});
}


void
Printer::declaration_seq(Decl_list const& ds)
{
  for (Decl const& d : ds) {
    declaration(d);
    newline();
  }
}

// -------------------------------------------------------------------------- //
// Super declarations

void
Printer::super_declaration(Super_decl const& d)
{
  token(super_tok);
  space();
  //identifier(d);
  binary_operator(colon_tok);
  space();
  type(d.type());
  token(semicolon_tok);

}

// Write the specifier token followed by a space.
void
Printer::specifier(Token_kind k)
{
  token(k);
  space();
}


void
Printer::specifier_seq(Specifier_set s)
{
  if (s & static_spec)
    specifier(static_tok);
  if (s & dynamic_spec)
    specifier(dynamic_tok);
  if (s & implicit_spec)
    specifier(implicit_tok);
  if (s & explicit_spec)
    specifier(explicit_tok);
  if (s & virtual_spec)
    specifier(virtual_tok);
  if (s & abstract_spec)
    specifier(abstract_tok);
  if (s & inline_spec)
    specifier(inline_tok);
  if (s & public_spec)
    specifier(public_tok);
  if (s & private_spec)
    specifier(private_tok);
  if (s & protected_spec)
    specifier(protected_tok);
  if (s & in_spec)
    specifier(in_tok);
  if (s & out_spec)
    specifier(out_tok);
  if (s & mutable_spec)
    specifier(mutable_tok);
  if (s & consume_spec)
    specifier(consume_tok);
  if (s & const_spec)
    specifier(const_tok);
}


// -------------------------------------------------------------------------- //
// Variable declarations

void
Printer::variable_declaration(Variable_decl const& d)
{
  token(var_tok);
  space();
  identifier(d);
  binary_operator(colon_tok);
  type(d.type());
  variable_initializer(d.initializer());
  token(semicolon_tok);
}


void
Printer::variable_initializer(Def const& d)
{
  struct fn
  {
    Printer& p;
    void operator()(Def const& d)            { lingo_unhandled(d); }
    void operator()(Empty_def const& d)      { p.variable_initializer(d); }
    void operator()(Expression_def const& d) { p.variable_initializer(d); }
  };
  apply(d, fn{*this});
}


// TODO: Maybe print a comment?
void
Printer::variable_initializer(Empty_def const& d)
{ }


void
Printer::variable_initializer(Expression_def const& d)
{
  binary_operator(eq_tok);
  expression(d.expression());
}


// -------------------------------------------------------------------------- //
// Function declarations

void
Printer::function_declaration(Function_decl const& d)
{
  token(def_tok);
  space();
  identifier(d);
  binary_operator(colon_tok);
  token(lparen_tok);
  parameter_list(d.parameters());
  token(rparen_tok);
  binary_operator(arrow_tok);
  type(d.return_type());
  function_definition(d.definition());
}

void
Printer::coroutine_delcaration(Coroutine_decl const& d)
{
  token(coroutine_tok);
  space();
  identifier(d);
  binary_operator(colon_tok);
  token(lparen_tok);
  parameter_list(d.parameters());
  token(rparen_tok);
  binary_operator(arrow_tok);
  type(d.return_type());
  function_definition(d.definition());
}

void
Printer::function_definition(Def const& d)
{
  struct fn
  {
    Printer& p;
    void operator()(Def const& d)            { lingo_unhandled(d); }
    void operator()(Function_def const& d)   { p.function_definition(d); }
    void operator()(Expression_def const& d) { p.function_definition(d); }
    void operator()(Deleted_def const& d)    { p.function_definition(d); }
    void operator()(Defaulted_def const& d)  { p.function_definition(d); }
  };
  apply(d, fn{*this});
}

void
Printer::function_definition(Function_def const& d)
{
  newline();
  statement(d.statement());
}


void
Printer::function_definition(Expression_def const& d)
{
  binary_operator(eq_tok);
  expression(d.expression());
  token(semicolon_tok);
}


void
Printer::function_definition(Deleted_def const&)
{
  binary_operator(eq_tok);
  token(delete_tok);
}


void
Printer::function_definition(Defaulted_def const&)
{
  binary_operator(eq_tok);
  token(default_tok);
}


// -------------------------------------------------------------------------- //
// Class declarations

void
Printer::class_declaration(Class_decl const& d)
{
  token(class_tok);
  space();
  identifier(d);
  binary_operator(colon_tok);
  type(d.type());
  class_definition(d.definition());
}


void
Printer::class_definition(Def const& d)
{
  struct fn
  {
    Printer& p;
    void operator()(Def const& d)         { lingo_unhandled(d); }
    void operator()(Class_def const& d)   { p.class_definition(d); }
    void operator()(Deleted_def const& d) { p.class_definition(d); }
  };
  apply(d, fn{*this});
}


void
Printer::class_definition(Class_def const& d)
{
  newline();
  statement(d.body());
}


// -------------------------------------------------------------------------- //
// Template declarations

void
Printer::template_declaration(Template_decl const& d)
{
  token(template_tok);
  token(lt_tok);
  template_parameter_list(d.parameters());
  token(gt_tok);
  newline();
  declaration(d.parameterized_declaration());
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


void
Printer::template_parameter(Decl const& d)
{
  struct fn
  {
    Printer& p;
    void operator()(Decl const& d)      { lingo_unhandled(d); }
    void operator()(Type_parm const& d) { p.type_template_parameter(d); }
  };
  apply(d, fn{*this});
}


// FIXME: Handle default arguments.
void
Printer::type_template_parameter(Type_parm const& d)
{
  token(typename_tok);
  id(d.name());
}


// FIXME: Default arguments.
void
Printer::value_template_parameter(Value_parm const& d)
{
  token(const_tok);
  id(d.name());
}


// FIXME: Actually implement me.
void
Printer::template_template_parameter(Template_parm const& d)
{
  token(template_tok);
  id(d.name());
}



// -------------------------------------------------------------------------- //
// Concept declarations

void
Printer::concept_definition(Expression_def const& d)
{
  token(eq_tok);
  space();
  expression(d.expression());
  token(semicolon_tok);
}


void
Printer::concept_definition(Concept_def const& d)
{
  token(lbrace_tok);
  newline_and_indent();
  concept_member_seq(d.requirements());
  newline_and_undent();
  token(rbrace_tok);
}


void
Printer::concept_member_seq(Req_list const& rs)
{
  for (auto iter = rs.begin(); iter != rs.end(); ++iter) {
    concept_member(*iter);
    if (std::next(iter) != rs.end())
      newline();
  }
}


void
Printer::concept_member(Req const& r)
{
  struct fn
  {
    Printer& p;
    void operator()(Req const& r)            { lingo_unreachable(); }
    void operator()(Syntactic_req const& r)  { p.concept_member(r); }
    void operator()(Semantic_req const& r)   { p.concept_member(r); }
    void operator()(Type_req const& r)       { p.concept_member(r); }
    void operator()(Expression_req const& r) { p.concept_member(r); }
  };
  apply(r, fn{*this});
}


void
Printer::concept_member(Syntactic_req const& r)
{
  expression(r.expression());
}


void
Printer::concept_member(Semantic_req const& r)
{
  declaration(r.declaration());
}


void
Printer::concept_member(Type_req const& r)
{
  token(typename_tok);
  space();
  type(r.type());
  token(semicolon_tok);
}


void
Printer::concept_member(Expression_req const& r)
{
  expression(r.expression());
  token(semicolon_tok);
}


void
Printer::requires_clause(Expr const& e)
{
  token(requires_tok);
  space();
  expression(e);
}


void
Printer::parameter(Decl const& d)
{
  struct parameter_fn
  {
    Printer& p;
    void operator()(Decl const& d) { lingo_unhandled(d); }
    void operator()(Object_parm const& d)   { p.parameter(d); }
    void operator()(Value_parm const& d)    { p.value_template_parameter(d); }
    void operator()(Type_parm const& d)     { p.type_template_parameter(d); }
    void operator()(Template_parm const& d) { p.template_template_parameter(d); }
  };

  specifier_seq(d.specifiers());
  apply(d, parameter_fn{*this});
}


// TODO: Print the default argument.
void
Printer::parameter(Object_parm const& p)
{
  identifier(p);
  binary_operator(colon_tok);
  type(p.type());
}


void
Printer::parameter_list(Decl_list const& d)
{
  for (auto iter = d.begin(); iter != d.end(); ++iter) {
    parameter(*iter);
    if (std::next(iter) != d.end()) {
      token(comma_tok);
      space();
    }
  }
}


// FIXME: Print the qualified name for template arguments?
void
Printer::template_argument(Term const& a)
{
  if (Type const* t = as<Type>(&a))
    type(*t);
  else if (Expr const* e = as<Expr>(&a))
    expression(*e);
  else if (Decl const* d = as<Decl>(&a))
    id(d->name());
  else
    lingo_unreachable();
}


void
Printer::template_argument_list(Term_list const& ts)
{
  for (auto iter = ts.begin(); iter != ts.end(); ++iter) {
    template_argument(*iter);
    if (std::next(iter) != ts.end())
      token(comma_tok);
  }
}

// -------------------------------------------------------------------------- //
// Requirements

void
Printer::usage_seq(Req_list const& rs)
{
  for (auto iter = rs.begin(); iter != rs.end(); ++iter) {
    usage_requirement(*iter);
    if (std::next(iter) != rs.end())
      newline();
  }
}


void
Printer::usage_requirement(Req const& r)
{
  struct fn
  {
    Printer& p;
    void operator()(Req const& r)            { banjo_unhandled_case(r); }
    void operator()(Basic_req const& r)      { p.requirement(r); }
    void operator()(Conversion_req const& r) { p.requirement(r); }
    void operator()(Deduction_req const& r)  { p.requirement(r); }
  };
  apply(r, fn{*this});
}


void
Printer::requirement(Basic_req const& r)
{
  expression(r.expression());
  space();
  token(colon_tok);
  space();
  type(r.type());
  token(semicolon_tok);
}


void
Printer::requirement(Conversion_req const& r)
{
  expression(r.expression());
  space();
  token(arrow_tok);
  space();
  type(r.type());
  token(semicolon_tok);
}


void
Printer::requirement(Deduction_req const& r)
{
  expression(r.expression());
  token(colon_tok);
  type(r.type());
  token(semicolon_tok);
}


// -------------------------------------------------------------------------- //
// Constraints

void
Printer::constraint(Cons const& c)
{
  struct fn
  {
    Printer& p;
    void operator()(Cons const& c)               { banjo_unhandled_case(c); }
    void operator()(Concept_cons const& c)       { p.constraint(c); }
    void operator()(Predicate_cons const& c)     { p.constraint(c); }
    void operator()(Expression_cons const& c)    { p.constraint(c); }
    void operator()(Conversion_cons const& c)    { p.constraint(c); }
    void operator()(Parameterized_cons const& c) { p.constraint(c); }
    void operator()(Conjunction_cons const& c)   { p.constraint(c); }
    void operator()(Disjunction_cons const& c)   { p.constraint(c); }
  };
  apply(c, fn{*this});
}


void
Printer::constraint(Concept_cons const& c)
{
  id(c.declaration().name());
  token(lt_tok);
  template_argument_list(c.arguments());
  token(gt_tok);
}


// Write this as [e]
void
Printer::constraint(Predicate_cons const& c)
{
  token(lbracket_tok);
  expression(c.expression());
  token(rbracket_tok);
}


// Write this as |e : t|
void
Printer::constraint(Expression_cons const& c)
{
  token(bar_tok);
  expression(c.expression());
  space();
  token(colon_tok);
  space();
  type(c.type());
  token(bar_tok);
}


// Write this as |e -> t|
void
Printer::constraint(Conversion_cons const& c)
{
  token(bar_tok);
  expression(c.expression());
  space();
  token(arrow_tok);
  space();
  type(c.type());
  token(bar_tok);
}


void
Printer::constraint(Parameterized_cons const& c)
{
  token("\\");
  token(lparen_tok);
  parameter_list(c.variables());
  token(rparen_tok);
  space();
  constraint(c.constraint());
}


void
Printer::constraint(Conjunction_cons const& c)
{
  grouped_constraint(c.left());
  space();
  token("/\\");
  space();
  grouped_constraint(c.right());
}


void
Printer::constraint(Disjunction_cons const& c)
{
  grouped_constraint(c.left());
  space();
  token("\\/");
  space();
  grouped_constraint(c.right());
}


// Write parens for every non-atomic constraint. The language
// doesn't have explicit precedence.
void
Printer::grouped_constraint(Cons const& c)
{
  struct fn
  {
    bool operator()(Cons const& c) const        { return false; }
    bool operator()(Binary_cons const& c) const { return true; }
  };

  if (apply(c, fn{})) {
    token(lparen_tok);
    constraint(c);
    token(rparen_tok);
  } else {
    constraint(c);
  }
}


// -------------------------------------------------------------------------- //
// Streaming


std::ostream&
operator<<(std::ostream& os, Term const& x)
{
  if (Name const* n = as<Name>(&x))
    return os << *n;
  if (Type const* t = as<Type>(&x))
    return os << *t;
  if (Expr const* e = as<Expr>(&x))
    return os << *e;
  if (Decl const* d = as<Decl>(&x))
    return os << *d;
  lingo_unreachable();
}


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
operator<<(std::ostream& os, Stmt const& s)
{
  Printer print(os);
  print(s);
  return os;
}


std::ostream&
operator<<(std::ostream& os, Decl const& d)
{
  Printer print(os);
  print(d);
  return os;
}


std::ostream&
operator<<(std::ostream& os, Cons const& c)
{
  Printer print(os);
  print(c);
  return os;
}


} // namespace banjo
