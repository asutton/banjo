// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_BASE_HPP
#define BANJO_AST_BASE_HPP

// This module contains declarations of the all AST nodes and various
// supporting structures.

#include "prelude.hpp"

#include <lingo/integer.hpp>
#include <lingo/real.hpp>

#include <vector>
#include <utility>


namespace banjo
{

struct Term;

struct Name;
struct Simple_id;
struct Global_id;
struct Placeholder_id;
struct Operator_id;
struct Conversion_id;
struct Literal_id;
struct Destructor_id;
struct Template_id;
struct Concept_id;
struct Qualified_id;

struct Type;
struct Void_type;
struct Boolean_type;
struct Byte_type;
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
struct Synthetic_type;

struct Expr;
struct Unary_expr;
struct Binary_expr;
struct Boolean_expr;
struct Integer_expr;
struct Real_expr;
struct Reference_expr;
struct Check_expr;
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
struct Requires_expr;
struct Synthetic_expr;

struct Conv;
struct Value_conv;
struct Qualification_conv;
struct Boolean_conv;
struct Integer_conv;
struct Float_conv;
struct Numeric_conv;
struct Ellipsis_conv;

struct Init;
struct Trivial_init;
struct Copy_init;
struct Bind_init;
struct Direct_init;
struct Aggregate_init;

struct Req;
struct Type_req;
struct Syntactic_req;
struct Semantic_req;
struct Expression_req;
struct Simple_req;
struct Conversion_req;
struct Deduction_req;
struct Existential_req;

struct Stmt;
struct Compound_stmt;
struct Expression_stmt;
struct Declaration_stmt;
struct Return_stmt;

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
struct Axiom_decl;
struct Object_parm;
struct Value_parm;
struct Type_parm;
struct Template_parm;
struct Variadic_parm;

struct Def;
struct Defaulted_def;
struct Deleted_def;
struct Expression_def;
struct Function_def;
struct Class_def;
struct Union_def;
struct Enum_def;
struct Concept_def;

struct Cons;
struct Concept_cons;
struct Expression_cons;
struct Type_cons;
struct Predicate_cons;
struct Conversion_cons;
struct Deduction_cons;
struct Conjunction_cons;
struct Disjunction_cons;
struct Parameterized_cons;

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

  List_iterator() = default;

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

  List_iterator() = default;

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
using Req_list  = List<Req>;
using Stmt_list = List<Stmt>;
using Decl_list = List<Decl>;
using Cons_list = List<Cons>;


// Iterators
using Term_iter = Term_list::iterator;
using Type_iter = Type_list::iterator;
using Expr_iter = Expr_list::iterator;
using Decl_iter = Decl_list::iterator;
using Cons_iter = Cons_list::iterator;


// Pairs and tuples
using Expr_pair = std::pair<Expr&, Expr&>;


} // namesapce banjo

#endif
