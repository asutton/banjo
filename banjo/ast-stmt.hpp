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
struct Empty_stmt : Stmt
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A helper node that represents all kinds of statement sequences.
struct Multiple_stmt : Stmt
{
  Multiple_stmt()
    : Stmt()
  { }

  Multiple_stmt(Stmt_list&& ss)
    : Stmt(), stmts(std::move(ss))
  { }

  Stmt_list const& statements() const { return stmts; }
  Stmt_list&       statements()       { return stmts; }

  Stmt_list stmts;
};


// A sequence of statements representing a complete translation unit.
struct Translation_stmt : Multiple_stmt
{
  using Multiple_stmt::Multiple_stmt;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A sequence of statements representing the body of a type.
//
// Note that this term only follows the declaration of a type as a form
// of its definition. It is otherwise syntactically identical to compound
// statement, but with different semantics.
struct Member_stmt : Multiple_stmt
{
  using Multiple_stmt::Multiple_stmt;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A sequence of statements to be executed within a function.
struct Compound_stmt : Multiple_stmt
{
  using Multiple_stmt::Multiple_stmt;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
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


// A statement that declares a variable.
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

// A yield statement.
struct Yield_stmt : Stmt
{
  Yield_stmt(Expr& e)
    : expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator &v)       { v.visit(*this); }

  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};

// An if-then statement.
struct If_then_stmt : Stmt
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
struct If_else_stmt : Stmt
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
struct While_stmt : Stmt
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
struct Break_stmt : Stmt
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A continue statement.
struct Continue_stmt : Stmt
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
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
