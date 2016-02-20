// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_NAME_HPP
#define BANJO_AST_NAME_HPP

#include "ast_base.hpp"
#include "operator.hpp"


namespace banjo
{

// A name denotes an entity in a progam. Most forms of names are
// ids, which denote use the of those entities in expressions, or
// types.
struct Name : Term
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  // Returns an unqualified representation of the name.
  virtual Name const& unqualified_name() const { return *this; }
  virtual Name&       unqualified_name()       { return *this; }
};


struct Name::Visitor
{
  virtual void visit(Simple_id const& n)      { }
  virtual void visit(Global_id const& n)      { }
  virtual void visit(Placeholder_id const& n) { }
  virtual void visit(Operator_id const& n)    { }
  virtual void visit(Conversion_id const& n)  { }
  virtual void visit(Literal_id const& n)     { }
  virtual void visit(Destructor_id const& n)  { }
  virtual void visit(Template_id const& n)    { }
  virtual void visit(Concept_id const& n)     { }
  virtual void visit(Qualified_id const& n)   { }
};


struct Name::Mutator
{
  virtual void visit(Simple_id& n)      { }
  virtual void visit(Global_id& n)      { }
  virtual void visit(Placeholder_id& n) { }
  virtual void visit(Operator_id& n)    { }
  virtual void visit(Conversion_id& n)  { }
  virtual void visit(Literal_id& n)     { }
  virtual void visit(Destructor_id& n)  { }
  virtual void visit(Template_id& n)    { }
  virtual void visit(Concept_id& n)     { }
  virtual void visit(Qualified_id& n)   { }
};


// A simple identifier.
struct Simple_id : Name
{
  Simple_id(Symbol const& sym)
    : first(&sym)
  { }

  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };

  Symbol const& symbol() const { return *first; }

  Symbol const* first;
};


// The name of the global namespace. This is essentially an empty
// name, but nonetheless used to represent that the name of that
// single entity.
struct Global_id : Name
{
  Global_id()
  { }

  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };
};


// An placeholder for a name.
//
// FIXME: This is not a good name for this class.
struct Placeholder_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };
};


// An identifier of an overloaded operator.
//
// TODO: Implement me.
struct Operator_id : Name
{
  Operator_id(Operator_kind k)
    : op(k)
  { }

  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };

  // Returns the operator kind.
  Operator_kind kind() const { return op; }

  Operator_kind op;
};


// An identifier of a connversion function.
//
// TODO: Implement me.
struct Conversion_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };
};


// An identifier of a user-defined literal.
//
// TODO: Implement me.
struct Literal_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };
};


// An identifier for a destructor.
struct Destructor_id : Name
{
  void accept(Mutator& v)       { v.visit(*this); };
  void accept(Visitor& v) const { v.visit(*this); };

  // Returns the type named by the destructor id.
  Type const& type() const { return *first; }
  Type&       type()       { return *first; }

  Type* first;
};


struct Template_decl;
struct Concept_decl;


// An identifier that refers a template specialization.
//
// FIXME: What happens if the declaration refers to an
// overload set?
struct Template_id : Name
{
  Template_id(Decl& d, Term_list const& a)
    : decl(&d), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };

  Template_decl const& declaration() const;
  Template_decl&       declaration();

  Term_list const& arguments() const { return args; }
  Term_list&       arguments()       { return args; }

  Decl*      decl;
  Term_list  args;
};


// An identifier that refers a concept check. The arguments
// are not matched against the declaration.
//
// FIXME: What happens if the declaration refers to an
// overload set?
struct Concept_id : Name
{
  Concept_id(Decl& d, Term_list const& a)
    : decl(&d), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };

  Concept_decl const& declaration() const;
  Concept_decl&       declaration();

  Term_list const& arguments() const { return args; }
  Term_list&       arguments()       { return args; }

  Decl*      decl;
  Term_list  args;
};


// An unqualified identifier (i.e., one of the other names)
// that is qualified by some enclosing scope. For example,
// in the name:
//
//    N1::N2::x
//
// `x` is a simple-id, and the enclosing scope is `N2`.
// Note that the complete qualification. Note that the
// enclosing namespaces can be recovered by "walking"
// up the declaration's context.
struct Qualified_id : Name
{
  Qualified_id(Decl& d, Name& n)
    : decl(&d), id(&n)
  { }

  void accept(Visitor& v) const { v.visit(*this); };
  void accept(Mutator& v)       { v.visit(*this); };

  // Returns the qualifying scope (the enclosing declaration)
  // for the unqualified id.
  Decl const& scope() const { return *decl; }
  Decl&       scope()       { return *decl; }

  // Returns the unqualified part of the id.
  Name const& name() const { return *id; }
  Name&       name()       { return *id; }

  // Returns the unqualified part of the id.
  Name const& unqualified_name() const { return *id; }
  Name&       unqualified_name()       { return *id; }

  Decl* decl;
  Name* id;
};


// A generic visitor for names.
template<typename F, typename T>
struct Generic_name_visitor : Name::Visitor, Generic_visitor<F, T>
{
  Generic_name_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Simple_id const& n)      { this->invoke(n); }
  void visit(Global_id const& n)      { this->invoke(n); }
  void visit(Placeholder_id const& n) { this->invoke(n); }
  void visit(Operator_id const& n)    { this->invoke(n); }
  void visit(Conversion_id const& n)  { this->invoke(n); }
  void visit(Literal_id const& n)     { this->invoke(n); }
  void visit(Destructor_id const& n)  { this->invoke(n); }
  void visit(Template_id const& n)    { this->invoke(n); }
  void visit(Qualified_id const& n)   { this->invoke(n); }
};


// Apply a function to the given name.
template<typename F, typename T = typename std::result_of<F(Simple_id const&)>::type>
inline decltype(auto)
apply(Name const& n, F fn)
{
  Generic_name_visitor<F, T> vis(fn);
  return accept(n, vis);
}


// A generic mutator for names.
template<typename F, typename T>
struct Generic_name_mutator : Name::Mutator, Generic_mutator<F, T>
{
  Generic_name_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Simple_id& n)      { this->invoke(n); }
  void visit(Global_id& n)      { this->invoke(n); }
  void visit(Placeholder_id& n) { this->invoke(n); }
  void visit(Operator_id& n)    { this->invoke(n); }
  void visit(Conversion_id& n)  { this->invoke(n); }
  void visit(Literal_id& n)     { this->invoke(n); }
  void visit(Destructor_id& n)  { this->invoke(n); }
  void visit(Template_id& n)    { this->invoke(n); }
  void visit(Qualified_id& n)   { this->invoke(n); }
};


// Apply a function to the given name.
template<typename F, typename T = typename std::result_of<F(Simple_id&)>::type>
inline decltype(auto)
apply(Name& n, F fn)
{
  Generic_name_mutator<F, T> vis(fn);
  return accept(n, vis);
}


} // namesapce banjo

#endif
