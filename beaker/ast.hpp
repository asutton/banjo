// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_AST_HPP
#define BEAKER_AST_HPP

#include "prelude.hpp"

#include <vector>

namespace beaker
{

struct Term;
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


// FIXME: Make this a random access iterator.
template<typename T>
struct List_iterator
{
  using Iter              = typename std::vector<T*>::iterator;
  using value_type        = T;
  using reference         = T&;
  using pointer           = T*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  List_iterator(Iter i)
    : iter(i)
  { }

  reference operator*() const { return **iter; }
  pointer  operator->() const { return *iter; }

  List_iterator& operator++()    { ++iter; return *this; }
  List_iterator& operator++(int) { List_iterator x = *this; ++iter; return x; }

  bool operator==(List_iterator i) const { return iter == i.iter; }
  bool operator!=(List_iterator i) const { return iter != i.iter; }

  Iter iter;
};


template<typename T>
struct List_iterator<T const>
{
  using Iter              = typename std::vector<T*>::const_iterator;
  using value_type        = T;
  using reference         = T const&;
  using pointer           = T const*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  List_iterator(Iter i)
    : iter(i)
  { }

  reference operator*() const { return **iter; }
  pointer  operator->() const { return *iter; }

  List_iterator& operator++()    { ++iter; return *this; }
  List_iterator& operator++(int) { List_iterator x = *this; ++iter; return x; }

  bool operator==(List_iterator i) const { return iter == i.iter; }
  bool operator!=(List_iterator i) const { return iter != i.iter; }

  Iter iter;
};


template<typename T>
struct List : Term, std::vector<T*>
{
  using iterator = List_iterator<T>;
  using const_iterator = List_iterator<T const>;

  using std::vector<T*>::vector;

  std::vector<T*> const& base() const { return *this; }
  std::vector<T*>&       base()       { return *this; }

  iterator begin() { return base().begin(); }
  iterator end()   { return base().end(); }

  const_iterator begin() const { return base().begin(); }
  const_iterator end() const   { return base().end(); }
  };


using Term_list = List<Term>;
using Type_list = List<Type>;
using Expr_list = List<Expr>;
using Stmt_list = List<Stmt>;
using Decl_list = List<Decl>;


// -------------------------------------------------------------------------- //
// Names
//
// TODO: Add support for De Bruijn names (parameter depth and offset).

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
  virtual void visit(Simple_id const& n)      { }
  virtual void visit(Global_id const& n)      { }
  virtual void visit(Placeholder_id const& n) { }
  virtual void visit(Operator_id const& n)    { }
  virtual void visit(Conversion_id const& n)  { }
  virtual void visit(Literal_id const& n)     { }
  virtual void visit(Destructor_id const& n)  { }
  virtual void visit(Template_id const& n)    { }
  virtual void visit(Qualified_id const& n)   { }
};


// A simple identifier.
struct Simple_id : Name
{
  Simple_id(Symbol const* sym)
    : first(sym)
  { }

  void accept(Visitor& v) const { v.visit(*this); };

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
};


// An placeholder for a name.
struct Placeholder_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };
};


// An identifier of an overloaded operator.
//
// TODO: Implement me.
struct Operator_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };
};


// An identifier of a connversion function.
//
// TODO: Implement me.
struct Conversion_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };
};


// An identifier of a user-defined literal.
//
// TODO: Implement me.
struct Literal_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };
};


// An identifier for a destructor.
//
// TODO: Implement me.
struct Destructor_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };

  Type const& type() { return *first; }

  Type* first;
};


// An identifier that refers a template specialization.
struct Template_id : Name
{
  void accept(Visitor& v) const { v.visit(*this); };

  Decl const&      declaration() const { return *decl; }
  Term_list const& arguments() const   { return first; }

  Decl*      decl;
  Term_list  first;
};


// An explicitly scoped identifier.
struct Qualified_id : Name
{
  Qualified_id(Decl* d, Name* n)
    : decl(d), first(n)
  { }

  void accept(Visitor& v) const { v.visit(*this); };

  Decl const& context() const { return *decl; }
  Name const& name() const    { return *first; }

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
inline T
apply(Name const& n, F fn)
{
  Generic_name_visitor<F, T> vis(fn);
  return accept(n, vis);
}


// -------------------------------------------------------------------------- //
// Types
//
// TODO: Add support for a univalent type? Unit?
//
// TODO: Add character types.

struct Type;
struct Void_type;
struct Boolean_type;
struct Integer_type;
struct Float_type;
struct Auto_type;
struct Decltype_type;
struct Declauto_type;
struct Function_type;
struct Qualified_type;
struct Pointer_type;
struct Reference_type;
struct Array_type;
struct Sequence_type;
struct Class_type;
struct Union_type;
struct Enum_type;
struct Typename_type;


// The base class of all types.
struct Type : Term
{
  struct Visitor;

  virtual void accept(Visitor&) const = 0;
};


struct Type::Visitor
{
  virtual void visit(Void_type const&)      { }
  virtual void visit(Boolean_type const&)   { }
  virtual void visit(Integer_type const&)   { }
  virtual void visit(Float_type const&)     { }
  virtual void visit(Auto_type const&)      { }
  virtual void visit(Decltype_type const&)  { }
  virtual void visit(Declauto_type const&)  { }
  virtual void visit(Function_type const&)  { }
  virtual void visit(Qualified_type const&) { }
  virtual void visit(Pointer_type const&)   { }
  virtual void visit(Reference_type const&) { }
  virtual void visit(Array_type const&)     { }
  virtual void visit(Sequence_type const&)  { }
  virtual void visit(Class_type const&)     { }
  virtual void visit(Union_type const&)     { }
  virtual void visit(Enum_type const&)      { }
  virtual void visit(Typename_type const&)  { }
};


// The void type.
struct Void_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
};


// The boolean type.
struct Boolean_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
};


// The integer types.
//
// TODO: Add a flag that distinguishes between native spellings
// of types and their precise representation? For example, "int"
// is lexically different than "int32", and those differences
// might be meaningful in the output. Do the same for float.
struct Integer_type : Type
{
  Integer_type(bool s = true, int p = 32)
    : sign(s), prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  bool is_signed() const   { return sign; }
  bool is_unsigned() const { return !is_signed(); }
  int  precision() const   { return prec; }

  bool sign;
  int  prec;
};


// The IEEE 754 floating point types.
struct Float_type : Type
{
  Float_type(int p = 64)
    : prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  int precision() const { return prec; }

  int prec;
};



// The auto type.
//
// TODO: Allow a deduction constraint on placeholder types.
// This would be checked after deduction. Extend this for
// decltype(auto) types as well.
struct Auto_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
};


// The type decltype(e).
struct Decltype_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
};


// The type decltype(auto).
struct Declauto_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
};


// A function type.
struct Function_type : Type
{
  Function_type(Type_list const& p, Type* r)
    : first(p), second(r)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Type_list const& parameter_types() const { return first; }
  Type const&      return_type() const     { return *second; }

  Type_list first;
  Type*     second;
};


struct Qualified_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }

  Type* first;
};


struct Pointer_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }

  Type* first;
};


struct Reference_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }

  Type* first;
};


struct Array_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }

  Type* first;
  Expr* second;
};


// The type of an unspecified sequence of objects. An array
// of unknown bound.
struct Sequence_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }

  Type* first;
};


// The base class of all user-defined types.
struct User_defined_type : Type
{
  User_defined_type(Decl* d)
    : decl(d)
  { }

  Decl const& declaration() const { return *decl; }

  Decl* decl;
};


struct Class_decl;
struct Union_decl;
struct Enum_decl;
struct Type_parm;


// TODO: Factor a base class for all of these: user-defined type.
struct Class_type : User_defined_type
{
  Class_decl const& declaration() const;

  void accept(Visitor& v) const { v.visit(*this); }
};


struct Union_type : User_defined_type
{
  void accept(Visitor& v) const { v.visit(*this); }

  Union_decl const& declaration() const;
};


struct Enum_type : User_defined_type
{
  void accept(Visitor& v) const { v.visit(*this); }

  Enum_decl const& declaration() const;
};


// The type of a type parameter declaration.
//
// FIXME: Guarantee that d is a Type_parm.
struct Typename_type : User_defined_type
{
  Typename_type(Decl* d)
    : User_defined_type(d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Type_parm const& declaration() const;
};


// A generic visitor for types.
template<typename F, typename T>
struct Generic_type_visitor : Type::Visitor, Generic_visitor<F, T>
{
  Generic_type_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Void_type const& t)      { this->invoke(t); }
  void visit(Boolean_type const& t)   { this->invoke(t); }
  void visit(Integer_type const& t)   { this->invoke(t); }
  void visit(Float_type const& t)     { this->invoke(t); }
  void visit(Auto_type const& t)      { this->invoke(t); }
  void visit(Decltype_type const& t)  { this->invoke(t); }
  void visit(Declauto_type const& t)  { this->invoke(t); }
  void visit(Qualified_type const& t) { this->invoke(t); }
  void visit(Pointer_type const& t)   { this->invoke(t); }
  void visit(Reference_type const& t) { this->invoke(t); }
  void visit(Array_type const& t)     { this->invoke(t); }
  void visit(Sequence_type const& t)  { this->invoke(t); }
  void visit(Class_type const& t)     { this->invoke(t); }
  void visit(Union_type const& t)     { this->invoke(t); }
  void visit(Enum_type const& t)      { this->invoke(t); }
  void visit(Typename_type const& t)  { this->invoke(t); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_type const&)>::type>
inline T
apply(Type const& t, F fn)
{
  Generic_type_visitor<F, T> vis(fn);
  return accept(t, vis);
}


// -------------------------------------------------------------------------- //
// Expressions
//
// TODO: Add bitwise operations.

struct Boolean_expr;
struct Integer_expr;
struct Real_expr;
struct Reference_expr;
struct Add_expr;
struct Sub_expr;
struct Mul_expr;
struct Div_expr;
struct Rem_expr;
struct Neg_expr;
struct Pos_expr;
struct Eq_expr;
struct Ne_expr;
struct Lt_expr;
struct Gt_expr;
struct Le_expr;
struct Ge_expr;
struct And_expr;
struct Or_expr;
struct Not_expr;
struct Assign_expr;


// The base class of all expresions.
struct Expr : Term
{
  struct Visitor;

  Expr(Type* t)
    : ty(t)
  { }

  virtual void accept(Visitor&) const = 0;

  Type const& type() const { return *ty; }

  Type* ty;
};


// The visitor for expressions.
struct Expr::Visitor
{
  virtual void visit(Boolean_expr const&) { }
  virtual void visit(Integer_expr const&) { }
  virtual void visit(Real_expr const&) { }
  virtual void visit(Reference_expr const&) { }
  virtual void visit(Add_expr const&) { }
  virtual void visit(Sub_expr const&) { }
  virtual void visit(Mul_expr const&) { }
  virtual void visit(Div_expr const&) { }
  virtual void visit(Rem_expr const&) { }
  virtual void visit(Neg_expr const&) { }
  virtual void visit(Pos_expr const&) { }
  virtual void visit(Eq_expr const&) { }
  virtual void visit(Ne_expr const&) { }
  virtual void visit(Lt_expr const&) { }
  virtual void visit(Gt_expr const&) { }
  virtual void visit(Le_expr const&) { }
  virtual void visit(Ge_expr const&) { }
  virtual void visit(And_expr const&) { }
  virtual void visit(Or_expr const&) { }
  virtual void visit(Not_expr const&) { }
  virtual void visit(Assign_expr const&) { }
};


// The base class of all literal values.
struct Literal_expr : Expr
{
  Literal_expr(Type* t, Symbol const* s)
    : Expr(t), sym(s)
  { }

  Symbol const& value() const { return *sym; }

  Symbol const* sym;
};


// The base class of all unary expressions.
struct Unary_expr : Expr
{
  Unary_expr(Type* t, Expr* e)
    : Expr(t), first(e)
  { }

  Expr const& operand() const { return *first; }

  Expr* first;
};


// The base class of all binary expressions.
struct Binary_expr : Expr
{
  Binary_expr(Type* t, Expr* e1, Expr* e2)
    : Expr(t), first(e1), second(e2)
  { }

  Expr const& left() const  { return *first; }
  Expr const& right() const { return *second; }

  Expr* first;
  Expr* second;
};


// A boolean literal.
struct Boolean_expr : Literal_expr
{
  using Literal_expr::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// An integer-valued literal.
struct Integer_expr : Literal_expr
{
  using Literal_expr::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A real-valued literal.
struct Real_expr : Literal_expr
{
  using Literal_expr::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A reference to a single declaration.
//
// TODO: Subclass for variables and functions? Also, add unresolved
// identifiers and referneces to overload sets.
struct Reference_expr : Expr
{
  Reference_expr(Type* t, Name* n, Decl* d)
    : Expr(t), decl(d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Name const& name() const        { return *id; }
  Decl const& declaration() const { return *decl; }

  Name* id;
  Decl* decl;
};


// An addition express.
struct Add_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A subtraction expression.
struct Sub_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A multiplication expression.
struct Mul_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A division expression.
struct Div_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A remainder expression.
struct Rem_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A negation expression.
struct Neg_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A identity expression.
struct Pos_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// An equality expression.
struct Eq_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// An inequality expression.
struct Ne_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A less-than expression.
struct Lt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A greater-than expression.
struct Gt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A less-equal expression.
struct Le_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A greater-equal expression.
struct Ge_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A logical and expression.
struct And_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A logical or expression.
struct Or_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


struct Not_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// An assignment expresion.
struct Assign_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// A generic visitor for expressions.
template<typename F, typename T>
struct Generic_expr_visitor : Expr::Visitor, Generic_visitor<F, T>
{
  Generic_expr_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Boolean_expr const& e)   { this->invoke(e); }
  void visit(Integer_expr const& e)   { this->invoke(e); }
  void visit(Real_expr const& e)      { this->invoke(e); }
  void visit(Reference_expr const& e) { this->invoke(e); }
  void visit(Add_expr const& e)       { this->invoke(e); }
  void visit(Sub_expr const& e)       { this->invoke(e); }
  void visit(Mul_expr const& e)       { this->invoke(e); }
  void visit(Div_expr const& e)       { this->invoke(e); }
  void visit(Rem_expr const& e)       { this->invoke(e); }
  void visit(Neg_expr const& e)       { this->invoke(e); }
  void visit(Pos_expr const& e)       { this->invoke(e); }
  void visit(Eq_expr const& e)        { this->invoke(e); }
  void visit(Ne_expr const& e)        { this->invoke(e); }
  void visit(Lt_expr const& e)        { this->invoke(e); }
  void visit(Gt_expr const& e)        { this->invoke(e); }
  void visit(Le_expr const& e)        { this->invoke(e); }
  void visit(Ge_expr const& e)        { this->invoke(e); }
  void visit(And_expr const& e)       { this->invoke(e); }
  void visit(Or_expr const& e)        { this->invoke(e); }
  void visit(Not_expr const& e)       { this->invoke(e); }
  void visit(Assign_expr const& e)    { this->invoke(e); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Boolean_expr const&)>::type>
inline T
apply(Expr const& e, F fn)
{
  Generic_expr_visitor<F, T> vis(fn);
  return accept(e, vis);
}


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
  // List* first;
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
  // List* first;  // bases
  // List* second; // members
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
struct Class_decl;
struct Union_decl;
struct Enum_decl;
struct Namespace_decl;
struct Template_decl;
struct Object_parm;
struct Value_parm;
struct Type_parm;
struct Template_parm;
struct Variadic_parm;


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
  Name const& name() const    { return *first; }
  Name const& qualified_id() const;
  Name const& fully_qualified_id() const;

  Specifier spec;
  Decl*     cxt;
  Name*     first;
};


// The declaration visitor.
struct Decl::Visitor
{
  virtual void visit(Variable_decl const&)  { }
  virtual void visit(Constant_decl const&)  { }
  virtual void visit(Function_decl const&)  { }
  virtual void visit(Class_decl const&)     { }
  virtual void visit(Union_decl const&)     { }
  virtual void visit(Enum_decl const&)      { }
  virtual void visit(Namespace_decl const&) { }
  virtual void visit(Template_decl const&)  { }
  virtual void visit(Object_parm const&)    { }
  virtual void visit(Value_parm const&)     { }
  virtual void visit(Type_parm const&)      { }
  virtual void visit(Template_parm const&)  { }
  virtual void visit(Variadic_parm const&)  { }
};


// Declares a variable, constant, or function parameter.
struct Object_decl : Decl
{
  Object_decl(Name* n, Type* t, Init* i)
    : Decl(n), second(t), third(i)
  { }

  Object_decl(Decl* cxt, Name* n, Type* t, Init* i)
    : Decl(cxt, n), second(t), third(i)
  { }

  Type const& type() const { return *second; }
  Type& type()             { return *second; }

  Type* second;
  Init* third;
};


// Declares a class, union, enum, or generic type.
struct Type_decl : Decl
{
  Type_decl(Name* n, Init* i)
    : Decl(n), second(i)
  { }

  Init* second;
};


// Declares a variable.
struct Variable_decl : Object_decl
{
  Variable_decl(Name* n, Type* t, Init* i)
    : Object_decl(n, t, i)
  { }

  Variable_decl(Decl* cxt, Name* n, Type* t, Init* i)
    : Object_decl(cxt, n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& initializer() const { return *third; }
};


// Declares a symbolic constant.
struct Constant_decl : Object_decl
{
  Constant_decl(Name* n, Type* t, Init* i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& initializer() const { return *third; }
};


// Declares a function.
//
// TODO: Write type/return type accessors.
struct Function_decl : Decl
{
  Function_decl(Name* n, Type* t, Decl_list const& p, Init* i)
    : Decl(n), second(t), third(p), fourth(i)
  { }

  Function_decl(Decl* cxt, Name* n, Type* t, Decl_list const& p, Init* i)
    : Decl(cxt, n), second(t), third(p), fourth(i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Function_type const& type() const        { return *cast<Function_type>(second); }
  Type const&          return_type() const { return type().return_type(); }

  Decl_list const& parameters() const { return third; }
  Init const&      definition() const { return *fourth; }

  Type*     second;
  Decl_list third;
  Init*     fourth;
};


struct Class_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(*this); }

  Init const& definition() const { return *second; }
};


struct Union_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(*this); }

  Init const& definition() const { return *second; }
};


struct Enum_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(*this); }

  Init const& definition() const { return *second; }
};


// Defines a namespace.
//
// TODO: How should I model re-opened namespaces? Probably
// just do the lookup and re-establish the context.
struct Namespace_decl : Decl
{
  Namespace_decl(Name* n)
    : Decl(n), second()
  { }

  Namespace_decl(Decl* cxt, Name* n)
    : Decl(cxt, n), second()
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  bool is_global() const    { return cxt == nullptr; }
  bool is_anonymous() const { return is<Placeholder_id>(first); }

  Decl_list second;
};


// Declares a template.
struct Template_decl : Decl
{
  Template_decl(Decl_list const& p, Decl* d)
    : Decl(d->first), second(p), third(d)
  {
    lingo_assert(!d->cxt);
    d->cxt = this;
  }

  void accept(Visitor& v) const { v.visit(*this); }

  Decl_list const& parameters() const { return second; }
  Decl const&      pattern() const    { return *third; }

  Decl_list second;
  Decl*     third;
};


// An object paramter of a function.
struct Object_parm : Object_decl
{
  Object_parm(Name* n, Type* t, Init* i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& default_argument() const { return *third; }
};


// A constant value parameter of a template.
struct Value_parm : Object_decl
{
  Value_parm(Name* n, Type* t, Init* i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& default_argument() const { return *third; }
};


// A type parameter of a template.
struct Type_parm : Type_decl
{
  Type_parm(Name* n, Init* i)
    : Type_decl(n, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& default_argument() const { return *second; }
};


// Represents an unspecified sequence of arguments. This
// is distinct from a parameter pack.
//
// Note that we allow the variadic parameter to be named although
// the variadic parameter has a canonical name (...).
struct Variadic_parm : Decl
{
  Variadic_parm(Name* n)
    : Decl(n)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
};


// A template parameter of a template.
//
// The nested effectively denotes the "kind" of the template. It
// must be a template declaration. The name of the parameter and
// that of the underlying declaration must be the same.
struct Template_parm : Decl
{
  Template_parm(Name* n, Decl* t, Init* i)
    : Decl(n), second(t), third(i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Decl const& declaration() const { return *second; }
  Init const& default_argument() const { return *third; }

  Decl*     second;
  Init*     third;
};



// A generic visitor for names.
template<typename F, typename T>
struct Generic_decl_visitor : Decl::Visitor, Generic_visitor<F, T>
{
  Generic_decl_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Variable_decl const& d)  { this->invoke(d); }
  void visit(Constant_decl const& d)  { this->invoke(d); }
  void visit(Function_decl const& d)  { this->invoke(d); }
  void visit(Class_decl const& d)     { this->invoke(d); }
  void visit(Union_decl const& d)     { this->invoke(d); }
  void visit(Enum_decl const& d)      { this->invoke(d); }
  void visit(Namespace_decl const& d) { this->invoke(d); }
  void visit(Template_decl const& d)  { this->invoke(d); }
  void visit(Object_parm const& d)    { this->invoke(d); }
  void visit(Value_parm const& d)     { this->invoke(d); }
  void visit(Type_parm const& d)      { this->invoke(d); }
  void visit(Template_parm const& d)  { this->invoke(d); }
  void visit(Variadic_parm const& d)  { this->invoke(d); }
};


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl const&)>::type>
inline T
apply(Decl const& d, F fn)
{
  Generic_decl_visitor<F, T> vis(fn);
  return accept(d, vis);
}


// -------------------------------------------------------------------------- //
// Miscellaneous

struct Translation_unit : Term
{
  Decl_list first;
};


// -------------------------------------------------------------------------- //
// Implementation


inline Class_decl const&
Class_type::declaration() const
{
  return *cast<Class_decl>(decl);
}


inline Union_decl const&
Union_type::declaration() const
{
  return *cast<Union_decl>(decl);
}


inline Enum_decl const&
Enum_type::declaration() const
{
  return *cast<Enum_decl>(decl);
}


inline Type_parm const&
Typename_type::declaration() const
{
  return *cast<Type_parm>(decl);
}


} // namespace beaker


#endif
