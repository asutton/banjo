// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_REQ_HPP
#define BANJO_AST_REQ_HPP

#include "ast_base.hpp"


namespace banjo
{

// Represents the syntactic form of requirements in concept
// definitions, requires-expressions, and axioms.
struct Req : Term
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;
};


struct Req::Visitor
{
  virtual void visit(Type_req const&)        { }
  virtual void visit(Syntactic_req const&)   { }
  virtual void visit(Semantic_req const&)    { }
  virtual void visit(Expression_req const&)  { }
  virtual void visit(Simple_req const&)      { }
  virtual void visit(Conversion_req const&)  { }
  virtual void visit(Deduction_req const&)   { }
};


struct Req::Mutator
{
  virtual void visit(Type_req&)        { }
  virtual void visit(Syntactic_req&)   { }
  virtual void visit(Semantic_req&)    { }
  virtual void visit(Expression_req&)  { }
  virtual void visit(Simple_req&)      { }
  virtual void visit(Conversion_req&)  { }
  virtual void visit(Deduction_req&)   { }
};


// Represents the requirement for an associated type.
struct Type_req : Req
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the requirement for a sequence of usage requirements.
// This wraps a requires-expression.
struct Syntactic_req : Req
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& expression() const { return *req; }
  Expr&       expression()       { return *req; }

  Expr* req;
};


// Represents the requiremnt for a sequence of semantic requirements.
// This wraps an axiom-declaration.
struct Semantic_req : Req
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Decl* sema;
};


// Represents the requirement for an expression to be satsified.
struct Expression_req : Req
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// A requirement for a valid expression. The result of the
// expression is represented by a unique invented type.
struct Simple_req : Req
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// Represents the requirement for a valid expression and its
// conversion to a given type.
//
//    e -> t
//
// Here, e must have type t. Note that t cannot contain existentials,
// but may include template parameters. These must be instantiated
// prior to determining conversion.
struct Conversion_req : Req
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Expr* expr;
  Type* ty;
};


// Represents the rquiremnet for a valid expression and the deduction
// of its return type as a given (or invented) type.
//
//    e : t
//
// Here, e must be valid and its return type deduced as t. Note that
// t may be non-dependent, in which case the requirement is that
// the types match.
struct Deduction_req : Req
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Expr* expr;
  Type* ty;
};


// A generic visitor for expressions.
template<typename F, typename T>
struct Generic_req_visitor : Req::Visitor, Generic_visitor<F, T>
{
  Generic_req_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Type_req const& r)        { this->invoke(r); }
  void visit(Syntactic_req const& r)   { this->invoke(r); }
  void visit(Semantic_req const& r)    { this->invoke(r); }
  void visit(Expression_req const& r)  { this->invoke(r); }
  void visit(Simple_req const& r)      { this->invoke(r); }
  void visit(Conversion_req const& r)  { this->invoke(r); }
  void visit(Deduction_req const& r)   { this->invoke(r); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Type_req const&)>::type>
inline T
apply(Req const& r, F fn)
{
  Generic_req_visitor<F, T> vis(fn);
  return accept(r, vis);
}


// A generic mutator for expressions.
template<typename F, typename T>
struct Generic_req_mutator : Req::Mutator, Generic_mutator<F, T>
{
  Generic_req_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Type_req& r)        { this->invoke(r); }
  void visit(Syntactic_req& r)   { this->invoke(r); }
  void visit(Semantic_req& r)    { this->invoke(r); }
  void visit(Expression_req& r)  { this->invoke(r); }
  void visit(Simple_req& r)      { this->invoke(r); }
  void visit(Conversion_req& r)  { this->invoke(r); }
  void visit(Deduction_req& r)   { this->invoke(r); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Type_req&)>::type>
inline T
apply(Req& r, F fn)
{
  Generic_req_mutator<F, T> vis(fn);
  return accept(r, vis);
}


} // namesapce banjo

#endif
