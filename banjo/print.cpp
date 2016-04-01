// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "print.hpp"
#include "ast.hpp"

#include <iterator>
#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Lexical items


// Conditionally print whitespace based on this and the
// previous token.
void
Printer::space(Token_info info)
{
  if (prev) {
    // Always put space after a comma.
    if (prev == comma_tok)
      space();

    // Insert a space between all words.
    else if (is_word(prev) && is_word(info))
      space();

    // Guarantee spaces around binary operators.
    else if (prev == binary_op_use || info == binary_op_use)
      space();

    // FIXME: Finish all the spacing rules.
  }
}


// Unconditionally print whitespace. Clear the previous token.
void
Printer::space()
{
  os << ' ';
  prev.clear();
}


// Print a new line and indent. Clear the previous token.
void
Printer::newline()
{
  os << '\n' << std::string(2 * indent, ' ');
  prev.clear();
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


// Print the token. Print an extra space after keywords
// and commas.
void
Printer::token(Token_kind k)
{
  token(k, general_use);
}


void
Printer::token(Token_kind k, Token_use u)
{
  Token_info info(k, u);
  space(info);
  os << get_spelling(k);
  prev = info;
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


// Print a string as an identifier.
void
Printer::token(String const& str)
{
  space(identifier_tok);
  os << str;
  prev = identifier_tok;
}


// Print n as an integer token.
void
Printer::token(int n)
{
  os << n;
  prev = integer_tok;
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
    unqualified_id(d->declared_name());
    token(colon_colon_tok);
  }
}


// -------------------------------------------------------------------------- //
// Printing of types

// Returns the precedence of a type. Note that the precedence of
// function types is poisoned so that it always requires parens
// when writing it out.
//
// Maybe this indicates that function types should be the lowest
// precedence parse. Err... function types are annoying.
int
precedence(Type const& t)
{
  struct fn
  {
    int operator()(Void_type const& t)      { return 0; }
    int operator()(Boolean_type const& t)   { return 0; }
    int operator()(Integer_type const& t)   { return 0; }
    int operator()(Byte_type const& t)      { return 0; }
    int operator()(Float_type const& t)     { return 0; }
    int operator()(Auto_type const& t)      { return 0; }
    int operator()(Decltype_type const& t)  { return 0; }
    int operator()(Declauto_type const& t)  { return 0; }
    int operator()(Function_type const& t)  { return 99; }
    int operator()(Qualified_type const& t) { return 1; }
    int operator()(Pointer_type const& t)   { return 1; }
    int operator()(Reference_type const& t) { return 3; }
    int operator()(Array_type const& t)     { return 1; }
    int operator()(Sequence_type const& t)  { return 2; }
    int operator()(Class_type const& t)     { return 0; }
    int operator()(Union_type const& t)     { return 0; }
    int operator()(Enum_type const& t)      { return 0; }
    int operator()(Typename_type const& t)  { return 0; }
    int operator()(Synthetic_type const& t) { return 0; }
  };
  return apply(t, fn{});
}


void
Printer::type(Type const& t)
{
  struct fn
  {
    Printer& p;
    void operator()(Type const& t)           { lingo_unimplemented(); }
    void operator()(Void_type const& t)      { p.simple_type(t); }
    void operator()(Boolean_type const& t)   { p.simple_type(t); }
    void operator()(Byte_type const& t)      { p.simple_type(t); }
    void operator()(Integer_type const& t)   { p.simple_type(t); }
    void operator()(Float_type const& t)     { p.simple_type(t); }
    void operator()(Auto_type const& t)      { p.simple_type(t); }
    void operator()(Decltype_type const& t)  { p.simple_type(t); }
    void operator()(Declauto_type const& t)  { p.simple_type(t); }
    void operator()(Function_type const& t)  { p.simple_type(t); }
    void operator()(Qualified_type const& t) { p.postfix_type(t); }
    void operator()(Pointer_type const& t)   { p.postfix_type(t); }
    void operator()(Reference_type const& t) { p.reference_type(t); }
    void operator()(Array_type const& t)     { p.postfix_type(t); }
    void operator()(Sequence_type const& t)  { p.sequence_type(t); }
    void operator()(Class_type const& t)     { p.simple_type(t); }
    void operator()(Union_type const& t)     { lingo_unimplemented(); }
    void operator()(Enum_type const& t)      { lingo_unimplemented(); }
    void operator()(Typename_type const& t)  { p.simple_type(t); }
    void operator()(Synthetic_type const& t) { p.simple_type(t); }
  };
  apply(t, fn{*this});
}


void
Printer::simple_type(Void_type const& t)
{
  token(void_tok);
}


void
Printer::simple_type(Boolean_type const& t)
{
  token(bool_tok);
}


void
Printer::simple_type(Byte_type const& t)
{
  token(byte_tok);
}


// FIXME: Map this back to a token.
void
Printer::simple_type(Integer_type const& t)
{
  std::stringstream ss;
  if (t.is_unsigned())
    ss << 'u';
  ss << "int" << t.precision();
  token(ss.str().c_str());
}


// FIXME: Map this back to a token.
void
Printer::simple_type(Float_type const& t)
{
  std::stringstream ss;
  ss << "float" << t.precision();
  token(ss.str().c_str());
}


void
Printer::simple_type(Auto_type const& t)
{
  token(auto_tok);
}


// TODO: Implement me.
void
Printer::simple_type(Decltype_type const& t)
{
  lingo_unreachable();
}


void
Printer::simple_type(Declauto_type const& t)
{
  token(decltype_tok);
  token(lparen_tok);
  token(auto_tok);
  token(rparen_tok);
}


void
Printer::simple_type(Function_type const& t)
{
  token(lparen_tok);
  Type_list const& p = t.parameter_types();
  for (auto iter = p.begin(); iter != p.end(); ++iter) {
    type(*iter);
    if (std::next(iter) != p.end())
      token(comma_tok);
  }
  token(rparen_tok);
  return_type(t.return_type());
}


// FIXME: Print a qualification of the name that uniquely
// identifiers the type, given the current context. Naturally,
// this means we need to track scopes...
void
Printer::simple_type(Class_type const& t)
{
  id(t.declaration().name());
}


// FIXME: Print the qualified id? Print a qualification that
// guarantees unique naming?
void
Printer::simple_type(Typename_type const& t)
{
  id(t.declaration().name());
}


void
Printer::simple_type(Synthetic_type const& t)
{
  id(t.declaration().name());
}


void
Printer::grouped_type(Type const& t, Type const& s)
{
  if (precedence(t) < precedence(s)) {
    token(lparen_tok);
    type(s);
    token(rparen_tok);
  } else {
    type(s);
  }
}


void
Printer::postfix_type(Pointer_type const& t)
{
  grouped_type(t, t.type());
  token(star_tok);
}


void
Printer::postfix_type(Qualified_type const& t)
{
  grouped_type(t, t.type());
  if (t.is_const())
    token(const_tok);
  if (t.is_volatile())
    token(volatile_tok);
}


void
Printer::postfix_type(Array_type const& t)
{
  lingo_unimplemented();
}


void
Printer::sequence_type(Sequence_type const& t)
{
  grouped_type(t, t.type());
  token(lbracket_tok);
  token(rbracket_tok);
}


void
Printer::reference_type(Reference_type const& t)
{
  grouped_type(t, t.type());
  token(amp_tok);
}


void
Printer::return_type(Type const& t)
{
  token(arrow_tok, binary_op_use);
  type(t);
}


// -------------------------------------------------------------------------- //
// Expressions


// Returns the precedence of the operator for an expression.
int
precedence(Expr const& e)
{
  struct fn
  {
    int operator()(Expr const& e)           { lingo_unimplemented(); }
    int operator()(Boolean_expr const& e)   { return 0; }
    int operator()(Integer_expr const& e)   { return 0; }
    int operator()(Real_expr const& e)      { return 0; }
    int operator()(Reference_expr const& e) { return 0; }
    int operator()(Check_expr const& e)     { return 0; }
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
    int operator()(Call_expr const& e)      { return 2; }
    int operator()(Assign_expr const& e)    { return 15; }
    int operator()(Requires_expr const& e)  { return 0; }
    int operator()(Conv const& e)           { return precedence(e.source()); }
    int operator()(Init const& e)           { lingo_unreachable(); }
    int operator()(Bind_init const& e)      { return precedence(e.expression()); }
    int operator()(Copy_init const& e)      { return precedence(e.expression()); }
  };
  return apply(e, fn{});
}


void
Printer::expression(Expr const& e)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const& e)               { lingo_unimplemented(); }
    void operator()(Boolean_expr const& e)       { p.literal(e); }
    void operator()(Integer_expr const& e)       { p.literal(e); }
    void operator()(Real_expr const& e)          { p.literal(e); }
    void operator()(Reference_expr const& e)     { p.id_expression(e); }
    void operator()(Check_expr const& e)         { p.id_expression(e); }
    void operator()(Add_expr const& e)           { p.binary_expression(e, plus_tok); }
    void operator()(Sub_expr const& e)           { p.binary_expression(e, minus_tok); }
    void operator()(Mul_expr const& e)           { p.binary_expression(e, star_tok); }
    void operator()(Div_expr const& e)           { p.binary_expression(e, slash_tok); }
    void operator()(Rem_expr const& e)           { p.binary_expression(e, percent_tok); }
    void operator()(Neg_expr const& e)           { p.unary_expression(e, minus_tok); }
    void operator()(Pos_expr const& e)           { p.unary_expression(e, plus_tok); }
    void operator()(Eq_expr const& e)            { p.binary_expression(e, eq_eq_tok); }
    void operator()(Ne_expr const& e)            { p.binary_expression(e, bang_eq_tok); }
    void operator()(Lt_expr const& e)            { p.binary_expression(e, lt_tok); }
    void operator()(Gt_expr const& e)            { p.binary_expression(e, gt_tok); }
    void operator()(Le_expr const& e)            { p.binary_expression(e, lt_eq_tok); }
    void operator()(Ge_expr const& e)            { p.binary_expression(e, gt_eq_tok); }
    void operator()(And_expr const& e)           { p.binary_expression(e, amp_amp_tok); }
    void operator()(Or_expr const& e)            { p.binary_expression(e, bar_bar_tok); }
    void operator()(Not_expr const& e)           { p.unary_expression(e, bang_tok); }
    void operator()(Call_expr const& e)          { p.postfix_expression(e); }
    void operator()(Assign_expr const& e)        { p.binary_expression(e, eq_tok); }
    void operator()(Requires_expr const& e)      { p.requires_expression(e); }
    void operator()(Synthetic_expr const& e)     { p.id_expression(e); }
    void operator()(Value_conv const& e)         { p.postfix_expression(e); }
    void operator()(Qualification_conv const& e) { p.postfix_expression(e); }
    void operator()(Integer_conv const& e)       { p.postfix_expression(e); }
    void operator()(Boolean_conv const& e)       { p.postfix_expression(e); }
    void operator()(Float_conv const& e)         { p.postfix_expression(e); }
    void operator()(Numeric_conv const& e)       { p.postfix_expression(e); }
    void operator()(Dependent_conv const& e)     { p.postfix_expression(e); }
    void operator()(Ellipsis_conv const& e)      { p.postfix_expression(e); }
    void operator()(Init const& e)               { lingo_unreachable(); }

    // TODO: Certain forms of initialization are transparent.
    // Do we need to support direct and aggregate initialization
    // as expressions also? Presumably not.
    void operator()(Copy_init const& e)          { p.expression(e.expression()); }
    void operator()(Bind_init const& e)          { p.expression(e.expression()); }
};
  apply(e, fn{*this});
}


// TODO: All of the literal expressions can be unified into
// a single function.
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


void
Printer::id_expression(Reference_expr const& e)
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
Printer::postfix_expression(Call_expr const& e)
{
  grouped_expression(e, e.function());
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
Printer::unary_expression(Unary_expr const& e, Token_kind k)
{
  token(k, unary_op_use);
  grouped_expression(e, e.operand());
}


void
Printer::binary_expression(Binary_expr const& e, Token_kind k)
{
  grouped_expression(e, e.left());
  token(k, binary_op_use);
  grouped_expression(e, e.right());
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
// Definitions


void
Printer::statement(Stmt const& s)
{
  struct fn
  {
    Printer& p;
    void operator()(Stmt const&)               { lingo_unimplemented(); }
    void operator()(Compound_stmt const& s)    { p.compound_statement(s); }
    void operator()(Return_stmt const& s)      { p.return_statement(s); }
    void operator()(Expression_stmt const& s)  { p.expression_statement(s); }
    void operator()(Declaration_stmt const& s) { p.declaration_statement(s); }
  };
  apply(s, fn{*this});
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
Printer::compound_statement(Compound_stmt const& s)
{
  token(lbrace_tok);
  newline_and_indent();
  statement_seq(s.statements());
  newline_and_undent();
  token(rbrace_tok);
}


void
Printer::return_statement(Return_stmt const& s)
{
  token(return_tok);
  expression(s.expression());
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
//
// Select a canonical form of initialization based on the semantics
// selected by initialization.

void
Printer::initializer(Expr const& e)
{
  if (is<Init>(&e))
    initializer(cast<Init>(e));
  else
    lingo_unreachable();
}


void
Printer::initializer(Init const& i)
{
  struct fn
  {
    Printer& p;
    void operator()(Expr const&)             { lingo_unreachable(); }
    void operator()(Trivial_init const& i)   { }
    void operator()(Copy_init const& i)      { p.equal_initializer(i); }
    void operator()(Bind_init const& i)      { p.equal_initializer(i); }
    void operator()(Direct_init const& i)    { p.paren_initializer(i); }
    void operator()(Aggregate_init const& i) { p.brace_initializer(i); }
  };
  apply(i, fn{*this});
}


void
Printer::equal_initializer(Copy_init const& i)
{
  token(eq_tok);
  space();
  expression(i.expression());
}


void
Printer::equal_initializer(Bind_init const& i)
{
  token(eq_tok);
  space();
  expression(i.expression());
}


// TODO: Implement me.
void
Printer::paren_initializer(Direct_init const& i)
{
  token("(...)");
}


// TODO: Implement me.
void
Printer::brace_initializer(Aggregate_init const&)
{
  token("{...}");
}


// -------------------------------------------------------------------------- //
// Definitions

void
Printer::function_definition(Def const& d)
{
  struct fn
  {
    Printer& p;
    void operator()(Def const&) { lingo_unimplemented(); }
    void operator()(Function_def const& d) { p.function_definition(d); }
    void operator()(Deleted_def const& d) { p.function_definition(d); }
    void operator()(Defaulted_def const& d) { p.function_definition(d); }
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
Printer::function_definition(Deleted_def const&)
{
  space();
  token(eq_tok);
  space();
  token(delete_tok);
}


void
Printer::function_definition(Defaulted_def const&)
{
  space();
  token(eq_tok);
  space();
  token(default_tok);
}


void
Printer::class_definition(Def const& d)
{
  struct fn
  {
    Printer& p;
    void operator()(Def const&) { lingo_unimplemented(); }
    void operator()(Class_def const& d) { p.class_definition(d); }
    void operator()(Deleted_def const& d) { p.class_definition(d); }
  };
  apply(d, fn{*this});
}


void
Printer::class_definition(Class_def const& d)
{
  if (d.members().empty()) {
    space();
    token(lbrace_tok);
    space();
    token(rbrace_tok);
  } else {
    newline();
    token(lbrace_tok);
    newline_and_indent();
    member_seq(d.members());
    newline_and_undent();
    token(rbrace_tok);
  }
}


void
Printer::member_seq(Decl_list const& ds)
{
  for (auto iter = ds.begin(); iter != ds.end(); ++iter) {
    declaration(*iter);
    if (std::next(iter) != ds.end())
      newline();
  }
}


void
Printer::class_definition(Deleted_def const&)
{
  space();
  token(eq_tok);
  space();
  token(delete_tok);
}


// -------------------------------------------------------------------------- //
// Declarations

void
Printer::declaration(Decl const& d)
{
  struct fn
  {
    Printer& p;

    void operator()(Decl const&) { lingo_unreachable(); }
    void operator()(Variable_decl const& d)  { p.variable_declaration(d); }
    void operator()(Constant_decl const& d)  { p.constant_declaration(d); }
    void operator()(Function_decl const& d)  { p.function_declaration(d); }
    void operator()(Class_decl const& d)     { p.class_declaration(d); }
    void operator()(Union_decl const& d)     { p.union_declaration(d); }
    void operator()(Enum_decl const& d)      { p.enum_declaration(d); }
    void operator()(Namespace_decl const& d) { p.namespace_declaration(d); }
    void operator()(Template_decl const& d)  { p.template_declaration(d); }
    void operator()(Concept_decl const& d)   { p.concept_declaration(d); }
    void operator()(Axiom_decl const& d)     { p.axiom_declaration(d); }

    // Support emitting these here so we can print parameters
    // without an appropriate context.
    void operator()(Object_parm const& d)    { p.parameter(d); }
    void operator()(Value_parm const& d)     { p.value_template_parameter(d); }
    void operator()(Type_parm const& d)      { p.type_template_parameter(d); }
    void operator()(Template_parm const& d)  { p.template_template_parameter(d); }
  };
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


// FIXME: Print the initializer.
void
Printer::variable_declaration(Variable_decl const& d)
{
  token(var_tok);
  type(d.type());
  space();
  id(d.name());
  if (d.has_initializer()) {
    // FIXME: This is printing a space for trivial initializers.
    space();
    initializer(d.initializer());
  }
  token(semicolon_tok);
}


// FIXME: Print the initializer.
void
Printer::constant_declaration(Constant_decl const& d)
{
  token(const_tok);
  type(d.type());
  id(d.name());
  token(semicolon_tok);
}


void
Printer::function_declaration(Function_decl const& d)
{
  token(def_tok);
  space();
  id(d.name());
  token(lparen_tok);
  parameter_list(d.parameters());
  token(rparen_tok);
  return_type(d.return_type());
  if (d.is_definition())
    function_definition(d.definition());
  else
    token(semicolon_tok);
}


void
Printer::class_declaration(Class_decl const& d)
{
  token(class_tok);
  id(d.name());
  if (d.is_definition())
    class_definition(d.definition());
  else
    token(semicolon_tok);
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
  if (d.is_global()) {
    declaration_seq(d.members());
  } else {
    token(namespace_tok);
    id(d.name());
    token(lbrace_tok);
    newline_and_indent();
    declaration_seq(d.members());
    newline_and_undent();
    token(rbrace_tok);
  }
}


void
Printer::template_declaration(Template_decl const& d)
{
  token(template_tok);
  token(lt_tok);
  template_parameter_list(d.parameters());
  token(gt_tok);
  if (d.is_constrained()) {
    newline_and_indent();
    requires_clause(d.constraint());
    newline_and_undent();
  } else {
    newline();
  }
  declaration(d.parameterized_declaration());
}


void
Printer::concept_declaration(Concept_decl const& d)
{
  token(concept_tok);
  id(d.name());
  token(lt_tok);
  template_parameter_list(d.parameters());
  token(gt_tok);
  space();
  if (d.is_defined()) {
    concept_definition(d.definition());
  } else {
    token(eq_tok);
    space();
    token(ellipsis_tok);
    token(semicolon_tok);
  }
}


void
Printer::concept_definition(Def const& d)
{
  struct fn
  {
    Printer& p;
    void operator()(Def const& d) { banjo_unhandled_case(d); }
    void operator()(Expression_def const& d) { p.concept_definition(d); }
    void operator()(Concept_def const& d)    { p.concept_definition(d); }
  };
  apply(d, fn{*this});
}


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
Printer::axiom_declaration(Axiom_decl const& d)
{
  token(axiom_tok);
  space();
  id(d.name());
  space();

  token(lparen_tok);
  parameter_list(d.parameters());
  token(rparen_tok);
  space();

  token(lbrace_tok);
  newline_and_indent();
  token("..."); // FIXME: Implement me!
  newline_and_undent();
  token(rbrace_tok);
}


void
Printer::requires_clause(Expr const& e)
{
  token(requires_tok);
  space();
  expression(e);
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
  space();
  id(p.name());
}


void
Printer::parameter(Variadic_parm const& p)
{
  token(ellipsis_tok);
}


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
