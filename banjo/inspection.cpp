// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "inspection.hpp"
#include "print.hpp"

#include <iterator>
#include <iostream>


namespace banjo
{

// An RAII class that wraps each term in parens and emits its
// implementing class name.
struct Sexpr
{
  Sexpr(Debug_printer& p, Term const& t)
    : printer(p)
  {
    printer.open();
    printer.rep(t);
  }

  // When debugging expressions, include their type as
  // part of the representation.
  Sexpr(Debug_printer& p, Expr const& e)
    : printer(p)
  {
    printer.open();
    printer.rep(e);
    printer.space();
    printer.prop("type");
    printer.os << e.type();
  }

  ~Sexpr()
  {
    printer.close();
  }

  Debug_printer& printer;
};


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
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Types


void
Debug_printer::type(Type const& t)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Expr const& e)               { banjo_unhandled_case(e); }
    void operator()(Boolean_type const& e)       { self.boolean_type(e); }
    void operator()(Integer_type const& e)       { self.integer_type(e); }
  };
}


// -------------------------------------------------------------------------- //
// Expressions


void
Debug_printer::expression(Expr const& e)
{
  struct fn
  {
    Debug_printer& self;
    void operator()(Expr const& e)               { banjo_unhandled_case(e); }
    void operator()(Boolean_expr const& e)       { self.literal(e); }
    void operator()(Integer_expr const& e)       { self.literal(e); }
    void operator()(Binary_expr const& e)        { self.binary_expression(e); }
    void operator()(Unary_expr const& e)         { self.unary_expression(e); }
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


// -------------------------------------------------------------------------- //
// Statements

void
Debug_printer::statement(Stmt const& s)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Declarations

void
Debug_printer::declaration(Decl const& d)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Constraints

void
Debug_printer::constraint(Cons const& c)
{
  lingo_unimplemented();
}


} // namespace banjo
