// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_DEF_HPP
#define BANJO_AST_DEF_HPP

#include "ast_base.hpp"


namespace banjo
{

// Denotes the set of definitions for functions and types.
//
// Note that the inclusion of definitions for both functions
// and types is largely one of syntactic convenience. Care
// must be taken to ensure that a class declaration does not
// have e.g., a function definition (that doesn't make sense).
struct Def : Term
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;
};


struct Def::Visitor
{
  virtual void visit(Defaulted_def const&)  { }
  virtual void visit(Deleted_def const&)    { }
  virtual void visit(Expression_def const&) { }
  virtual void visit(Function_def const&)   { }
  virtual void visit(Class_def const&)      { }
  virtual void visit(Union_def const&)      { }
  virtual void visit(Enum_def const&)       { }
  virtual void visit(Concept_def const&)    { }
};


struct Def::Mutator
{
  virtual void visit(Defaulted_def&)  { }
  virtual void visit(Deleted_def&)    { }
  virtual void visit(Expression_def&) { }
  virtual void visit(Function_def&)   { }
  virtual void visit(Class_def&)      { }
  virtual void visit(Union_def&)      { }
  virtual void visit(Enum_def&)       { }
  virtual void visit(Concept_def&)    { }
};


// A defaulted definition has a specification determined by
// the compiler.
//
// C++ allows only defaulted special functions, but this can
// be made far more general.
//
// TODO: When we see this, we're actually going to select a
// specific kind of behavior, depending on the function defaulted.
// We should have derived classes for each kind of defaulted
// behavior (I think).
struct Defaulted_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// A deleted definition is specified to be invalid.
//
// C++ allows deleted functions, but deleted classes (and also
// variables) make sense for partial specializations.
struct Deleted_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// An expression definition defines an entity by an expression.
// Both functions and concepts can have expression definitions.
struct Expression_def : Def
{
  Expression_def(Expr& e)
    : expr(&e)
  { }

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }

  // Returns the expression that defines the entity.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// A function declaration can be initialized by a compound
// statement.
//
// TODO: Provide extended support for member initialization
// lists of member functions.
struct Function_def : Def
{
  Function_def(Stmt& s)
    : stmt(&s)
  { }

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }

  // Returns the statement associated with the function
  // definition.
  Stmt const& statement() const { return *stmt; }
  Stmt&       statement()       { return *stmt; }

  Stmt* stmt;
};


// A definition of a class.
//
// FIXME: Add base classes.
struct Class_def : Def
{
  Class_def(Decl_list const& ds)
    : decls(ds)
  { }

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }

  // Returns the list of member declarations.
  Decl_list const& members() const { return decls; }
  Decl_list&       members()       { return decls; }

  Decl_list decls;
};


// A definition of a union.
struct Union_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// A definition of an enumeration.
struct Enum_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// A concept body is a sequence of statements.
struct Concept_def : Def
{
  Concept_def(Req_list const& rs)
    : reqs(rs)
  { }

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }

  // Returns the sequence of required declarations.
  Req_list const& requirements() const { return reqs; }
  Req_list&       requirements()       { return reqs; }

  Req_list reqs;
};


// A generic visitor for definitions.
template<typename F, typename T>
struct Generic_def_visitor : Def::Visitor, Generic_visitor<F, T>
{
  Generic_def_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Defaulted_def const& d)  { this->invoke(d); }
  void visit(Deleted_def const& d)    { this->invoke(d); }
  void visit(Expression_def const& d) { this->invoke(d); }
  void visit(Function_def const& d)   { this->invoke(d); }
  void visit(Class_def const& d)      { this->invoke(d); }
  void visit(Union_def const& d)      { this->invoke(d); }
  void visit(Enum_def const& d)       { this->invoke(d); }
  void visit(Concept_def const& d)    { this->invoke(d); }
};


template<typename F, typename T = typename std::result_of<F(Defaulted_def const&)>::type>
inline decltype(auto)
apply(Def const& t, F fn)
{
  Generic_def_visitor<F, T> vis(fn);
  return accept(t, vis);
}


// A generic mutator for definitions.
template<typename F, typename T>
struct Generic_def_mutator : Def::Mutator, Generic_visitor<F, T>
{
  Generic_def_mutator(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Defaulted_def& d)  { this->invoke(d); }
  void visit(Deleted_def& d)    { this->invoke(d); }
  void visit(Expression_def& d) { this->invoke(d); }
  void visit(Function_def& d)   { this->invoke(d); }
  void visit(Class_def& d)      { this->invoke(d); }
  void visit(Union_def& d)      { this->invoke(d); }
  void visit(Enum_def& d)       { this->invoke(d); }
  void visit(Concept_def& d)    { this->invoke(d); }
};


template<typename F, typename T = typename std::result_of<F(Defaulted_def&)>::type>
inline decltype(auto)
apply(Def& t, F fn)
{
  Generic_def_mutator<F, T> vis(fn);
  return accept(t, vis);
}


} // namesapce banjo

#endif
