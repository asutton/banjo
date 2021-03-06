// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "debugging.hpp"
#include "ast.hpp"

#include <lingo/io.hpp>

#include <iterator>
#include <iostream>


namespace banjo
{

// An RAII class that wraps each term in parens and emits its
// implementing class name.
struct Sexpr
{
  Sexpr(Debug_printer& p, Term const& t)
    : p(p)
  {
    p.open();
    p.rep(t);
  }

  ~Sexpr()
  {
    p.close();
  }

  Debug_printer& p;
};


Debug_printer::Debug_printer(std::ostream& os)
  : os(os), color(), tree(false), indent(0)
{
  color = os.iword(lingo::ios_color_flag);
}


void
Debug_printer::open()
{
  os << '(';
}


void
Debug_printer::close()
{
  os << ')';
}


void
Debug_printer::space()
{
  os << ' ';
}


// Print a new line and indent to the current depth. Clear the previous token.
void
Debug_printer::newline()
{
  os << '\n' << std::string(2 * indent, ' ');
}


// Print a newline and indent one level.
void
Debug_printer::newline_and_indent()
{
  ++indent;
  newline();
}


// Print a newline and undent (back up) one level.
void
Debug_printer::newline_and_undent()
{
  --indent;
  newline();
}


// Print a newline and undent (back up) one level.
void
Debug_printer::undent_and_newline()
{
  newline();
  --indent;
}


void
Debug_printer::prop(char const* s)
{
  os << s << '=';
}


void
Debug_printer::rep(Term const& t)
{
  // Get a the shortest part of the name.
  String s = type_str(t);
  auto n = s.find_last_of(':');
  String name = s.substr(n + 1);

  // Print the name in pretty colors.
  if (color) {
    if (is<Name>(t))      os << lingo::bright_cyan(name);
    else if (is<Type>(t)) os << lingo::bright_blue(name);
    else if (is<Expr>(t)) os << lingo::bright_magenta(name);
    else if (is<Decl>(t)) os << lingo::bright_green(name);
    else                  os << lingo::bright(name);
  }
  else {
    os << name;
  }
}


void
Debug_printer::value(bool b)
{
  os << (b ? "true" : "false");
}


void
Debug_printer::value(Integer const& n)
{
  os << n;
}


// -------------------------------------------------------------------------- //
// Terms

void
Debug_printer::operator()(Term const& x)
{
  if (Name const* n = as<Name>(&x))
    id(*n);
  else if (Type const* t = as<Type>(&x))
    type(*t);
  else if (Expr const* e = as<Expr>(&x))
    expression(*e);
  else if (Stmt const* s = as<Stmt>(&x))
    statement(*s);
  else if (Decl const* d = as<Decl>(&x))
    declaration(*d);
  else {
    Sexpr guard (*this, x);
  }
}


// -------------------------------------------------------------------------- //
// Names

void
Debug_printer::id(Name const& n)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Name const& n)           { lingo_unhandled(n); }
    void operator()(Simple_id const& n)      { self.simple_id(n); }
    void operator()(Operator_id const& n)    { self.operator_id(n); }
    void operator()(Conversion_id const& n)  { self.conversion_id(n); }
    void operator()(Placeholder_id const& n) { self.placeholder_id(n); }
  };
  apply(n, fn{*this});
}


void
Debug_printer::simple_id(Simple_id const& n)
{
  Sexpr guard(*this, n);
  space();
  os << '"' << n.symbol().spelling() << '"';
}


void
Debug_printer::operator_id(Operator_id const& n)
{
  Sexpr guard(*this, n);
  space();
  os << n.spelling();
}


void
Debug_printer::conversion_id(Conversion_id const& n)
{
  Sexpr guard(*this, n);
  newline_and_indent();
  type(n.type());
  newline_and_undent();
}


void
Debug_printer::placeholder_id(Placeholder_id const& n)
{
  Sexpr guard(*this, n);
  space();
  os << n.number();
}


// -------------------------------------------------------------------------- //
// Types


void
Debug_printer::type(Type const& t)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Type const& t)          { lingo_unhandled(t); }
    void operator()(Void_type const& t)     { self.void_type(t); }
    void operator()(Boolean_type const& t)  { self.boolean_type(t); }
    void operator()(Integer_type const& t)  { self.integer_type(t); }
    void operator()(Function_type const& t) { self.function_type(t); }
  };
  Sexpr guard(*this, t);
  
  type_category(t);
  type_qualifiers(t);
  apply(t, fn{*this});
}


void
Debug_printer::void_type(Void_type const& t)
{
}


void
Debug_printer::boolean_type(Boolean_type const& t)
{
}


// TODO: Dump precision and sign.
void
Debug_printer::integer_type(Integer_type const& t)
{
  space();
  os << (t.is_signed() ? "signed" : " unsigned");
  space();
  os << t.precision() << 'b';
}


// TODO: Actually label fields... Also, get the tabbing right.
void
Debug_printer::function_type(Function_type const& t)
{
  newline_and_indent();
  os << "parameter-types=";
  open();
  newline_and_indent();
  Type_list const& parms = t.parameter_types();
  for (auto iter = parms.begin(); iter != parms.end(); ++iter) {
    type(*iter);
    if (std::next(iter) != parms.end())
      newline();
  }
  newline_and_undent();
  close();
  newline();
  os << "return-type=";
  type(t.return_type());
  newline_and_undent();
}


static inline char const*
category_name(Type_category c)
{
  switch (c) {
    case banjo::object_type: 
      return "object";
    case banjo::reference_type: 
      return "reference";
    case banjo::function_type: 
      return "function";
    default:  
      return "uncategorized-type";
  }  
}


void
Debug_printer::type_category(Type const& t)
{
  space();
  os << category_name(t.category());
}


void
Debug_printer::type_qualifiers(Type const& t)
{
  if (!t.is_qualified())
    return;

  Qualifier_set q = t.qualifiers();  
  if (q & const_qual)
    type_qualifier("const");
  if (q & volatile_qual)
    type_qualifier("volatile");
  if (q & meta_qual)
    type_qualifier("meta");
  if (q & consume_qual)
    type_qualifier("consume");
  if (q & noexcept_qual)
    type_qualifier("noexcept");
}


void
Debug_printer::type_qualifier(char const* q)
{
  space();
  os << q;
}


// -------------------------------------------------------------------------- //
// Expressions


// TODO: We should be printing type information for every expression.
void
Debug_printer::expression(Expr const& e)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Expr const& e)          { lingo_unhandled(e); }
    void operator()(Boolean_expr const& e)  { self.literal(e); }
    void operator()(Integer_expr const& e)  { self.literal(e); }
    void operator()(Id_expr const& e)       { self.id_expression(e); }
    void operator()(Unary_expr const& e)    { self.unary_expression(e); }
    void operator()(Binary_expr const& e)   { self.binary_expression(e); }
    void operator()(Call_expr const& e)     { self.call_expression(e); }
    void operator()(Unparsed_expr const& e) { self.unparsed_expression(e); }
    void operator()(Value_conv const& e)    { self.conversion(e); }
    void operator()(Copy_init const& e)     { self.initialization(e); }
    void operator()(Bind_init const& e)     { self.initialization(e); }
  };
  apply(e, fn{*this});
}


template<typename T>
inline void
debug_literal(Debug_printer& p, Literal_expr<T> const& e)
{
  Sexpr sentinel(p, e);
  p.space();
  p.value(e.value());
}


void
Debug_printer::literal(Boolean_expr const& e)
{
  debug_literal(*this, e);
}


void
Debug_printer::literal(Integer_expr const& e)
{
  debug_literal(*this, e);
}


void
Debug_printer::id_expression(Id_expr const& e)
{
  Sexpr sentinel(*this, e);
  space();
  id(e.id());
}


void
Debug_printer::unary_expression(Unary_expr const& e)
{
  Sexpr sentinel(*this, e);
  space();
  expression(e.operand());
}


void
Debug_printer::binary_expression(Binary_expr const& e)
{
  Sexpr sentinel(*this, e);
  newline_and_indent();
  newline();
  expression(e.left());
  newline();
  expression(e.right());
  newline_and_undent();
}


void
Debug_printer::call_expression(Call_expr const& e)
{
  Sexpr sentinel(*this, e);
  newline_and_indent();
  expression(e.function());
  newline();
  Expr_list const& args = e.arguments();
  for (auto iter = args.begin(); iter != args.end(); ++iter) {
    expression(*iter);
    if (std::next(iter) != args.end())
      newline();
  }  
  newline_and_undent();
}


void
Debug_printer::unparsed_expression(Unparsed_expr const& e)
{
  Sexpr sentinel(*this, e);
  os << " <|";
  for (auto iter = e.tokens().begin(); iter != e.tokens().end(); ++iter) {
    os << *iter;
    if (std::next(iter) != e.tokens().end())
      os << ' ';
  }
  os << "|>";
}


void
Debug_printer::conversion(Value_conv const& e)
{
  Sexpr guard(*this, e);
  newline_and_indent();
  type(e.type()); // TODO: Destination?
  newline();
  expression(e.source());
  newline_and_undent();
}


void
Debug_printer::initialization(Copy_init const& e)
{
  Sexpr sentinel(*this, e);
  newline_and_indent();
  expression(e.expression());
  newline_and_undent();
}


void
Debug_printer::initialization(Bind_init const& e)
{
  Sexpr sentinel(*this, e);
  newline_and_indent();
  expression(e.expression());
  newline_and_undent();
}


// -------------------------------------------------------------------------- //
// Statements

void
Debug_printer::statement(Stmt const& s)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Stmt const& s)              { lingo_unhandled(s); }
    void operator()(Compound_stmt const& d)     { self.compound_statement(d); }
    void operator()(If_then_stmt const& d)      { self.if_statement(d); }
    void operator()(If_else_stmt const& d)      { self.if_statement(d); }
    void operator()(Return_stmt const& d)       { self.return_statement(d); }
    void operator()(Return_value_stmt const& d) { self.return_statement(d); }
    void operator()(Expression_stmt const& d)   { self.expression_statement(d); }
    void operator()(Declaration_stmt const& d)  { self.declaration_statement(d); }
  };
  apply(s, fn{*this});
}


void
Debug_printer::compound_statement(Compound_stmt const& s)
{
  Sexpr guard(*this, s);
  newline_and_indent();
  Stmt_list const& ss = s.statements();
  for (auto iter = ss.begin(); iter != ss.end(); ++iter) {
    statement(*iter);
    if (std::next(iter) != ss.end())
      newline();    
  }
  newline_and_undent();
}


void
Debug_printer::if_statement(If_then_stmt const& s)
{
  Sexpr guard(*this, s);
  newline_and_indent();
  expression(s.condition());
  newline();
  statement(s.true_branch());
  newline_and_undent();
}


void
Debug_printer::if_statement(If_else_stmt const& s)
{
  Sexpr guard(*this, s);
  newline_and_indent();
  expression(s.condition());
  newline();
  statement(s.true_branch());
  newline();
  statement(s.false_branch());
  newline_and_undent();
}


void
Debug_printer::return_statement(Return_stmt const& s)
{
  Sexpr guard(*this, s);
}


void
Debug_printer::return_statement(Return_value_stmt const& s)
{
  Sexpr guard(*this, s);
  newline_and_indent();
  expression(s.expression());
  newline_and_undent();
}


void
Debug_printer::expression_statement(Expression_stmt const& s)
{
  Sexpr guard(*this, s);
  newline_and_indent();
  expression(s.expression());
  newline_and_undent();
}


void
Debug_printer::declaration_statement(Declaration_stmt const& s)
{
  Sexpr guard(*this, s);
  newline_and_indent();
  declaration(s.declaration());
  newline_and_undent();
}


// -------------------------------------------------------------------------- //
// Declarations

void
Debug_printer::declaration(Decl const& d)
{  
  struct fn
  {
    Debug_printer& self;
    void operator()(Decl const& d)             { lingo_unhandled(d); }
    void operator()(Translation_unit const& d) { self.translation_unit(d); }
    void operator()(Variable_decl const& d)    { self.variable_declaration(d); }
    void operator()(Function_decl const& d)    { self.function_declaration(d); }
  };
  apply(d, fn{*this});
}


void
Debug_printer::translation_unit(Translation_unit const& d)
{
  Sexpr guard(*this, d); 
  d.statements();
}


// -------------------------------------------------------------------------- //
// Variables

void
Debug_printer::variable_declaration(Variable_decl const& d)
{
  Sexpr guard(*this, d); 
  newline_and_indent();
  id(d.name());
  newline();
  type(d.type());
  newline();
  variable_initializer(d.initializer());
  newline_and_undent();
}


void
Debug_printer::variable_initializer(Def const& d)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Def const& d)            { lingo_unhandled(d); }
    void operator()(Empty_def const& d)      { self.variable_initializer(d); }
    void operator()(Expression_def const& d) { self.variable_initializer(d); }
  };
  apply(d, fn{*this});
}

void
Debug_printer::variable_initializer(Empty_def const& d)
{
  Sexpr guard(*this, d);
}


void
Debug_printer::variable_initializer(Expression_def const& d)
{
  Sexpr guard(*this, d);
  newline_and_indent();
  expression(d.expression());
  newline_and_undent();
}


// -------------------------------------------------------------------------- //
// Functions

void
Debug_printer::function_declaration(Function_decl const& d)
{
  Sexpr guard(*this, d); 
  newline_and_indent();
  id(d.name());
  newline();
  type(d.type());  
  newline();
  function_definition(d.definition());
  newline_and_undent();
}


void
Debug_printer::function_definition(Def const& d)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Def const& d)            { lingo_unhandled(d); }
    void operator()(Empty_def const& d)      { self.function_definition(d); }
    void operator()(Expression_def const& d) { self.function_definition(d); }
    void operator()(Function_def const& d)   { self.function_definition(d); }
    void operator()(Intrinsic_def const& d)  { self.function_definition(d); }
  };
  apply(d, fn{*this});
}


void
Debug_printer::function_definition(Empty_def const& d)
{
  Sexpr guard(*this, d); 
}


void
Debug_printer::function_definition(Expression_def const& d)
{
  Sexpr guard(*this, d);
  newline_and_indent();
  expression(d.expression());
  newline_and_undent();
}


void
Debug_printer::function_definition(Function_def const& d)
{
  Sexpr guard(*this, d);
  newline_and_indent();
  statement(d.statement());
  newline_and_undent();
}


// TODO: Print any other useful information about intrinsics? The
// function object/name?
void
Debug_printer::function_definition(Intrinsic_def const& d)
{
  Sexpr guard(*this, d);
}


// -------------------------------------------------------------------------- //
// Constraints

void
Debug_printer::constraint(Cons const& c)
{
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Interface

void
debug(Term const& t)
{
  Debug_printer p(std::cerr);
  p.tree = true;
  p(t);
  p.newline();
}


} // namespace banjo
