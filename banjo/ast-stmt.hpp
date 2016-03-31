// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_STMT_HPP
#define BANJO_AST_STMT_HPP

#include "ast_base.hpp"


namespace banjo
{

// Represents the set of all statemnts in the language.
struct Stmt : Term
{
  struct Visitor;

  virtual void accept(Visitor& v) const = 0;
};


struct Stmt::Visitor
{
  virtual void visit(Compound_stmt const&) { }
  virtual void visit(Expression_stmt const&) { }
  virtual void visit(Declaration_stmt const&) { }
  virtual void visit(Return_stmt const&) { }
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

  // Returns the expression returned by the statement.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// A generic visitor for statement.
template<typename F, typename T>
struct Generic_stmt_visitor : Stmt::Visitor, Generic_visitor<F, T>
{
  Generic_stmt_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Compound_stmt const& s)    { this->invoke(s); }
  void visit(Expression_stmt const& s)  { this->invoke(s); }
  void visit(Declaration_stmt const& s) { this->invoke(s); }
  void visit(Return_stmt const& s)      { this->invoke(s); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Return_stmt const&)>::type>
inline T
apply(Stmt const& s, F fn)
{
  Generic_stmt_visitor<F, T> vis(fn);
  return accept(s, vis);
}


} // namesapce banjo

#endif
