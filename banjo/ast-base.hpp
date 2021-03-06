// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_BASE_HPP
#define BANJO_AST_BASE_HPP

// This module contains declarations of the all AST nodes and various
// supporting structures.

#include "prelude.hpp"
#include "value.hpp"

#include <lingo/integer.hpp>
#include <lingo/real.hpp>
#include <lingo/token.hpp>

#include <forward_list>
#include <vector>
#include <utility>


namespace banjo
{

// Bring more lingo types into scope.
using lingo::Integer;
using lingo::Real;
using lingo::Token;
using lingo::Token_seq;


// Primary structures
struct Term;

struct Name;
struct Type;
struct Declared_type;

struct Expr;
struct Id_expr;
struct Access_expr;
struct Unary_expr;
struct Binary_expr;
struct Conv;
struct Init;

struct Req;
struct Stmt;
struct Decl;
struct Typed_decl;
struct Mapping_decl;
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


// Defines the interface (and default method) for allocating memory.
// This structure returns storage for object initialization. Note that not
// all allocators are required to deallocate storage. 
//
// Allocation and deallocation routines accept typeinfo objects. This
// allows the allocator to group objects of like type or support debugging.
struct Allocator
{
  virtual void* allocate(std::size_t n, std::type_info const& ti)
  {
    return ::operator new(n);
  }
  
  virtual void deallocate(void* p, std::type_info const& ti)
  {
    ::operator delete(p);
  }
};




// A simple data-retaining allocator that releases all memory when it goes 
// out of scope. Explicit deallocation has no behavior.
//
// FIXME: This is *not* a good allocator. Replace it with a better-defined
// block allocator.
struct List_allocator : Allocator
{
  struct Block
  {
    explicit Block(std::size_t n) : buf( ::operator new(n)) { }
    ~Block() { ::operator delete(buf); }

    void* buf;
  };

  void* allocate(std::size_t n, std::type_info const& ti)
  {
    list.emplace_front(n);
    return list.front().buf;
  }
  
  void deallocate(void* p, std::type_info const& ti)
  {
    // Never explicitly deallocate memory.
  }

  std::forward_list<Block> list;
};



// A CRTP class that provides overloads of the allocation operators.
// In particular, this provides overloads of placement allocators which
// use the Allocator interface. This allows for "striped" forms of
// allocation, where objects of different dynamic type are pooled into
// different resources.
//
// Note that T must be derived from Term.
//
// TODO: Make allocator tracking optional.
template<typename T>
struct Allocatable
{
  template<typename... Args>
  static T& make(Allocator& a, Args&&... args)
  {
    void* p = a.allocate(sizeof(T), typeid(T));
    T* obj = new (p) T(std::forward<Args>(args)...);
    obj->alloc_ = &a;
    return *obj;
  }

  // Return memory to the arena. Behavior is undefined if the object
  // is not allocated from this arena.
  void unmake(Allocator& a)
  {
    T* obj = static_cast<T*>(this);
    lingo_assert(obj->alloc_ == & a);
    obj->~T();
    a.deallocate(obj, typeid(T));
  }
};


// -------------------------------------------------------------------------- //
// Terms

// The base class of all terms in the language.
//
// Each term has an associated source code location. However, this
// is not meaningful for all terms. In particular, canonicalized
// terms must not include a valid source code location.
//
// Each term is also pinned to the region of memory from which it is allocated 
// (assuming that the object is so-allocated). Note, however, that deallocation 
// is not supported. This is because some terms may be shared (e.g., as a 
// result of canonicalization), meaning that the ownership model is runtime
// dependent. In general, one should instantiate new allocators for specific
// subtasks, and the clone only the results into the "main" memory arena.
struct Term
{
  Term()
    : alloc_(nullptr), loc_()
  { }

  virtual ~Term() { }

  // Returns the source code location of the term. this
  // may be an invalid position.
  Location location() const { return loc_; }

  // Returns the region of text over which the term is written.
  // By default, this is is the empty region, which starts
  // and ends at the term's location.
  virtual Region region() const { return {loc_, loc_}; }

  Allocator* alloc_;
  Location loc_;
};


} // namespace banjo


namespace banjo
{

// -------------------------------------------------------------------------- //
// Lists


// FIXME: Make this a random access iterator.
template<typename T>
struct List_iterator
{
  using Iter              = T*;
  using value_type        = T;
  using reference         = T&;
  using pointer           = T*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  List_iterator() = default;

  List_iterator(T** i)
    : iter(i)
  { }

  reference operator*() const { return **iter; }
  pointer  operator->() const { return *iter; }

  List_iterator& operator++()    { ++iter; return *this; }
  List_iterator  operator++(int) { List_iterator x = *this; ++iter; return x; }

  bool operator==(List_iterator i) const { return iter == i.iter; }
  bool operator!=(List_iterator i) const { return iter != i.iter; }

  T** iter;
};


template<typename T>
struct List_iterator<T const>
{
  using Iter              = T const*;
  using value_type        = T;
  using reference         = T const&;
  using pointer           = T const*;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

  List_iterator() = default;

  List_iterator(T* const* i)
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

  T* const* iter;
};


// Represents a sequence of terms. Note that a list is not, itself, a term.
// It has no semantics beyond containing other terms. Lists are often
// short-lived, copied, and moved.
template<typename T>
struct List : std::vector<T*>
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

  bool is_empty() const { return base().empty(); }
  
  std::size_t size() const { return base().size(); }

  T const& operator[](std::size_t n) const { return *base()[n]; }
  T&       operator[](std::size_t n)       { return *base()[n]; }

  T const& front() const { return *base().front(); }
  T&       front()       { return *base().front(); }

  T const& back() const { return *base().back(); }
  T&       back()       { return *base().back(); }

  void push_back(T& x) { base().push_back(&x); }
  void push_back(T* x) { base().push_back(x); }

  template<typename I>
  void append(I, I);

  iterator begin() { return base().data(); }
  iterator end()   { return base().data() + size(); }

  const_iterator begin() const { return base().data(); }
  const_iterator end() const   { return base().data() + size(); }
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


// -------------------------------------------------------------------------- //
// Ranges

// An iterator range over a contiguous sequence of terms.
//
// TOOD: This seems useful, but I'm not currently using it anywhere. Also,
// it needs to a corresponding const bit to make it work everywhere.
template<typename T>
struct Range
{
  using iterator = List_iterator<T>;
  using const_iterator = List_iterator<T const>;

  Range()
    : first(), last()
  { }

  Range(T** f, T** l)
    : first(f), last(l)
  { }

  iterator begin() { return first; }
  iterator end()   { return last; }

  const_iterator begin() const { return first; }
  const_iterator end() const   { return last; }

  T* first;
  T* last;
};


// ---------------------------------------------------------------------------//
// Utility types

// Lists
using Term_list = List<Term>;
using Type_list = List<Type>;
using Expr_list = List<Expr>;
using Stmt_list = List<Stmt>;
using Decl_list = List<Decl>;
using Req_list = List<Decl>;


// Ranges
using Term_range = Range<Term>;
using Type_range = Range<Type>;
using Expr_range = Range<Expr>;
using Stmt_range = Range<Stmt>;
using Decl_range = Range<Decl>;


// Iterators
using Term_iter = Term_list::iterator;
using Type_iter = Type_list::iterator;
using Expr_iter = Expr_list::iterator;
using Decl_iter = Decl_list::iterator;


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


// Intrinsic function types.
using Nullary_fn = Expr& (*)(Context&);
using Unary_fn   = Value (*)(Context&, Value const&);
using Binary_fn  = Value (*)(Context&, Value const&, Value const&);
using Ternary_fn = Value (*)(Context&, Value const&, Value const&, Value const&);


// Forward declarations of enumerations.
enum Operator_kind : int;
enum Qualifier_set : int;
enum Reference_kind : int;
enum Specifier_set : int;

} // namespace banjo

#endif
