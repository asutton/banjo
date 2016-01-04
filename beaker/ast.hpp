// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_AST_HPP
#define BEAKER_AST_HPP

#include "prelude.hpp"

#include <vector>

namespace beaker
{

struct Term;
struct List;
struct Name;
struct Type;
struct Expr;
struct Stmt;
struct Decl;
struct Init;


// -------------------------------------------------------------------------- //
// Terms

// The base class of all terms in the language.
struct Term
{
  virtual ~Term() { }
};


// -------------------------------------------------------------------------- //
// Lists

// A general purpose list of terms.
struct List : Term, std::vector<Term*>
{
};



// -------------------------------------------------------------------------- //
// Names

struct Name;
struct Simple_id;
struct Global_id;
struct Placeholder_id;
struct Operator_id;
struct Conversion_id;
struct Literal_id;
struct Destructor_id;
struct Template_id;
struct Qualified_id;


// The base class of all identifiers.
struct Name : Term
{
  struct Visitor;

  virtual void accept(Visitor&) const = 0;
};


// Non-modifying visitor.
struct Name::Visitor
{
  virtual void visit(Simple_id const* n)      { }
  virtual void visit(Global_id const* n)      { }
  virtual void visit(Placeholder_id const* n) { }
  virtual void visit(Operator_id const* n)    { }
  virtual void visit(Conversion_id const* n)  { }
  virtual void visit(Literal_id const* n)     { }
  virtual void visit(Destructor_id const* n)  { }
  virtual void visit(Template_id const* n)    { }
  virtual void visit(Qualified_id const* n)   { }
};


// A simple identifier.
struct Simple_id : Name
{
  Simple_id(Symbol* sym)
    : first(sym)
  { }

  void accept(Visitor& v) const { v.visit(this); };

  Symbol const* symbol() const { return first; }

  Symbol* first;
};


// The name of the global namespace. This is essentially an empty
// name, but nonetheless used to represent that the name of that
// single entity.
struct Global_id : Name
{
  Global_id()
  { }

  void accept(Visitor& v) const { v.visit(this); };
};


// An placeholder for a name.
struct Placeholder_id : Name
{
  void accept(Visitor& v) const { v.visit(this); };
};


// An identifier of an overloaded operator.
struct Operator_id : Name
{
  void accept(Visitor& v) const { v.visit(this); };
};


// An identifier of a connversion function.
struct Conversion_id : Name
{
  void accept(Visitor& v) const { v.visit(this); };
};


// An identifier of a user-defined literal.
struct Literal_id : Name
{
  void accept(Visitor& v) const { v.visit(this); };
};


// An identifier for a destructor.
struct Destructor_id : Name
{
  void accept(Visitor& v) const { v.visit(this); };

  Type* first;
};


// An identifier that refers a template specialization.
struct Template_id : Name
{
  void accept(Visitor& v) const { v.visit(this); };

  Decl const* declaration() const { return decl; }
  List const* arguments() const   { return first; }

  Decl* decl;
  List* first;
};


// An explicitly scoped identifier.
struct Qualified_id : Name
{
  Qualified_id(Decl* d, Name* n)
    : decl(d), first(n)
  { }

  void accept(Visitor& v) const { v.visit(this); };

  Decl const* context() const { return decl; }
  Name const* name() const    { return first; }

  Decl* decl;
  Name* first;
};


// A generic visitor for names.
template<typename F, typename T>
struct Generic_name_visitor : Name::Visitor, Generic_visitor<F, T>
{
  Generic_name_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Simple_id const* n)      { this->invoke(n); }
  void visit(Global_id const* n)      { this->invoke(n); }
  void visit(Placeholder_id const* n) { this->invoke(n); }
  void visit(Operator_id const* n)    { this->invoke(n); }
  void visit(Conversion_id const* n)  { this->invoke(n); }
  void visit(Literal_id const* n)     { this->invoke(n); }
  void visit(Destructor_id const* n)  { this->invoke(n); }
  void visit(Template_id const* n)    { this->invoke(n); }
  void visit(Qualified_id const* n)   { this->invoke(n); }
};


// Apply a function to the given name.
template<typename F, typename T = typename std::result_of<F(Simple_id const*)>::type>
inline T
apply(Name const* n, F fn)
{
  Generic_name_visitor<F, T> vis(fn);
  return accept(n, vis);
}


// -------------------------------------------------------------------------- //
// Types

// The base class of all type.
struct Type : Term
{
};


// The integer types.
struct Integer_type : Type
{
  bool sgn;
  int  prec;
};


// The floating point types.
struct Float_type : Type
{
  int prec;
};


// The auto type.
struct Auto_type : Type
{

};


// The type decltype(e).
struct Decltype_type : Type
{

};


// The type decltype(auto).
struct Declauto_type : Type
{

};


struct Qualified_type : Type
{
  Type* first;
};


struct Pointer_type : Type
{
  Type* first;
};


struct Reference_type : Type
{
  Type* first;
};


struct Array_type : Type
{
  Type* first;
  Expr* second;
};


// The type of an unspecified sequence of objects. An array
// of unknown bound.
struct Sequence_type : Type
{
  Type* first;
};


struct Class_type : Type
{
  Decl* first;
};


struct Union_type : Type
{
  Decl* first;
};


struct Enum_type : Type
{
  Decl* first;
};


// -------------------------------------------------------------------------- //
// Expressions

struct Expr : Term
{
};


// -------------------------------------------------------------------------- //
// Statements

struct Stmt : Term
{
};


// -------------------------------------------------------------------------- //
// Variable initializers

// An initializers is any term that follows a declaration.
// Each kind of iniitializer corresponds to a particular
// syntax.
struct Init : Term
{
};


// Default initialization by a constructor or internal
// mechanism.
struct Default_init : Init
{
};


// Deletion.
struct Delete_init : Init
{
};


// Declared but not defined.
struct Incomplete_init : Init
{
};


// Initialization by an expression.
struct Value_init : Init
{
  Expr* first;
};


// Direct invocation of a constructor.
struct Direct_init : Init
{
  List* first;
};


// Initialization by an aggregate.
//
// TODO: This is a list of expression-like things that
// could include designated initializers.
struct Aggregate_init : Init
{
};


// Initialization of a function by a body.
struct Function_init : Init
{
  Stmt* first;
};


// Initializer of a class type.
struct Class_init : Init
{
  List* first;  // bases
  List* second; // members
};


// Initializer of a union type.
struct Union_init : Init
{
};


// Initializer of an enumeration type.
struct Enum_init : Init
{
};


// -------------------------------------------------------------------------- //
// Declarations

struct Decl;
struct Variable_decl;
struct Constant_decl;
struct Function_decl;
struct Parameter_decl;
struct Class_decl;
struct Union_decl;
struct Enum_decl;
struct Namespace_decl;
struct Template_decl;


// A specifier is a flag.
using Specifier = std::int32_t;


// The base class of all declarations. Each declaration
// has a set of specifiers and a reference to the context
// in which it the entity is declared.
struct Decl : Term
{
  struct Visitor;

  Decl(Name* n)
    : spec(0), cxt(nullptr), first(n)
  { }

  Decl(Decl* c, Name* n)
    : spec(0), cxt(c), first(n)
  { }

  virtual void accept(Visitor& v) const = 0;

  Decl const* context() const { return cxt; }
  Name const* name() const    { return first; }

  Specifier spec;
  Decl*     cxt;
  Name*     first;
};


// The declaration visitor.
struct Decl::Visitor
{
  virtual void visit(Variable_decl const*) { }
  virtual void visit(Constant_decl const*) { }
  virtual void visit(Function_decl const*) { }
  virtual void visit(Parameter_decl const*) { }
  virtual void visit(Class_decl const*) { }
  virtual void visit(Union_decl const*) { }
  virtual void visit(Enum_decl const*) { }
  virtual void visit(Namespace_decl const*) { }
  virtual void visit(Template_decl const*) { }
};


// Declares a variable, constant, or function parameter.
struct Object_decl : Decl
{
  Object_decl(Name* n, Type* t, Init* i)
    : Decl(n), second(t), third(i)
  { }

  Type const* type() const { return second; }

  Type* second;
  Init* third;
};


// Declares a class, union, enum, or generic type.
struct Type_decl : Decl
{
  Type_decl(Name* n, Init* i)
    : Decl(n), second(i)
  { }

  Init const* definition() const { return second; }

  Name* first;
  Init* second;
};


// Declares a variable.
struct Variable_decl : Object_decl
{
  Variable_decl(Name* n, Type* t, Init* i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(this); }

  Init const* initializer() const { return third; }
};


// Declares a symbolic constant.
struct Constant_decl : Object_decl
{
  Constant_decl(Name* n, Type* t, Init* i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(this); }

  Init const* initializer() const { return third; }
};


// Declares a function.
//
// TODO: Write type/return type accessors.
struct Function_decl : Decl
{
  void accept(Visitor& v) const { v.visit(this); }

  Name const* name() const       { return first; }
  List const* parameters() const { return third; }
  Init const* definition() const { return fourth; }

  Name* first;
  Type* second;
  List* third;
  Init* fourth;
};


// Declares a function paramter.
struct Parameter_decl : Object_decl
{
  Parameter_decl(Name* n, Type* t, Init* i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(this); }

  Init const* default_argument() const { return third; }
};


struct Class_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(this); }
};


struct Union_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(this); }
};


struct Enum_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(this); }
};


// Declares a template.
struct Template_decl : Decl
{
  void accept(Visitor& v) const { v.visit(this); }

  List* first;
  Decl* second;
};


// Defines a namespace.
//
// TODO: How should I model re-opened namespaces? Probably
// just do the lookup and re-establish the context.
struct Namespace_decl : Decl
{
  Namespace_decl(Name* n)
    : Decl(n), second(new List())
  { }

  Namespace_decl(Decl* cxt, Name* n)
    : Decl(cxt, n), second(new List())
  { }

  void accept(Visitor& v) const { v.visit(this); }

  bool is_global() const    { return cxt == nullptr; }
  bool is_anonymous() const { return is<Placeholder_id>(first); }

  List* second;
};


// A generic visitor for names.
template<typename F, typename T>
struct Generic_decl_visitor : Decl::Visitor, Generic_visitor<F, T>
{
  Generic_decl_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Variable_decl const* d)  { this->invoke(d); }
  void visit(Constant_decl const* d)  { this->invoke(d); }
  void visit(Function_decl const* d)  { this->invoke(d); }
  void visit(Parameter_decl const* d) { this->invoke(d); }
  void visit(Class_decl const* d)     { this->invoke(d); }
  void visit(Union_decl const* d)     { this->invoke(d); }
  void visit(Enum_decl const* d)      { this->invoke(d); }
  void visit(Namespace_decl const* d) { this->invoke(d); }
  void visit(Template_decl const* d)  { this->invoke(d); }
};


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl const*)>::type>
inline T
apply(Decl const* d, F fn)
{
  Generic_decl_visitor<F, T> vis(fn);
  return accept(d, vis);
}


// -------------------------------------------------------------------------- //
// Miscellaneous


struct Translation_unit : Term
{
  List* first;
};

} // namespace beaker


#endif
