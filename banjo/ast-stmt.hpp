// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_STMT_HPP
#define BANJO_AST_STMT_HPP

#include "ast-base.hpp"


namespace banjo
{

// Represents the set of all statemnts in the language.
struct Stmt : Term
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor& v) const = 0;
  virtual void accept(Mutator& v)       = 0;
};


struct Stmt::Visitor
{
#define define_node(Node) virtual void visit(Node const&) = 0;
#include "ast-stmt.def"
#undef define_node
};


struct Stmt::Mutator
{
#define define_node(Node) virtual void visit(Node&) = 0;
#include "ast-stmt.def"
#undef define_node
};


// A blocked sequence of statements.
struct Compound_stmt : Stmt
{
  Compound_stmt()
    : Stmt()
  { }

  Compound_stmt(Stmt_list const& ss)
    : Stmt(), stmts(ss)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Stmt_list const& statements() const { return stmts; }
  Stmt_list&       statements()       { return stmts; }

  Stmt_list stmts;
};


// A statement that evaluates an expression and discards
// the result.
struct Expression_stmt : Stmt
{
  Expression_stmt(Expr& e)
    : expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the expression of the statement.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// A statemnt that declares a variable.
struct Declaration_stmt : Stmt
{
  Declaration_stmt(Decl& d)
    : decl(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the statement.
  Decl const& declaration() const { return *decl; }
  Decl&       declaration()       { return *decl; }

  Decl* decl;
};


// A return statement.
struct Return_stmt : Stmt
{
  Return_stmt(Expr& e)
    : expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the expression returned by the statement.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// Represents an unparsed statement.
struct Unparsed_stmt : Unparsed_term<Stmt>
{
  using Unparsed_term::Unparsed_term;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// ---------------------------------------------------------------------------//
// Visitors

// A generic visitor for statements.
template<typename F, typename T>
struct Generic_stmt_visitor : Stmt::Visitor, Generic_visitor<F, T>
{
  Generic_stmt_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

#define define_node(Node) void visit(Node const& t) { this->invoke(t); }
#include "ast-stmt.def"
#undef define_node
};


// A generic visitor for statements.
template<typename F, typename T>
struct Generic_stmt_mutator : Stmt::Mutator, Generic_mutator<F, T>
{
  Generic_stmt_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

#define define_node(Node) void visit(Node& t) { this->invoke(t); }
#include "ast-stmt.def"
#undef define_node
};


// Apply a function to the given statement.
template<typename F, typename T = typename std::result_of<F(Return_stmt const&)>::type>
inline T
apply(Stmt const& s, F fn)
{
  Generic_stmt_visitor<F, T> vis(fn);
  return accept(s, vis);
}


// Apply a function to the given statement.
template<typename F, typename T = typename std::result_of<F(Return_stmt&)>::type>
inline T
apply(Stmt& s, F fn)
{
  Generic_stmt_mutator<F, T> vis(fn);
  return accept(s, vis);
}


} // namesapce banjo

#endif
