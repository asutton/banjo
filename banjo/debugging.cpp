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
  std::string s = type_str(t);
  auto n = s.find_last_of(':');
  if (color)
    os << lingo::bright_green(s.substr(n + 1));
  else
    os << s.substr(n + 1);
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
// Names

void
Debug_printer::id(Name const& n)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Name const& n)      { lingo_unhandled(n); }
    void operator()(Simple_id const& n) { self.simple_id(n); }
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
  for (Type const& p : t.parameter_types()) {
    type(p);
    newline();
  }
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


void
Debug_printer::expression(Expr const& e)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Expr const& e)         { lingo_unhandled(e); }
    void operator()(Boolean_expr const& e) { self.literal(e); }
    void operator()(Integer_expr const& e) { self.literal(e); }
    void operator()(Id_expr const& e)      { self.id_expression(e); }
    void operator()(Binary_expr const& e)  { self.binary_expression(e); }
    void operator()(Unary_expr const& e)   { self.unary_expression(e); }
    void operator()(Bind_init const& e)    { self.initializer(e); }
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
  space();
  expression(e.left());
  space();
  expression(e.right());
}


void
Debug_printer::initializer(Bind_init const& e)
{
  Sexpr sentinel(*this, e);
  space();
  expression(e.expression());
}


// -------------------------------------------------------------------------- //
// Statements

void
Debug_printer::statement(Stmt const& s)
{
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Declarations

void
Debug_printer::declaration(Decl const& d)
{
  Sexpr sentinel(*this, d);
  space();
  id(d.name());
  space();
  prop("id");
  os << &d;
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
debug(Name const& n)
{
  Debug_printer p(std::cerr);
  p.tree = true;
  p(n);
  p.newline();
}


void
debug(Type const& t)
{
  Debug_printer p(std::cerr);
  p.tree = true;
  p(t);
  p.newline();
}


} // namespace banjo
