// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_CONS_HPP
#define BANJO_AST_CONS_HPP

#include "ast_base.hpp"


namespace banjo
{

// A name denotes an entity in a progam. Most forms of names are
// ids, which denote use the of those entities in expressions,
// or types.
struct Cons : Term
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;
};


struct Cons::Visitor
{
  virtual void visit(Concept_cons const&)       { }
  virtual void visit(Expression_cons const&)    { }
  virtual void visit(Type_cons const&)          { }
  virtual void visit(Predicate_cons const&)     { }
  virtual void visit(Conversion_cons const&)    { }
  virtual void visit(Deduction_cons const&)     { }
  virtual void visit(Conjunction_cons const&)   { }
  virtual void visit(Disjunction_cons const&)   { }
  virtual void visit(Parameterized_cons const&) { }
};


struct Cons::Mutator
{
  virtual void visit(Concept_cons&)       { }
  virtual void visit(Expression_cons&)    { }
  virtual void visit(Type_cons&)          { }
  virtual void visit(Predicate_cons&)     { }
  virtual void visit(Conversion_cons&)    { }
  virtual void visit(Deduction_cons&)     { }
  virtual void visit(Conjunction_cons&)   { }
  virtual void visit(Disjunction_cons&)   { }
  virtual void visit(Parameterized_cons&) { }
};


// Represents the checking of a nested concepts. These are kept
// in the constraint language in order to facilitate optimization
// in subsumption algorithms.
struct Concept_cons : Cons
{
  Concept_cons(Decl& d, Term_list const& ts)
    : decl(&d), args(ts)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the resolved concept declaration.
  Concept_decl const& declaration() const { return cast<Concept_decl>(*decl); }
  Concept_decl&       declaration()       { return cast<Concept_decl>(*decl); }

  // Returns the template arguments used to check the template.
  Term_list const& arguments() const { return args; }
  Term_list&       arguments()       { return args; }

  Decl*     decl;
  Term_list args;
};


// Represents the evaluation of a constant expression as a constraint.
struct Predicate_cons : Cons
{
  Predicate_cons(Expr& e)
    : expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the expression to be evaluated.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// FIXME: Implement me.
struct Expression_cons : Cons
{
  Expression_cons(Expr& e, Type& t)
    : expr(&e), ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Expr* expr;
  Type* ty;
};


// FIXME: Implement me.
struct Type_cons : Cons
{
  using Cons::Cons;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// FIXME: Implement me.
struct Conversion_cons : Cons
{
  Conversion_cons(Expr& e, Type& t)
    : expr(&e), ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Expr* expr;
  Type* ty;
};


// FIXME: Implement me.
struct Deduction_cons : Cons
{
  using Cons::Cons;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the parameterization of a constraint by constraint
// variables. These are names indicating values of specific types.
// Once resolved by lookup, they are essentially meaningless.
struct Parameterized_cons : Cons
{
  Parameterized_cons(Decl_list const& ps, Cons& c)
    : vars(ps), cons(&c)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Decl_list const& variables() const { return vars; }
  Decl_list&       variables()       { return vars; }

  Cons const& constraint() const { return *cons; }
  Cons&       constraint()       { return *cons; }

  Decl_list vars;
  Cons*     cons;
};


// The base class of binary constraints.
struct Binary_cons : Cons
{
  Binary_cons(Cons& c1, Cons& c2)
    : c1(&c1), c2(&c2)
  { }

  // Returns the left operand.
  Cons const& left() const { return *c1; }
  Cons&       left()       { return *c1; }

  // Returns the right operand.
  Cons const& right() const { return *c2; }
  Cons&       right()       { return *c2; }

  Cons* c1;
  Cons* c2;
};


// Represents the conjunction of constraints.
struct Conjunction_cons : Binary_cons
{
  using Binary_cons::Binary_cons;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the disjunction of constraints.
struct Disjunction_cons : Binary_cons
{
  using Binary_cons::Binary_cons;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A generic visitor for constraints.
template<typename F, typename T>
struct Generic_cons_visitor : Cons::Visitor, Generic_visitor<F, T>
{
  Generic_cons_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Concept_cons const& c)       { this->invoke(c); }
  void visit(Expression_cons const& c)    { this->invoke(c); }
  void visit(Type_cons const& c)          { this->invoke(c); }
  void visit(Predicate_cons const& c)     { this->invoke(c); }
  void visit(Conversion_cons const& c)    { this->invoke(c); }
  void visit(Deduction_cons const& c)     { this->invoke(c); }
  void visit(Conjunction_cons const& c)   { this->invoke(c); }
  void visit(Disjunction_cons const& c)   { this->invoke(c); }
  void visit(Parameterized_cons const& c) { this->invoke(c); }
};


// Apply a function to the given constraint.
template<typename F, typename T = typename std::result_of<F(Concept_cons const&)>::type>
inline decltype(auto)
apply(Cons const& c, F fn)
{
  Generic_cons_visitor<F, T> vis(fn);
  return accept(c, vis);
}


// A generic mutator for names.
template<typename F, typename T>
struct Generic_cons_mutator : Cons::Mutator, Generic_mutator<F, T>
{
  Generic_cons_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Concept_cons& c)       { this->invoke(c); }
  void visit(Expression_cons& c)    { this->invoke(c); }
  void visit(Type_cons& c)          { this->invoke(c); }
  void visit(Predicate_cons& c)     { this->invoke(c); }
  void visit(Conversion_cons& c)    { this->invoke(c); }
  void visit(Deduction_cons& c)     { this->invoke(c); }
  void visit(Conjunction_cons& c)   { this->invoke(c); }
  void visit(Disjunction_cons& c)   { this->invoke(c); }
  void visit(Parameterized_cons& c) { this->invoke(c); }
};


// Apply a function to the given name.
template<typename F, typename T = typename std::result_of<F(Concept_cons&)>::type>
inline decltype(auto)
apply(Cons& c, F fn)
{
  Generic_cons_mutator<F, T> vis(fn);
  return accept(c, vis);
}


} // namesapce banjo

#endif
