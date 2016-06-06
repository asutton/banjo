// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_INSPECTION_HPP
#define BANJO_INSPECTION_HPP

#include "token.hpp"
#include "language.hpp"

#include <iostream>


namespace banjo
{

// The debug printer is used to render AST nodes into s-exprs.
// Output can be formatted as either as an inline string or a
// a fully indented tree.
struct Debug_printer
{
  Debug_printer(std::ostream&);

  void operator()(Name const& n) { id(n); }
  void operator()(Type const& t) { type(t); }
  void operator()(Expr const& e) { expression(e); }
  void operator()(Stmt const& s) { statement(s); }
  void operator()(Decl const& d) { declaration(d); }
  void operator()(Cons const& c) { constraint(c); }

  // Lexical stuff
  void open();
  void close();
  void space();
  void rep(Term const&);
  void prop(char const*);
  void value(bool);
  void value(Integer const&);

  // Unresolved names
  void id(Name const&);
  void simple_id(Simple_id const&);

  // Types
  void type(Type const&);
  void boolean_type(Boolean_type const&);
  void integer_type(Integer_type const&);

  // Expressions
  void expression(Expr const&);
  void literal(Boolean_expr const&);
  void literal(Integer_expr const&);
  void id_expression(Id_expr const&);
  void unary_expression(Unary_expr const&);
  void binary_expression(Binary_expr const&);

  void initializer(Bind_init const&);

  // Statements
  void statement(Stmt const&);

  // Declarations
  void declaration(Decl const&);

  // Constraints
  void constraint(Cons const&);

  std::ostream& os;
  bool  color;
  bool  tree;
  int   indent;
};


template<typename T>
struct Debug_term
{
  T const& t;
};


// Indicate that we should print the debug version of this term.
//
// FIXME: Find a better
template<typename T>
inline Debug_term<T>
debug(T const& t)
{
  return Debug_term<T>{t};
}


template<typename T>
inline std::ostream&
operator<<(std::ostream& os, Debug_term<T> const& d)
{
  Debug_printer p(os);
  p.tree = true;
  p(d.t);
  return os;
}


} // namespace banjo

#endif
