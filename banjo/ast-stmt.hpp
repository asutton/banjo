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


// An empty statement.
struct Empty_stmt : Stmt, Allocatable<Empty_stmt>
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A helper node that represents all kinds of statement sequences.
struct Compound_stmt : Stmt, Allocatable<Compound_stmt>
{
  Compound_stmt() = default;

  Compound_stmt(Stmt_list&& ss)
    : Stmt(), stmts_(std::move(ss))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Stmt_list const& statements() const { return stmts_; }
  Stmt_list&       statements()       { return stmts_; }

  Stmt_list stmts_;
};


// A statement that evaluates an expression and discards
// the result.
struct Expression_stmt : Stmt, Allocatable<Expression_stmt>
{
  Expression_stmt(Expr& e)
    : expr_(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the expression of the statement.
  Expr const& expression() const { return *expr_; }
  Expr&       expression()       { return *expr_; }

  Expr* expr_;
};


// A statement that declares a variable.
struct Declaration_stmt : Stmt, Allocatable<Declaration_stmt>
{
  Declaration_stmt(Decl& d)
    : decl_(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the statement.
  Decl const& declaration() const { return *decl_; }
  Decl&       declaration()       { return *decl_; }

  Decl* decl_;
};


// A return statement.
struct Return_stmt : Stmt, Allocatable<Return_stmt>
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A return-value statement.
struct Return_value_stmt : Stmt, Allocatable<Return_value_stmt>
{
  Return_value_stmt(Expr& e)
    : expr_(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the expression returned by the statement.
  Expr const& expression() const { return *expr_; }
  Expr&       expression()       { return *expr_; }

  Expr* expr_;
};


// A yield statement.
struct Yield_stmt : Stmt, Allocatable<Yield_stmt>
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A yield-value statement.
struct Yield_value_stmt : Stmt, Allocatable<Yield_value_stmt>
{
  Yield_value_stmt(Expr& e)
    : expr_(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator &v)       { v.visit(*this); }

  Expr const& expression() const { return *expr_; }
  Expr&       expression()       { return *expr_; }

  Expr* expr_;
};


// An if-then statement.
struct If_then_stmt : Stmt, Allocatable<If_then_stmt>
{
  If_then_stmt(Expr& e, Stmt& s)
    : cond_(&e), then_(&s)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& condition() const { return *cond_; }
  Expr&       condition()       { return *cond_; }

  Stmt const& true_branch() const { return *then_; }
  Stmt&       true_branch()       { return *then_; }

  Expr* cond_;
  Stmt* then_;
};


// An if-then-else statement.
struct If_else_stmt : Stmt, Allocatable<If_else_stmt>
{
  If_else_stmt(Expr& e, Stmt& s1, Stmt& s2)
    : cond_(&e), true_(&s1), false_(&s2)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& condition() const { return *cond_; }
  Expr&       condition()       { return *cond_; }

  Stmt const& true_branch() const { return *true_; }
  Stmt&       true_branch()       { return *true_; }

  Stmt const& false_branch() const { return *false_; }
  Stmt&       false_branch()       { return *false_; }

  Expr* cond_;
  Stmt* true_;
  Stmt* false_;
};


// A while statement.
struct While_stmt : Stmt, Allocatable<While_stmt>
{
  While_stmt(Expr& e, Stmt& s)
    : cond_(&e), body_(&s)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& condition() const { return *cond_; }
  Expr&       condition()       { return *cond_; }

  Stmt const& body() const { return *body_; }
  Stmt&       body()       { return *body_; }

  Expr* cond_;
  Stmt* body_;
};


// A break statement.
struct Break_stmt : Stmt, Allocatable<Break_stmt>
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A continue statement.
struct Continue_stmt : Stmt, Allocatable<Continue_stmt>
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents an unparsed statement.
struct Unparsed_stmt : Unparsed_term<Stmt>, Allocatable<Unparsed_stmt>
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
template<typename F, typename T = typename std::result_of<F(Empty_stmt const&)>::type>
inline T
apply(Stmt const& s, F fn)
{
  Generic_stmt_visitor<F, T> vis(fn);
  return accept(s, vis);
}


// Apply a function to the given statement.
template<typename F, typename T = typename std::result_of<F(Empty_stmt&)>::type>
inline T
apply(Stmt& s, F fn)
{
  Generic_stmt_mutator<F, T> vis(fn);
  return accept(s, vis);
}


} // namesapce banjo

#endif
