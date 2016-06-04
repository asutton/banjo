// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_BASE_HPP
#define BANJO_AST_BASE_HPP

// This module contains declarations of the all AST nodes and various
// supporting structures.

#include "prelude.hpp"

#include <lingo/integer.hpp>
#include <lingo/real.hpp>
#include <lingo/token.hpp>

#include <vector>
#include <utility>


namespace banjo
{

// Bring some names into scope so they aren't hidden by banjo declarations
// in the type hierarchy.
using lingo::is;
using lingo::as;
using lingo::cast;


// Bring useful structures into scope.
using lingo::Integer;


// Primary structures
struct Term;

struct Name;
struct Type;
struct Declared_type;

struct Expr;
struct Id_expr;
struct Id_decl_expr;
struct Dot_expr;
struct Dot_decl_expr;
struct Unary_expr;
struct Binary_expr;
struct Conv;
struct Init;

struct Req;
struct Stmt;
struct Decl;
struct Object_decl;
struct Value_decl;
struct Type_decl;

struct Def;
struct Cons;


#define define_node(Node) struct Node;
#include "ast-name.def"
#include "ast-type.def"
#include "ast-expr.def"
#include "ast-req.def"
#include "ast-stmt.def"
#include "ast-decl.def"
#include "ast-def.def"
#include "ast-cons.def"
#undef define_node


// Secondary structures.
struct Context;
struct Scope;
struct Overload_set;


// -------------------------------------------------------------------------- //
// Terms

// The base class of all terms in the language.
//
// Each term has an associated source code location. However, this
// is not meaningful for all terms. In particular, canonicalized
// terms must not include a valid source code location.
//
// TODO: Do I really want a base class for all of these things?
struct Term
{
  virtual ~Term() { }

  // Returns the source code location of the term. this
  // may be an invalid position.
  Location location() const { return loc; }

  // Returns the region of text over which the term is written.
  // By default, this is is the empty region, which starts
  // and ends at the term's location.
  virtual Region region() const { return {loc, loc}; }

  Location loc;
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
  List_iterator  operator++(int) { List_iterator x = *this; ++iter; return x; }

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
  List_iterator  operator++(int) { List_iterator x = *this; ++iter; return x; }

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
using Type_list = std::vector<Type>;
using Expr_list = List<Expr>;
using Req_list  = List<Req>;
using Stmt_list = List<Stmt>;
using Decl_list = List<Decl>;
using Cons_list = List<Cons>;
using Term_list = List<Term>;


// Iterators
using Term_iter = Term_list::iterator;
using Type_iter = Type_list::iterator;
using Expr_iter = Expr_list::iterator;
using Decl_iter = Decl_list::iterator;
using Cons_iter = Cons_list::iterator;


// ---------------------------------------------------------------------------//
// Unparsed terms

// An unparsed term is defined by a sequence of tokens. Here, the 
//
// TODO: Would it make sense to make this an iterator range, pointing
// into the underlying token stream? That makes sense if we're not
// going to modify the underlying token buffer.
template<typename T>
struct Unparsed_term : T
{
  Unparsed_term(Token_seq&& toks)
    : toks_(std::move(toks))
  { }

  Token_seq const& tokens() const { return toks_; }
  Token_seq&       tokens()       { return toks_; }

  Token_seq toks_;
};


// ---------------------------------------------------------------------------//
// Miscellaneous types

// Pairs and tuples
using Expr_pair = std::pair<Expr&, Expr&>;


// An index records the depth and offset of a parameter.
struct Index : std::pair<int, int>
{
  using std::pair<int, int>::pair;

  int depth() const  { return first; }
  int offset() const { return second; }
};


// Intrinsic function types.
using Nullary_fn = Expr& (*)(Context&);
using Unary_fn   = Expr& (*)(Context&, Expr&);
using Binary_fn  = Expr& (*)(Context&, Expr&, Expr&);
using Ternary_fn = Expr& (*)(Context&, Expr&, Expr&, Expr&);


// Forward declarations of enumerations.
enum Operator_kind : int;
enum Qualifier_set : int;
enum Reference_kind : int;
enum Specifier_set : int;

} // namespace banjo

#endif
