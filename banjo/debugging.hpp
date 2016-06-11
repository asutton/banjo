// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_INSPECTION_HPP
#define BANJO_INSPECTION_HPP

#include "token.hpp"
#include "language.hpp"


namespace banjo
{

// The debug printer is used to render AST nodes into s-exprs.
// Output can be formatted as either as an inline string or a
// a fully indented tree.
struct Debug_printer
{
  Debug_printer(std::ostream&);

  void operator()(Term const& n);
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
  void newline();
  void newline_and_indent();
  void newline_and_undent();
  void undent_and_newline();
  void rep(Term const&);
  void prop(char const*);
  void value(bool);
  void value(Integer const&);

  // Unresolved names
  void id(Name const&);
  void simple_id(Simple_id const&);

  // Types
  void type(Type const&);
  void void_type(Void_type const&);
  void boolean_type(Boolean_type const&);
  void integer_type(Integer_type const&);
  void function_type(Function_type const&);
  void type_category(Type const&);
  void type_qualifiers(Type const&);
  void type_qualifier(char const*);

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


void debug(Term const&);


} // namespace banjo

#endif
