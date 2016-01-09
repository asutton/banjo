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


// Type qualifier flags.
//
// TODO: Make this a legitimate type.
constexpr int const_qual    = 0x01;
constexpr int volatile_qual = 0x02;


struct Qualified_type : Type
{
  Qualified_type(Type* t, int q)
    : ty(t), qual(q)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Type const& type() const { return *ty; }
  int qualifier() const    { return qual; }
  bool is_const() const    { return qual & const_qual; }
  bool is_volatile() const { return qual & volatile_qual; }

  Type* ty;
  int   qual;
};


struct Pointer_type : Type
{
  Pointer_type(Type* t)
    : ty(t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Type const& type() const { return *ty; }

  Type* ty;
};


struct Reference_type : Type
{
  Reference_type(Type* t)
    : ty(t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Type const& type() const { return *ty; }

  Type* ty;
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
  Sequence_type(Type* t)
    : ty(t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Type const& type() const { return *ty; }

  Type* ty;
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
  void visit(Function_type const& t)  { this->invoke(t); }
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
struct Call_expr;
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
  virtual void visit(Call_expr const&) { }
  virtual void visit(Assign_expr const&) { }
};


// The base class of all literal values.
struct Literal_expr : Expr
{
  Literal_expr(Type* t, Symbol const* s)
    : Expr(t), sym(s)
  { }

  Symbol const& symbol() const { return *sym; }

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


// Logical negation.
struct Not_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
};


// Represents a function call expression of the form `f(args...)`.
//
// If the `f` and `args` are non-dependent, then `f` must refer
// to a function declaration, `args...` must be the converted
// and instantiated default arguments, and the type is the return
// type of the function.
//
// If the operands are dependent, then the retun type is a fresh
// placeholder type.
//
// TODO: Consider subtyping for [virtual|open| method calls and
// unresolved calls. It would simplify code generation.
struct Call_expr : Expr
{
  Call_expr(Type* t, Expr* e, Expr_list const& a)
    : Expr(t), fn(e), args(a)
  { }

  Expr const&      function() const  { return *fn; }
  Expr_list const& arguments() const { return args; }

  Expr*     fn;
  Expr_list args;
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
  void visit(Call_expr const& e)       { this->invoke(e); }
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

struct Stmt;
struct Compound_stmt;
struct Expression_stmt;
struct Declaration_stmt;
struct Return_stmt;


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
  void accept(Visitor& v) const { v.visit(*this); }

  Stmt_list stms;
};


// A statement that evaluates an expression and discards
// the result.
struct Expression_stmt : Stmt
{
  void accept(Visitor& v) const { v.visit(*this); }

  Expr* expr;
};


// A statemnt that declares a variable.
struct Declaration_stmt : Stmt
{
  void accept(Visitor& v) const { v.visit(*this); }

  Decl* decl;
};


// A return statement.
struct Return_stmt : Stmt
{
  Return_stmt(Expr* e)
    : expr(e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Expr const& expression() const { return *expr; }

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


// -------------------------------------------------------------------------- //
// Object initializers
//
// TODO: Consider adding deleted initializers.
//
// TODO: Provide explicit support for trivial initialization?

struct Init;
struct Default_init;
struct Value_init;
struct Reference_init;
struct Direct_init;
struct Aggregate_init;
struct Type_init;
struct Template_init;


// An object initializer provides a value for a constant,
// variable, or parameter (of various kind).
struct Init : Term
{
  struct Visitor;
};


// The visitor for initializers.
struct Init::Visitor
{
  virtual void visit(Default_init const&)   { }
  virtual void visit(Value_init const&)     { }
  virtual void visit(Reference_init const&) { }
  virtual void visit(Direct_init const&)    { }
  virtual void visit(Aggregate_init const&) { }
  virtual void visit(Type_init const&)      { }
  virtual void visit(Template_init const&)  { }
};


// A declaration can be defined to have a default value,
// or in the case of functions, a default behavior.
struct Default_init : Init
{
};


// A declaration can be defined to have a given value,
// or in the case of functions, an expression.
struct Value_init : Init
{
  Expr* expr;
};


struct Reference_init : Init
{
  Expr* expr;
};


// A variable declaration can be directly initialized by a
// constructor corresponding to the given arguments.
struct Direct_init : Init
{
  Expr_list args;
};


// A variable declaration can be initialized by specifying all
// of its fields as an aggregate.
struct Aggregate_init : Init
{
  Expr_list inits;
};


// Represents the initialization of a type parameter by a type.
struct Type_init : Init
{
  Type* ty;
};


// Represents the initialization of a template parameter by
// a template declaration.
struct Template_init : Init
{
  Decl* decl;
};


// A generic visitor for initializers.
template<typename F, typename T>
struct Generic_init_visitor : Init::Visitor, Generic_visitor<F, T>
{
  Generic_init_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Default_init const& i)   { this->invoke(i); }
  void visit(Value_init const& i)     { this->invoke(i); }
  void visit(Reference_init const& i) { this->invoke(i); }
  void visit(Direct_init const& i)    { this->invoke(i); }
  void visit(Aggregate_init const& i) { this->invoke(i); }
  void visit(Type_init const& i)      { this->invoke(i); }
  void visit(Template_init const& i)  { this->invoke(i); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Default_init const&)>::type>
inline T
apply(Init const& i, F fn)
{
  Generic_init_visitor<F, T> vis(fn);
  return accept(i, vis);
}


// -------------------------------------------------------------------------- //
// Function and type definitions

struct Def;
struct Defaulted_def;
struct Deleted_def;
struct Function_def;
struct Class_def;
struct Union_def;
struct Enum_def;


// Denotes the set of definitions for functions and types.
//
// Note that the inclusion of definitions for both functions
// and types is largely one of syntactic convenience. Care
// must be taken to ensure that a class declaration does not
// have e.g., a function definition (that doesn't make sense).
struct Def
{
  struct Visitor;

  virtual void accept(Visitor&) = 0;
};


// Visitor for definitions.
struct Def::Visitor
{
  virtual void visit(Defaulted_def const&) { }
  virtual void visit(Deleted_def const&)   { }
  virtual void visit(Function_def const&)  { }
  virtual void visit(Class_def const&)     { }
  virtual void visit(Union_def const&)     { }
  virtual void visit(Enum_def const&)      { }
};


// A defaulted definition has a specification determined by
// the compiler.
//
// C++ allows only defaulted special functions, but this can
// be made far more general.
struct Defaulted_def : Def
{
};


// A deleted definition is specified to be invalid.
//
// C++ allows deleted functions, but deleted classes (and also
// variables) make sense for partial specializations.
struct Deleted_def : Def
{
};


// A function declaration can be initialized by a compound
// statement.
//
// TODO: Provide extended support for member initialization
// lists of member functions.
struct Function_def : Def
{
  Stmt* first;
};


// A definition of a class.
struct Class_def : Def
{
  // List* first;  // bases
  // List* second; // members
};


// A definition of a union.
struct Union_def : Def
{
};


// A definition of an enumeration.
struct Enum_def : Def
{
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
  void visit(Function_def const& d)   { this->invoke(d); }
  void visit(Class_def const& d)      { this->invoke(d); }
  void visit(Union_def const& d)      { this->invoke(d); }
  void visit(Enum_def const& d)       { this->invoke(d); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Default_init const&)>::type>
inline T
apply(Def const& d, F fn)
{
  Generic_def_visitor<F, T> vis(fn);
  return accept(d, vis);
}


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
  Type_decl(Name* n, Def* i)
    : Decl(n), def(i)
  { }

  Def* def;
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
// A function has three associated exprssions:
//    - a type constraint which governs use,
//    - a precondition which guards entry, and
//    - a postcondition that explicitly states effects.
struct Function_decl : Decl
{
  Function_decl(Name* n, Type* t, Decl_list const& p, Def* i)
    : Decl(n), ty(t), parms(p), def(i)
  { }

  Function_decl(Decl* cxt, Name* n, Type* t, Decl_list const& p, Def* i)
    : Decl(cxt, n), ty(t), parms(p), def(i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Function_type const& type() const        { return *cast<Function_type>(ty); }
  Type const&          return_type() const { return type().return_type(); }

  Decl_list const& parameters() const    { return parms; }
  Expr const&      constraint() const    { return *constr; }
  Expr const&      precondition() const  { return *constr; }
  Expr const&      postcondition() const { return *constr; }
  Def const&       definition() const    { return *def; }

  bool is_defined() const  { return def; }

  Type*     ty;
  Decl_list parms;
  Expr*     constr;
  Expr*     pre;
  Expr*     post;
  Def*     def;
};


struct Class_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(*this); }

  Def const& definition() const { return *def; }

  bool is_defined() const { return def; }
};


struct Union_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(*this); }

  Def const& definition() const { return *def; }

  bool is_defined() const { return def; }
};


struct Enum_decl : Type_decl
{
  void accept(Visitor& v) const { v.visit(*this); }

  Def const& definition() const { return *def; }

  bool is_defined() const { return def; }
};


// Defines a namespace.
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
//
// A template has a single constraint expression corresponding
// to a requires clause. Note that this is transformed into
// a logical proposition for the purpose of constraint checking
// and comparison.
struct Template_decl : Decl
{
  Template_decl(Decl_list const& p, Decl* d)
    : Decl(d->first), parms(p), decl(d)
  {
    lingo_assert(!d->cxt);
    d->cxt = this;
  }

  void accept(Visitor& v) const { v.visit(*this); }

  Decl_list const& parameters() const { return parms; }
  Expr const&      constraint() const { return *constr; }
  Decl const&      pattern() const    { return *decl; }

  bool is_constrained() const { return constr; }

  Decl_list parms;
  Expr*     constr;
  Decl*     decl;
};


// An object paramter of a function.
struct Object_parm : Object_decl
{
  Object_parm(Name* n, Type* t, Init* i = nullptr)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& default_argument() const { return *third; }
};


// A constant value parameter of a template.
struct Value_parm : Object_decl
{
  Value_parm(Name* n, Type* t, Init* i = nullptr)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& default_argument() const { return *third; }

  bool has_default_arguement() const { return third; }
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


// A type parameter of a template.
struct Type_parm : Decl
{
  Type_parm(Name* n, Init* d = nullptr)
    : Decl(n), def(d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Init const& default_argument() const { return *def; }

  bool has_default_arguement() const { return def; }

  Init* def;
};


// A template parameter of a template.
//
// The nested effectively denotes the "kind" of the template. It
// must be a template declaration. The name of the parameter and
// that of the underlying declaration must be the same.
struct Template_parm : Decl
{
  Template_parm(Name* n, Decl* t, Init* d)
    : Decl(n), temp(t), def(d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Decl const& declaration() const      { return *temp; }
  Init const& default_argument() const { return *def; }

  bool has_default_arguement() const { return def; }

  Decl*     temp;
  Init*     def;
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
