// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_HPP
#define BANJO_AST_HPP

#include "prelude.hpp"

#include <lingo/integer.hpp>
#include <lingo/real.hpp>

#include <vector>
#include <utility>

namespace banjo
{

struct Term;
struct Name;
struct Type;
struct Expr;
struct Stmt;
struct Decl;

struct Scope;

using lingo::Integer;


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

  List_iterator(List_iterator<T> i)
    : iter(i.iter)
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
  using base_type = std::vector<T*>;
  using iterator = List_iterator<T>;
  using const_iterator = List_iterator<T const>;

  List() = default;

  List(base_type const& x)
    : base_type(x)
  { }

  List(base_type&& x)
    : base_type(std::move(x))
  { }

  List(std::initializer_list<T*> list)
    : base_type(list)
  { }

  std::vector<T*> const& base() const { return *this; }
  std::vector<T*>&       base()       { return *this; }

  T const& front() const { return *base().front(); }
  T&       front()       { return *base().front(); }

  T const& back() const { return *base().back(); }
  T&       back()       { return *base().back(); }

  void push_back(T& x) { base().push_back(&x); }
  void push_back(T* x) { base().push_back(x); }

  template<typename I>
  void append(I, I);

  iterator begin() { return base().begin(); }
  iterator end()   { return base().end(); }

  const_iterator begin() const { return base().begin(); }
  const_iterator end() const   { return base().end(); }
};


// Insert a range of iterators at the end of the list.
template<typename T>
template<typename I>
inline void
List<T>::append(I first, I last)
{
  while (first != last) {
    push_back(*first);
    ++first;
  }
}



// Lists
using Term_list = List<Term>;
using Type_list = List<Type>;
using Expr_list = List<Expr>;
using Stmt_list = List<Stmt>;
using Decl_list = List<Decl>;


// Iterators
using Term_iter = Term_list::iterator;
using Type_iter = Type_list::iterator;
using Expr_iter = Expr_list::iterator;
using Decl_iter = Decl_list::iterator;


// Pairs and tuples
using Expr_pair = std::pair<Expr&, Expr&>;


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


// A name denotes an entity in a progam. Most forms of names are
// ids, which denote use the of those entities in expressions,
// or types.
struct Name : Term
{
  struct Visitor;

  virtual void accept(Visitor&) const = 0;

  // Returns an unqualified representation of the name.
  virtual Name const& unqualified_name() const { return *this; }
  virtual Name&       unqualified_name()       { return *this; }
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
  Simple_id(Symbol const& sym)
    : first(&sym)
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
//
// FIXME: Call this something else?
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


struct Template_decl;


// An identifier that refers a template specialization.
//
// FIXME: Make the declaration a template declaration.
struct Template_id : Name
{
  Template_id(Decl& d, Term_list const& a)
    : decl(&d), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); };

  Template_decl const& declaration() const;
  Template_decl&       declaration();

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


bool is_object_type(Type const&);


// Represents a set of type qualifiers.
enum Qualifier_set
{
  empty_qual    = 0x00,
  const_qual    = 0x01,
  volatile_qual = 0x02,
  total_qual    = const_qual | volatile_qual
};


inline Qualifier_set&
operator|=(Qualifier_set& a, Qualifier_set b)
{
  return a = Qualifier_set(a | b);
}


// Returns true if a is a superset of b.
inline bool
is_superset(Qualifier_set a, Qualifier_set b)
{
  return (a & b) == b;
}


// Returns true if a is strictly more qualified than b.
inline bool
is_more_qualified(Qualifier_set a, Qualifier_set b)
{
  return is_superset(a, b) && a != b;
}


// The base class of all types.
struct Type : Term
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&)       = 0;

  // Returns the qualifier for this type.
  virtual Qualifier_set qualifier() const   { return empty_qual; }
  virtual bool          is_const() const    { return false; }
  virtual bool          is_volatile() const { return false; }
  bool                  is_qualified() { return qualifier() != empty_qual; }

  // Returns the unqualified version of this type.
  virtual Type const& unqualified_type() const { return *this; }
  virtual Type&       unqualified_type()       { return *this; }
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


struct Type::Mutator
{
  virtual void visit(Void_type&)      { }
  virtual void visit(Boolean_type&)   { }
  virtual void visit(Integer_type&)   { }
  virtual void visit(Float_type&)     { }
  virtual void visit(Auto_type&)      { }
  virtual void visit(Decltype_type&)  { }
  virtual void visit(Declauto_type&)  { }
  virtual void visit(Function_type&)  { }
  virtual void visit(Qualified_type&) { }
  virtual void visit(Pointer_type&)   { }
  virtual void visit(Reference_type&) { }
  virtual void visit(Array_type&)     { }
  virtual void visit(Sequence_type&)  { }
  virtual void visit(Class_type&)     { }
  virtual void visit(Union_type&)     { }
  virtual void visit(Enum_type&)      { }
  virtual void visit(Typename_type&)  { }
};


// The void type.
struct Void_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The boolean type.
struct Boolean_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
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
    : sgn(s), prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  bool sign() const        { return sgn; }
  bool is_signed() const   { return sgn; }
  bool is_unsigned() const { return !sgn; }
  int  precision() const   { return prec; }

  bool sgn;
  int  prec;
};


// The IEEE 754 floating point types.
struct Float_type : Type
{
  Float_type(int p = 64)
    : prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  int precision() const { return prec; }

  int prec;
};


// The auto type.
//
// TODO: Allow a deduction constraint on placeholder types.
// This would be checked after deduction. Extend this for
// decltype(auto) types as well.
//
// FIXME: The model for auto types works like this:
//
//    auto x = e;
//
// Is essentially shorthand for writing:
//
//    typename T;
//    T x = e;
//
// So x would have typename-type and not auto-type. If we
// add constraints, we might represent that internally as
// this.
//
//    typename T;
//    T|C x = e;  // |C means give C.
//
// We could eliminate this node entirely, and the declauto
// node, except that we would to encode the deduction mechanism
// into the tree somehow. Maybe make deduction a property of
// of the typename declaration (i.e., a decltype decl).
struct Auto_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The type decltype(e).
struct Decltype_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The type decltype(auto).
struct Declauto_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A function type.
struct Function_type : Type
{
  Function_type(Type_list const& p, Type& r)
    : parms(p), ret(&r)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type_list const& parameter_types() const { return parms; }
  Type_list&       parameter_types()       { return parms; }

  Type const&      return_type() const     { return *ret; }
  Type&            return_type()           { return *ret; }

  Type_list parms;
  Type*     ret;
};


struct Qualified_type : Type
{
  Qualified_type(Type& t, Qualifier_set q)
    : ty(&t), qual(q)
  {
    lingo_assert(q != empty_qual);
    lingo_assert(!is<Qualified_type>(ty));
  }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  // Returns the qualifier for this type. Note that these
  // override functions in type.
  Qualifier_set qualifier() const   { return qual; }
  bool          is_const() const    { return qual & const_qual; }
  bool          is_volatile() const { return qual & volatile_qual; }

  // Returns the unqualfiied version of this type.
  Type const& unqualified_type() const { return type(); }
  Type&       unqualified_type()       { return type(); }

  Type*         ty;
  Qualifier_set qual;
};


struct Pointer_type : Type
{
  Pointer_type(Type& t)
    : ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
};


struct Reference_type : Type
{
  Reference_type(Type& t)
    : ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
};


struct Array_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type* first;
  Expr* second;
};


// The type of an unspecified sequence of objects. An array
// of unknown bound.
struct Sequence_type : Type
{
  Sequence_type(Type& t)
    : ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
};


// The base class of all user-defined types.
struct User_defined_type : Type
{
  User_defined_type(Decl& d)
    : decl(&d)
  { }

  Decl const& declaration() const { return *decl; }
  Decl&       declaration()       { return *decl; }

  Decl* decl;
};


struct Class_decl;
struct Union_decl;
struct Enum_decl;
struct Type_parm;


// TODO: Factor a base class for all of these: user-defined type.
struct Class_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the class type.
  Class_decl const& declaration() const;
  Class_decl&       declaration();
};


struct Union_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the union type.
  Union_decl const& declaration() const;
  Union_decl&       declaration();
};


struct Enum_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the enum type.
  Enum_decl const& declaration() const;
  Enum_decl&       declaration();
};


// The type of a type parameter declaration.
//
// FIXME: Guarantee that d is a Type_parm.
struct Typename_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the typename type.
  Type_parm const& declaration() const;
  Type_parm&       declaration();
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
inline decltype(auto)
apply(Type const& t, F fn)
{
  Generic_type_visitor<F, T> vis(fn);
  return accept(t, vis);
}


// A generic mutator for types.
template<typename F, typename T>
struct Generic_type_mutator : Type::Mutator, Generic_mutator<F, T>
{
  Generic_type_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Void_type& t)      { this->invoke(t); }
  void visit(Boolean_type& t)   { this->invoke(t); }
  void visit(Integer_type& t)   { this->invoke(t); }
  void visit(Float_type& t)     { this->invoke(t); }
  void visit(Auto_type& t)      { this->invoke(t); }
  void visit(Decltype_type& t)  { this->invoke(t); }
  void visit(Declauto_type& t)  { this->invoke(t); }
  void visit(Function_type& t)  { this->invoke(t); }
  void visit(Qualified_type& t) { this->invoke(t); }
  void visit(Pointer_type& t)   { this->invoke(t); }
  void visit(Reference_type& t) { this->invoke(t); }
  void visit(Array_type& t)     { this->invoke(t); }
  void visit(Sequence_type& t)  { this->invoke(t); }
  void visit(Class_type& t)     { this->invoke(t); }
  void visit(Union_type& t)     { this->invoke(t); }
  void visit(Enum_type& t)      { this->invoke(t); }
  void visit(Typename_type& t)  { this->invoke(t); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_type&)>::type>
inline decltype(auto)
apply(Type& t, F fn)
{
  Generic_type_mutator<F, T> vis(fn);
  return accept(t, vis);
}

// -------------------------------------------------------------------------- //
// Expressions
//
// TODO: Add bitwise operations.

// Literals
struct Boolean_expr;
struct Integer_expr;
struct Real_expr;
// Expressions
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
// Conversions
struct Value_conv;
struct Qualification_conv;
struct Boolean_conv;
struct Integer_conv;
struct Float_conv;
struct Numeric_conv;
struct Ellipsis_conv;
// Initialization
struct Trivial_init;
struct Copy_init;
struct Bind_init;
struct Direct_init;
struct Aggregate_init;


// The base class of all expresions.
struct Expr : Term
{
  struct Visitor;
  struct Mutator;

  Expr()
    : ty(nullptr)
  { }

  Expr(Type& t)
    : ty(&t)
  { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

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
  virtual void visit(Value_conv const&) { }
  virtual void visit(Qualification_conv const&) { }
  virtual void visit(Boolean_conv const&) { }
  virtual void visit(Integer_conv const&) { }
  virtual void visit(Float_conv const&) { }
  virtual void visit(Numeric_conv const&) { }
  virtual void visit(Ellipsis_conv const&) { }
  virtual void visit(Trivial_init const&) { }
  virtual void visit(Copy_init const&) { }
  virtual void visit(Bind_init const&) { }
  virtual void visit(Direct_init const&) { }
  virtual void visit(Aggregate_init const&) { }
};

struct Expr::Mutator
{
  virtual void visit(Boolean_expr&) { }
  virtual void visit(Integer_expr&) { }
  virtual void visit(Real_expr&) { }
  virtual void visit(Reference_expr&) { }
  virtual void visit(Add_expr&) { }
  virtual void visit(Sub_expr&) { }
  virtual void visit(Mul_expr&) { }
  virtual void visit(Div_expr&) { }
  virtual void visit(Rem_expr&) { }
  virtual void visit(Neg_expr&) { }
  virtual void visit(Pos_expr&) { }
  virtual void visit(Eq_expr&) { }
  virtual void visit(Ne_expr&) { }
  virtual void visit(Lt_expr&) { }
  virtual void visit(Gt_expr&) { }
  virtual void visit(Le_expr&) { }
  virtual void visit(Ge_expr&) { }
  virtual void visit(And_expr&) { }
  virtual void visit(Or_expr&) { }
  virtual void visit(Not_expr&) { }
  virtual void visit(Call_expr&) { }
  virtual void visit(Assign_expr&) { }
  virtual void visit(Value_conv&) { }
  virtual void visit(Qualification_conv&) { }
  virtual void visit(Boolean_conv&) { }
  virtual void visit(Integer_conv&) { }
  virtual void visit(Float_conv&) { }
  virtual void visit(Numeric_conv&) { }
  virtual void visit(Ellipsis_conv&) { }
  virtual void visit(Trivial_init&) { }
  virtual void visit(Copy_init&) { }
  virtual void visit(Bind_init&) { }
  virtual void visit(Direct_init&) { }
  virtual void visit(Aggregate_init&) { }
};


// The family of base classes for literal. This holds a value
// of the parameterized type.
//
// TODO: Integrate this with a value system?
template<typename T>
struct Literal_expr : Expr
{
  Literal_expr(Type& t, T const& x)
    : Expr(t), val(x)
  { }

  // Returns the interpreted value of the literal.
  T const& value() const { return val; }
  T&       value()       { return val; }

  T val;
};


// The base class of all unary expressions.
struct Unary_expr : Expr
{
  Unary_expr(Type& t, Expr& e)
    : Expr(t), first(&e)
  { }

  Expr const& operand() const { return *first; }

  Expr* first;
};


// The base class of all binary expressions.
struct Binary_expr : Expr
{
  Binary_expr(Type& t, Expr& e1, Expr& e2)
    : Expr(t), first(&e1), second(&e2)
  { }

  Expr const& left() const { return *first; }
  Expr&       left()       { return *first; }

  Expr const& right() const { return *second; }
  Expr&       right()       { return *second; }

  Expr* first;
  Expr* second;
};


// A boolean literal.
struct Boolean_expr : Literal_expr<bool>
{
  using Literal_expr<bool>::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An integer-valued literal.
struct Integer_expr : Literal_expr<Integer>
{
  using Literal_expr<Integer>::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A real-valued literal.
struct Real_expr : Literal_expr<lingo::Real>
{
  using Literal_expr<Real>::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A reference to a single declaration.
//
// TODO: Subclass for variables, constants, and functions.
// Unresolved identifiers are also interesting. These should be
// called Variable_expr, Constant_expr, and Function_expr,
// respectively.
struct Reference_expr : Expr
{
  Reference_expr(Type& t,Decl& d)
    : Expr(t), decl(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced declaration.
  Decl const& declaration() const { return *decl; }
  Decl&       declaration()       { return *decl; }

  Decl* decl;
};


// An addition express.
struct Add_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A subtraction expression.
struct Sub_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A multiplication expression.
struct Mul_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A division expression.
struct Div_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A remainder expression.
struct Rem_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A negation expression.
struct Neg_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A identity expression.
struct Pos_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An equality expression.
struct Eq_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An inequality expression.
struct Ne_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A less-than expression.
struct Lt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A greater-than expression.
struct Gt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A less-equal expression.
struct Le_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A greater-equal expression.
struct Ge_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A logical and expression.
struct And_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A logical or expression.
struct Or_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Logical negation.
struct Not_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents a function call expression of the form `f(args...)`.
// The function arguments are initializers for the declared parameters.
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
  Call_expr(Type& t, Expr& e, Expr_list const& a)
    : Expr(t), fn(&e), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& function() const { return *fn; }
  Expr&       function()       { return *fn; }

  Expr_list const& arguments() const { return args; }
  Expr_list&       arguments()       { return args; }

  Expr*     fn;
  Expr_list args;
};


// An assignment expresion.
struct Assign_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the set of standard conversions. A conversion
// has a source expression and target type. Each derived
// conversion contains the logic needed to transform the
// value computed by the source expression into the target
// type. Note that the target type is also the type of the
// expression.
//
// FIXME: Embed as much information in type conversions as possible.
// Essentially we want to duplicate the set of conversions that
// would be applied in the IR. That frees us from dealing with
// complex code generation implementations.
struct Conv : Expr
{
  Conv(Type& t, Expr& e)
    : Expr(t), expr(&e)
  { }

  // Returns the destination type of the conversion. This is the
  // same as the expression's type.
  Type const& destination() const { return type(); }
  Type&       destination()       { return type(); }

  // Returns the converted expression (the operand).
  Expr const& source() const { return *expr; }
  Expr&       source()       { return *expr; }

  Expr* expr;
};


// A grouping class. All standard conversions derive frrom
// this type.
struct Standard_conv : Conv
{
  using Conv::Conv;
};


// A conversion from an object to a value.
struct Value_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from a less cv-qualified type to a more
// cv-qualified type.
struct Qualification_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one integer type to another.
struct Boolean_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one integer type to another.
struct Integer_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one floating point type to another.
struct Float_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from integer to floating point type.
//
// TODO: Integrate this with the float conversion?
struct Numeric_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the conversion of an argument type the ellipsis
// parameter.
struct Ellipsis_conv : Conv
{
  using Conv::Conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An initializer is an expressio that provides a value for an
// object or reference.
//
// TODO: Should all initializers have type void?
struct Init : Expr
{
  using Expr::Expr;
};


// Represents the absence of initialization for an object. This
// is selected by zero initialization of references and by the
// default constrution of trivially constructible class and union
// types.
struct Trivial_init : Init
{
  using Init::Init;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the initialization of an object by an expression.
// The result of the expression is copied (or moved?) into a
// target object.
struct Copy_init : Init
{
  Copy_init(Type& t, Expr& e)
    : Init(t), expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the source expression.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// Represents the initialization of a reference by an expression.
// The declared reference is bound to the result of the expression.
struct Bind_init : Init
{
  Bind_init(Type& t, Expr& e)
    : Init(t), expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the source expression.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// Represents the initialization of an class or union object by a
// constructor.
struct Direct_init : Init
{
  // FIXME: The type of this expression is the type of the
  // constructed object. We should be able to compute this
  // instead of passing it directly.
  Direct_init(Type& t, Decl& d, Expr_list const& a)
    : Init(t), ctor(&d), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the constructor declaration
  Decl const& consructor() const { return *ctor; }
  Decl&       consructor()       { return *ctor; }

  // Returns the argumetns supplied for direct initialization.
  Expr_list const& arguments() const { return args; }
  Expr_list&       arguments()       { return args; }

  Decl*     ctor;
  Expr_list args;
};


// Represents the recursive initialziation of a class, union,
// or array object.
struct Aggregate_init : Init
{
  Aggregate_init(Type& t, Expr_list const& es)
    : Init(t), inits(es)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns a sequence of selected initializers for
  // a compound target type.
  Expr_list const& initializers() const { return inits; }
  Expr_list&       initializers()       { return inits; }

  Expr_list inits;
};


// A generic visitor for expressions.
template<typename F, typename T>
struct Generic_expr_visitor : Expr::Visitor, Generic_visitor<F, T>
{
  Generic_expr_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Boolean_expr const& e)       { this->invoke(e); }
  void visit(Integer_expr const& e)       { this->invoke(e); }
  void visit(Real_expr const& e)          { this->invoke(e); }
  void visit(Reference_expr const& e)     { this->invoke(e); }
  void visit(Add_expr const& e)           { this->invoke(e); }
  void visit(Sub_expr const& e)           { this->invoke(e); }
  void visit(Mul_expr const& e)           { this->invoke(e); }
  void visit(Div_expr const& e)           { this->invoke(e); }
  void visit(Rem_expr const& e)           { this->invoke(e); }
  void visit(Neg_expr const& e)           { this->invoke(e); }
  void visit(Pos_expr const& e)           { this->invoke(e); }
  void visit(Eq_expr const& e)            { this->invoke(e); }
  void visit(Ne_expr const& e)            { this->invoke(e); }
  void visit(Lt_expr const& e)            { this->invoke(e); }
  void visit(Gt_expr const& e)            { this->invoke(e); }
  void visit(Le_expr const& e)            { this->invoke(e); }
  void visit(Ge_expr const& e)            { this->invoke(e); }
  void visit(And_expr const& e)           { this->invoke(e); }
  void visit(Or_expr const& e)            { this->invoke(e); }
  void visit(Not_expr const& e)           { this->invoke(e); }
  void visit(Call_expr const& e)          { this->invoke(e); }
  void visit(Assign_expr const& e)        { this->invoke(e); }
  void visit(Value_conv const& e)         { this->invoke(e); }
  void visit(Qualification_conv const& e) { this->invoke(e); }
  void visit(Boolean_conv const& e)       { this->invoke(e); }
  void visit(Integer_conv const& e)       { this->invoke(e); }
  void visit(Float_conv const& e)         { this->invoke(e); }
  void visit(Numeric_conv const& e)       { this->invoke(e); }
  void visit(Ellipsis_conv const& e)      { this->invoke(e); }
  void visit(Trivial_init const& e)       { this->invoke(e); }
  void visit(Copy_init const& e)          { this->invoke(e); }
  void visit(Bind_init const& e)          { this->invoke(e); }
  void visit(Direct_init const& e)        { this->invoke(e); }
  void visit(Aggregate_init const& e)     { this->invoke(e); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Boolean_expr const&)>::type>
inline T
apply(Expr const& e, F fn)
{
  Generic_expr_visitor<F, T> vis(fn);
  return accept(e, vis);
}


// A generic mutator for expressions.
template<typename F, typename T>
struct Generic_expr_mutator : Expr::Mutator, Generic_mutator<F, T>
{
  Generic_expr_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Boolean_expr& e)       { this->invoke(e); }
  void visit(Integer_expr& e)       { this->invoke(e); }
  void visit(Real_expr& e)          { this->invoke(e); }
  void visit(Reference_expr& e)     { this->invoke(e); }
  void visit(Add_expr& e)           { this->invoke(e); }
  void visit(Sub_expr& e)           { this->invoke(e); }
  void visit(Mul_expr& e)           { this->invoke(e); }
  void visit(Div_expr& e)           { this->invoke(e); }
  void visit(Rem_expr& e)           { this->invoke(e); }
  void visit(Neg_expr& e)           { this->invoke(e); }
  void visit(Pos_expr& e)           { this->invoke(e); }
  void visit(Eq_expr& e)            { this->invoke(e); }
  void visit(Ne_expr& e)            { this->invoke(e); }
  void visit(Lt_expr& e)            { this->invoke(e); }
  void visit(Gt_expr& e)            { this->invoke(e); }
  void visit(Le_expr& e)            { this->invoke(e); }
  void visit(Ge_expr& e)            { this->invoke(e); }
  void visit(And_expr& e)           { this->invoke(e); }
  void visit(Or_expr& e)            { this->invoke(e); }
  void visit(Not_expr& e)           { this->invoke(e); }
  void visit(Call_expr& e)          { this->invoke(e); }
  void visit(Assign_expr& e)        { this->invoke(e); }
  void visit(Value_conv& e)         { this->invoke(e); }
  void visit(Qualification_conv& e) { this->invoke(e); }
  void visit(Boolean_conv& e)       { this->invoke(e); }
  void visit(Integer_conv& e)       { this->invoke(e); }
  void visit(Float_conv& e)         { this->invoke(e); }
  void visit(Numeric_conv& e)       { this->invoke(e); }
  void visit(Ellipsis_conv& e)      { this->invoke(e); }
  void visit(Trivial_init& e)       { this->invoke(e); }
  void visit(Copy_init& e)          { this->invoke(e); }
  void visit(Bind_init& e)          { this->invoke(e); }
  void visit(Direct_init& e)        { this->invoke(e); }
  void visit(Aggregate_init& e)     { this->invoke(e); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Boolean_expr&)>::type>
inline T
apply(Expr& e, F fn)
{
  Generic_expr_mutator<F, T> vis(fn);
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
  Compound_stmt()
    : Stmt()
  { }

  Compound_stmt(Stmt_list const& ss)
    : Stmt(), stmts(ss)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  Stmt_list const& statements() const { return stmts; }
  Stmt_list&       statements()       { return stmts; }

  Stmt_list stmts;
};


// A statement that evaluates an expression and discards
// the result.
struct Expression_stmt : Stmt
{
  Expression_stmt(Expr& e)
    : expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

  // Returns the expression of the statement.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// A statemnt that declares a variable.
struct Declaration_stmt : Stmt
{
  Declaration_stmt(Decl& d)
    : decl(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }

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

  // Returns the expression returned by the statement.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

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
struct Def : Term
{
  struct Visitor;

  virtual void accept(Visitor&) const = 0;
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
  void accept(Visitor& v) const { return v.visit(*this); }
};


// A deleted definition is specified to be invalid.
//
// C++ allows deleted functions, but deleted classes (and also
// variables) make sense for partial specializations.
struct Deleted_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
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

  // Returnse the statement associated with the function
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

  // Returns the list of member declarations.
  Decl_list const& members() const { return decls; }
  Decl_list      & members()       { return decls; }

  Decl_list decls;
};


// A definition of a union.
struct Union_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
};


// A definition of an enumeration.
struct Enum_def : Def
{
  void accept(Visitor& v) const { return v.visit(*this); }
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


template<typename F, typename T = typename std::result_of<F(Defaulted_def const&)>::type>
inline decltype(auto)
apply(Def const& t, F fn)
{
  Generic_def_visitor<F, T> vis(fn);
  return accept(t, vis);
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
struct Concept_decl;
struct Object_parm;
struct Value_parm;
struct Type_parm;
struct Template_parm;
struct Variadic_parm;


// A specifier is a flag.
//
// FIXME: Make this a legitimate type.
using Specifier = std::int32_t;


// The base class of all declarations. Each declaration has a set of
// specifiers and a reference to the context in which it the entity
// is declared.
//
// TODO: Factor named and typed declarations. Variables, constants, and
// functions have names and types. Classes and namespaces have types.
// Import directives and assertions have neither.
struct Decl : Term
{
  struct Visitor;
  struct Mutator;

  Decl(Name& n)
    : spec(0), cxt(nullptr), id(&n)
  { }

  Decl(Decl& cxt, Name& n)
    : spec(0), cxt(&cxt), id(&n)
  { }

  virtual void accept(Visitor& v) const = 0;
  virtual void accept(Mutator& v) = 0;

  // Returns a pointer to the context to which this
  // declaration belongs. This is only null for the
  // global namespace.
  Decl const* context() const  { return cxt; }
  Decl*       context()        { return cxt; }
  void        context(Decl& d) { cxt = &d; }

  // Return the name of the declaration.
  Name const& declared_name() const { return id->unqualified_name(); }
  Name&       declared_name()       { return id->unqualified_name(); };

  // Returns the name with which the declaration was declared.
  // Note that this can be a qualified id.
  Name const& name() const { return *id; }
  Name&       name()       { return *id; }

  // If the declaration is a template, this returns the templated
  // declaration.
  virtual Decl const& parameterized_declaration() const { return *this; }
  virtual Decl&       parameterized_declaration()       { return *this; }


  // Returns the saved scope associated with the declaration, if any.
  // Not all declarations have an associated scope.
  virtual Scope const* scope() const { return nullptr; }
  virtual Scope*       scope()       { return nullptr; }

  Specifier spec;
  Decl*     cxt;
  Name*     id;
};


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
  virtual void visit(Concept_decl const&)  { }
  virtual void visit(Object_parm const&)    { }
  virtual void visit(Value_parm const&)     { }
  virtual void visit(Type_parm const&)      { }
  virtual void visit(Template_parm const&)  { }
  virtual void visit(Variadic_parm const&)  { }
};


struct Decl::Mutator
{
  virtual void visit(Variable_decl&)  { }
  virtual void visit(Constant_decl&)  { }
  virtual void visit(Function_decl&)  { }
  virtual void visit(Class_decl&)     { }
  virtual void visit(Union_decl&)     { }
  virtual void visit(Enum_decl&)      { }
  virtual void visit(Namespace_decl&) { }
  virtual void visit(Template_decl&)  { }
  virtual void visit(Concept_decl&)  { }
  virtual void visit(Object_parm&)    { }
  virtual void visit(Value_parm&)     { }
  virtual void visit(Type_parm&)      { }
  virtual void visit(Template_parm&)  { }
  virtual void visit(Variadic_parm&)  { }
};


// Declares a variable, constant, or function parameter.
struct Object_decl : Decl
{
  Object_decl(Name& n, Type& t)
    : Decl(n), ty(&t), init()
  { }

  Object_decl(Name& n, Type& t, Expr& e)
    : Decl(n), ty(&t), init(&e)
  { }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
  Expr* init;
};


// Declares a class, union, enum.
struct Type_decl : Decl
{
  Type_decl(Name& n)
    : Decl(n), def()
  { }

  Type_decl(Name& n, Def& i)
    : Decl(n), def(&i)
  { }

  Def const& definition() const { return *def; }
  Def&       definition()       { return *def; }

  bool is_defined() const { return def; }

  Def* def;
};


// Declares a variable.
struct Variable_decl : Object_decl
{
  Variable_decl(Name& n, Type& t)
    : Object_decl(n, t)
  { }

  Variable_decl(Name& n, Type& t, Expr& i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the initializer for the variable. This is
  // defined iff has_initializer() is true.
  Expr const& initializer() const     { return *init; }
  Expr&       initializer()           { return *init; }
  bool        has_initializer() const { return init; }
};


// Declares a symbolic constant.
struct Constant_decl : Object_decl
{
  Constant_decl(Name& n, Type& t)
    : Object_decl(n, t)
  { }

  Constant_decl(Name& n, Type& t, Expr& i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the initializer for the variable. This is
  // defined iff has_initializer() is true.
  Expr const& initializer() const     { return *init; }
  Expr&       initializer()           { return *init; }
  bool        has_initializer() const { return init; }
};


// Declares a function.
//
// A function has three associated exprssions:
//    - a type constraint which governs use,
//    - a precondition which guards entry, and
//    - a postcondition that explicitly states effects.
struct Function_decl : Decl
{
  Function_decl(Name& n, Type& t, Decl_list const& p)
    : Decl(n), ty(&t), parms(p), def()
  { }

  Function_decl(Name& n, Type& t, Decl_list const& p, Def& d)
    : Decl(n), ty(&t), parms(p), def(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the type of this declaration.
  Function_type const& type() const { return *cast<Function_type>(ty); }
  Function_type&       type()       { return *cast<Function_type>(ty); }

  // Returns the return type of the function.
  Type const& return_type() const { return type().return_type(); }
  Type&       return_type()       { return type().return_type(); }

  // Returns the list of parameter declarations for the function.
  Decl_list const& parameters() const { return parms; }
  Decl_list&       parameters()       { return parms; }

  // Returns the function constraints. This is valid iff
  // is_constrained() is true.
  Expr const& constraint() const     { return *constr; }
  Expr&       constraint()           { return *constr; }


  // TODO: Implelemnt pre- and post-conditions.
  // Expr const& precondition() const  { return *constr; }
  // Expr const& postcondition() const { return *constr; }

  Def const& definition() const    { return *def; }
  Def&       definition()          { return *def; }

  // Returns true if this declaration has function constraints.
  bool is_constrained() const { return constr; }


  // Returns true iff this declaration is also a definition.
  bool is_definition() const { return def; }

  Type*     ty;
  Decl_list parms;
  Expr*     constr;
  Expr*     pre;
  Expr*     post;
  Def*      def;
};


// Represents the declaration of a class.
struct Class_decl : Type_decl
{
  using Type_decl::Type_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the definition for the class, if given. Behavior is
  // defined iff is_definition() is true.
  Class_def const& definition() const { return *cast<Class_def>(def); }
  Class_def&       definition()       { return *cast<Class_def>(def); }

  // Returns true if the declaration is also a definition.
  bool is_definition() const { return def; }
};


struct Union_decl : Type_decl
{
  using Type_decl::Type_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Union_def const& definition() const { return *cast<Union_def>(def); }
  Union_def&       definition()       { return *cast<Union_def>(def); }
};


struct Enum_decl : Type_decl
{
  using Type_decl::Type_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Enum_def const& definition() const { return *cast<Enum_def>(def); }
  Enum_def&       definition()       { return *cast<Enum_def>(def); }
};


// Represents the definition of a namespace and its enclosed declarations.
// Each namespace definition points to a (shared) scope that contains the
// aggregated declarations of all declarations of the same namespace. Note
// that re-opened namespaces are distinct declarations that share the same
// scope. For example:
//
//    naemspace N {
//      int x;
//    } // #1
//
//    namespace N {
//      int y;
//    } // #2
//
// At #1, there is a single namespace declaration named `N`, containing
// only the declaration of `x`. At #2, there are 2 namespace declarations
// of `N`. They share the same scope, which contains the declarations of
// `x` and `y`.
//
// TODO: Every namespace has an anonymous namespace.
//
// TODO: Handle using directives.
struct Namespace_decl : Decl
{
  Namespace_decl(Name&);
  Namespace_decl(Decl&, Name&);

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  bool is_global() const    { return cxt == nullptr; }
  bool is_anonymous() const { return is<Placeholder_id>(id); }

  // Returns a list of members in this namespace.
  Decl_list const& members() const { return decls; }
  Decl_list&       members()       { return decls; }

  // Returns the totoal set of declarations within the namespace.
  Scope const* scope() const { return lookup; }
  Scope*       scope()       { return lookup; }

  Decl_list decls;
  Scope*    lookup;
};


// Declares a template.
//
// A template has a single constraint expression corresponding
// to a requires clause. Note that this is transformed into
// a logical proposition for the purpose of constraint checking
// and comparison.
//
// TODO: Consider making a template parameter list a special
// term. We can linke template parameter lists and their
// constraints. Of course, this may not be necessary.
struct Template_decl : Decl
{
  Template_decl(Decl_list const& p, Decl& d)
    : Decl(d.name()), parms(p), cons(nullptr), decl(&d)
  {
    lingo_assert(!d.context());
    d.context(*this);
  }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the template parameters of the declaration.
  Decl_list const& parameters() const { return parms; }
  Decl_list&       parameters()       { return parms; }

  // Returns the constraint associated with the template.
  // This is valid iff is_constrained() is true.
  Expr const& constraint() const  { return *cons; }
  Expr&       constraint()        { return *cons; }

  // Set the template constraints.
  //
  // TODO: Is this used anywhere?
  void        constrain(Expr& e)  { cons = &e; }

  // Returns true if the template declaration has constraints.
  bool is_constrained() const { return cons; }

  // Returns the underlying pattern.
  Decl const& parameterized_declaration() const { return *decl; }
  Decl&       parameterized_declaration()       { return *decl; }

  Decl_list parms;
  Expr*     cons;
  Decl*     decl;
};


// Represents a concept definition.
//
// TODO: How do I want to handle syntactic requirements? Make a
// Concept_def that is either an expression or a body.
struct Concept_decl : Decl
{
  Concept_decl(Name& n, Decl_list const& ps, Expr& e)
    : Decl(n), parms(ps), def(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the template parameters of the declaration.
  Decl_list const& parameters() const { return parms; }
  Decl_list&       parameters()       { return parms; }

  // Returns the constraint associated with the template.
  // This is valid iff is_constrained() is true.
  Expr const& definition() const  { return *def; }
  Expr&       definition()        { return *def; }

  Decl_list parms;
  Expr*     def;
};

// An object paramter of a function.
//
// TODO: Name this variable_parm to be consistent with variable
// declarations?
struct Object_parm : Object_decl
{
  Object_parm(Name& n, Type& t)
    : Object_decl(n, t)
  { }

  Object_parm(Name& n, Type& t, Expr& i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Expr const& default_argument() const { return *init; }
  Expr&       default_argument()       { return *init; }

  bool has_default_arguement() const { return init; }
};


// A constant value parameter of a template.
//
// TODO: Name this Constant_parm to be consistent with
// constant declarations?
struct Value_parm : Object_decl
{
  Value_parm(Name& n, Type& t)
    : Object_decl(n, t)
  { }

  Value_parm(Name& n, Type& t, Expr& i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Expr const& default_argument() const { return *init; }
  Expr&       default_argument()       { return *init; }

  bool has_default_arguement() const { return init; }
};


// Represents an unspecified sequence of arguments. This
// is distinct from a parameter pack.
//
// Note that we allow the variadic parameter to be named
// although the variadic parameter has a canonical name (...).
//
// TODO: Make this the type of an annamed parameter?
struct Variadic_parm : Decl
{
  Variadic_parm(Name& n)
    : Decl(n)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A type parameter of a template.
struct Type_parm : Decl
{
  Type_parm(Name& n)
    : Decl(n), def()
  { }

  Type_parm(Name& n, Type& t)
    : Decl(n), def(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Type const& default_argument() const { return *def; }
  Type&       default_argument()       { return *def; }

  bool has_default_arguement() const { return def; }

  Type* def;
};


// A template parameter of a template.
//
// The nested effectively denotes the "kind" of the template. It
// must be a template declaration. The name of the parameter and
// that of the underlying declaration must be the same.
struct Template_parm : Decl
{
  Template_parm(Name& n, Decl& t)
    : Decl(n), temp(&t), def()
  { }

  Template_parm(Name& n, Decl& t, Init& i)
    : Decl(n), temp(&t), def(&i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the tempalte declaration that defines the
  // signature of accepted arguments.
  Template_decl const& declaration() const { return *cast<Template_decl>(temp); }
  Template_decl&       declaration()       { return *cast<Template_decl>(temp); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Init const& default_argument() const { return *def; }
  Init&       default_argument()       { return *def; }

  bool has_default_arguement() const { return def; }

  Decl*     temp;
  Init*     def;
};


// A generic visitor for declarations.
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
  void visit(Concept_decl const& d)   { this->invoke(d); }
  void visit(Object_parm const& d)    { this->invoke(d); }
  void visit(Value_parm const& d)     { this->invoke(d); }
  void visit(Type_parm const& d)      { this->invoke(d); }
  void visit(Template_parm const& d)  { this->invoke(d); }
  void visit(Variadic_parm const& d)  { this->invoke(d); }
};


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl const&)>::type>
inline decltype(auto)
apply(Decl const& d, F fn)
{
  Generic_decl_visitor<F, T> vis(fn);
  return accept(d, vis);
}


// A generic visitor for names.
template<typename F, typename T>
struct Generic_decl_mutator : Decl::Mutator, Generic_mutator<F, T>
{
  Generic_decl_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Variable_decl& d)  { this->invoke(d); }
  void visit(Constant_decl& d)  { this->invoke(d); }
  void visit(Function_decl& d)  { this->invoke(d); }
  void visit(Class_decl& d)     { this->invoke(d); }
  void visit(Union_decl& d)     { this->invoke(d); }
  void visit(Enum_decl& d)      { this->invoke(d); }
  void visit(Namespace_decl& d) { this->invoke(d); }
  void visit(Template_decl& d)  { this->invoke(d); }
  void visit(Concept_decl& d)   { this->invoke(d); }
  void visit(Object_parm& d)    { this->invoke(d); }
  void visit(Value_parm& d)     { this->invoke(d); }
  void visit(Type_parm& d)      { this->invoke(d); }
  void visit(Template_parm& d)  { this->invoke(d); }
  void visit(Variadic_parm& d)  { this->invoke(d); }
};


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl&)>::type>
inline decltype(auto)
apply(Decl& d, F fn)
{
  Generic_decl_mutator<F, T> vis(fn);
  return accept(d, vis);
}


// -------------------------------------------------------------------------- //
// Miscellaneous

// TODO: I'm not currently using this, but it might be useful.
struct Translation_unit : Term
{
  Decl_list first;
};


// -------------------------------------------------------------------------- //
// Implementation

// Template_id

inline Template_decl const&
Template_id::declaration() const
{
  return cast<Template_decl>(*decl);
}


inline Template_decl&
Template_id::declaration()
{
  return cast<Template_decl>(*decl);
}


// Class type

inline Class_decl const&
Class_type::declaration() const
{
  return *cast<Class_decl>(decl);
}


inline Class_decl&
Class_type::declaration()
{
  return *cast<Class_decl>(decl);
}

// Union type

inline Union_decl const&
Union_type::declaration() const
{
  return *cast<Union_decl>(decl);
}


inline Union_decl&
Union_type::declaration()
{
  return *cast<Union_decl>(decl);
}

// Enum type

inline Enum_decl const&
Enum_type::declaration() const
{
  return *cast<Enum_decl>(decl);
}


inline Enum_decl&
Enum_type::declaration()
{
  return *cast<Enum_decl>(decl);
}

// Typename type

inline Type_parm const&
Typename_type::declaration() const
{
  return *cast<Type_parm>(decl);
}


inline Type_parm&
Typename_type::declaration()
{
  return *cast<Type_parm>(decl);
}


// -------------------------------------------------------------------------- //
// Queries on types
//
// TODO: For the is_*_type() predicates, should we account
// for qualified types? For example, most rules asking for
// a class type also cover qualified class types also.


// Returns true if `t` is the boolean type.
inline bool
is_boolean_type(Type const& t)
{
  return is<Boolean_type>(&t);
}


// Returns true if `t` is an integer type.
inline bool
is_integer_type(Type const& t)
{
  return is<Integer_type>(&t);
}


// Returns true if `t` is a floating point type.
inline bool
is_floating_point_type(Type const& t)
{
  return is<Float_type>(&t);
}


// Returns true if `t` is a function type.
inline bool
is_function_type(Type const& t)
{
  return is<Function_type>(&t);
}


// Returns true if `t` is a reference type.
inline bool
is_reference_type(Type const& t)
{
  return is<Reference_type>(&t);
}


// Returns true if `t` is a pointer type.
inline bool
is_pointer_type(Type const& t)
{
  return is<Pointer_type>(&t);
}


// Returns true if `t` is an array type.
inline bool
is_array_type(Type const& t)
{
  return is<Array_type>(&t);
}


// Returns true if `t` is a sequence type.
inline bool
is_sequence_type(Type const& t)
{
  return is<Sequence_type>(&t);
}


// Returns true if `t` is a class type.
inline bool
is_class_type(Type const& t)
{
  return is<Class_type>(&t);
}


// Returns true if `t` is a (possibly qualified) class type.
inline bool
is_maybe_qualified_class_type(Type const& t)
{
  return is_class_type(t.unqualified_type());
}


// Returns true if `t` is a union type.
inline bool
is_union_type(Type const& t)
{
  return is<Union_type>(&t);
}


// Returns true if `t` is a (possibly qualified) union type.
inline bool
is_maybe_qualified_union_type(Type const& t)
{
  return is_union_type(t.unqualified_type());
}


// Returns true if `t` is a scalar type.
inline bool
is_scalar_type(Type const& t)
{
  return is_boolean_type(t)
      || is_integer_type(t)
      || is_floating_point_type(t)
      || is_pointer_type(t)
      || is_sequence_type(t);
}


// -------------------------------------------------------------------------- //
// Queries on expressions


// Returns true if the expression `e` has integer type.
inline bool
has_integer_type(Expr const& e)
{
  return is_integer_type(e.type());
}


// Returns true if the expression `e` has floating point type.
inline bool
has_floating_point_type(Expr const& e)
{
  return is_floating_point_type(e.type());
}


// Returns true if `e` has reference type.
inline bool
has_reference_type(Expr const& e)
{
  return is_reference_type(e.type());
}


// Returns trhe if `e` has array type.
inline bool
has_array_type(Expr const& e)
{
  return is_array_type(e.type());
}


// Returns true if `e` has class type.
inline bool
has_class_type(Expr const& e)
{
  return is_class_type(e.type());
}



// Returns true if `e` has union type.
inline bool
has_union_type(Expr const& e)
{
  return is_union_type(e.type());
}


// -------------------------------------------------------------------------- //
// Queries on conversions

// Returns true if the expression is a standard conversion.
inline bool
is_standard_conversion(Expr const& e)
{
  return is<Standard_conv>(&e);
}


// Returns true if the expression is an ellipsis conversion.
inline bool
is_ellipsis_conversion(Expr const& e)
{
  return is<Ellipsis_conv>(&e);
}


} // namespace banjo


#endif
