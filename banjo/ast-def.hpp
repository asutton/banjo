// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_DEF_HPP
#define BANJO_AST_DEF_HPP

#include "ast-base.hpp"


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
#define define_node(Node) virtual void visit(Node const&) = 0;
#include "ast-def.def"
#undef define_node
};


struct Def::Mutator
{
#define define_node(Node) virtual void visit(Node&) = 0;
#include "ast-def.def"
#undef define_node
};


// An empty definition. This is used to represent trivially initialiezd
// variables.
//
// TODO: Are there other kinds of declarations that can have empty
// definitions.
//
// ANSWER: User defined types as base_subobjects(Supers) (before code gen)

struct Empty_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
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


// Represents the definition of an entity by an expression. Both
// variables and functions can have expression definitions.
struct Expression_def : Def
{
  Expression_def(Expr& e)
    : expr_(&e)
  { }

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }

  // Returns the expression that defines the entity.
  Expr&       expression()       { return *expr_; }
  Expr const& expression() const { return *expr_; }

  Expr* expr_;
};


// A function declaration can be initialized by a compound statement.
//
// FIXME: Rename this to something more meaningful.
struct Function_def : Def
{
  Function_def(Stmt& s)
    : stmt_(&s)
  { }

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }

  // Returns the statement associated with the function
  // definition.
  Stmt const& statement() const { return *stmt_; }
  Stmt&       statement()       { return *stmt_; }

  Stmt* stmt_;
};


// Represents the compound body of a type as a sequence of statements.
//
// TODO: If we support multiple forms of type definition then what
// would we name the other kind. In fact, this is a broader problem for
// most kinds of entities.
//
// Note that the real distinction is in the syntactic form of the
// definitions. In subsequent passes, these would be reduced to a single
// kind of definition for each entity.
struct Class_def : Def
{
  Class_def(Stmt& s)
    : body_(&s)
  { }

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }

  // Returns the list of member declarations.
  Stmt const& body() const { return *body_; }
  Stmt&       body()       { return *body_; }

  Stmt* body_;
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


// -------------------------------------------------------------------------- //
// Visitors

// A generic visitor for definitions.
template<typename F, typename T>
struct Generic_def_visitor : Def::Visitor, Generic_visitor<F, T>
{
  Generic_def_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

#define define_node(Node) void visit(Node const& t) { this->invoke(t); }
#include "ast-def.def"
#undef define_node
};


// A generic mutator for definitions.
template<typename F, typename T>
struct Generic_def_mutator : Def::Mutator, Generic_mutator<F, T>
{
  Generic_def_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

#define define_node(Node) void visit(Node& t) { this->invoke(t); }
#include "ast-def.def"
#undef define_node
};


template<typename F, typename T = typename std::result_of<F(Empty_def const&)>::type>
inline decltype(auto)
apply(Def const& t, F fn)
{
  Generic_def_visitor<F, T> vis(fn);
  return accept(t, vis);
}


template<typename F, typename T = typename std::result_of<F(Empty_def&)>::type>
inline decltype(auto)
apply(Def& t, F fn)
{
  Generic_def_mutator<F, T> vis(fn);
  return accept(t, vis);
}


} // namesapce banjo

#endif
