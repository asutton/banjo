// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_HASH_HPP
#define BANJO_HASH_HPP

// This module defines the hash function on AST nodes.
//
// TODO: Migrate this to use an iterative hash instead of Boost's hashing
// library. It will generate better keys (although this is a big rewrite).

#include "ast.hpp"
#include "equivalence.hpp"


namespace banjo
{

// Implements the FNV-1a hashing algorithm as a function object
// with a queryable hash state.
struct fnv1a_hash
{
  // Hash a k of n bytes into the statue.
  void operator()(void const* k, std::size_t n) noexcept
  {
    unsigned char const* p = reinterpret_cast<unsigned char const*>(k);
    unsigned char const* const e = p + n;
    for (; p < e; ++p)
      state_ = (state_ ^ *p) * 1099511628211u;
  }

  // Returns the cumulative state of the hash.
  std::size_t state() const
  {
    return state_;
  }

  std::size_t state_ = 14695981039346656037u;
};


// ---------------------------------------------------------------------------//
// Hashing primitives

// Hash integer values.
template<typename A, typename T>
inline typename std::enable_if<std::is_integral<T>::value, void>::type
hash_append(A& h, T n)
{
  h(&n, sizeof(T));
}


// Hash floating point values. This adjusts the argument so that -0 will
// not hash differently than 0.
template<typename A, typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, void>::type
hash_append(A& h, T n)
{
  if (n == 0) 
    n = 0;
  h(&n, sizeof(T));
}


// Hash enumeration values.
template<typename A, typename T>
inline typename std::enable_if<std::is_enum<T>::value, void>::type
hash_append(A& h, T n)
{
  using U = typename std::underlying_type<T>::type;
  hash_append(h, static_cast<U>(n));
}


// Hash pointer values.
template<typename A, typename T>
inline void
hash_append(A& h, T const* p)
{
  hash_append(h, reinterpret_cast<std::uintptr_t>(p));
}


// Hash type_info objects.
template<typename A>
inline void
hash_append(A& h, std::type_info const& ti)
{
  hash_append(h, ti.hash_code());
}


// Hash an LLVM hash code by extracting its underlying value.
template<typename A>
inline void
hash_append(A& h, llvm::hash_code const& hc)
{
  hash_append(h, (std::size_t)hc);
}


// Hash a lingo integer value.
template<typename A>
void
hash_append(A& h, Integer const& n)
{
  hash_append(h, llvm::hash_value(n.impl()));
}


// Hash a lingo real value.
template<typename A>
void
hash_append(A& h, Real const& n)
{
  hash_append(h, llvm::hash_value(n.impl()));
}


// Hash a list of objects.
template<typename A, typename T>
void
hash_append(A& h, List<T> const& list)
{
  for (T const& t : list)
    hash_append(h, t);
  hash_append(h, list.size());
}


// ---------------------------------------------------------------------------//
// Hashing names

template<typename A>
inline void
hash_simple_id(A& h, Simple_id const& n)
{
  hash_append(h, &n.symbol());
}


template<typename A>
inline void
hash_placeholder_id(A& h, Placeholder_id const& n)
{
  hash_append(h, n.number());
}


template<typename A>
inline void
hash_operator_id(A& h, Operator_id const& n)
{
  hash_append(h, n.kind());
}


template<typename A>
void
hash_append(A& h, Name const& n)
{
  struct fn
  {
    A& h;
    void operator()(Name const& n)           { lingo_unhandled(n); }
    void operator()(Simple_id const& n)      { return hash_simple_id(h, n); }
    void operator()(Global_id const& n)      { }
    void operator()(Placeholder_id const& n) { return hash_placeholder_id(h, n); }
    void operator()(Operator_id const& n)    { return hash_operator_id(h, n); }
  };
  hash_append(h, typeid(n));
  apply(n, fn{h});
}



// ---------------------------------------------------------------------------//
// Hashing types

template<typename A>
inline void
hash_integer_type(A& h, Integer_type const& t)
{
  hash_append(h, t.precision());
  hash_append(h, t.sign());
}


template<typename A>
inline void
hash_float_type(A& h, Float_type const& t)
{
  hash_append(h, t.precision());
}


template<typename A>
inline void
hash_function_type(A& h, Function_type const& t)
{
  hash_append(h, t.parameter_types());
  hash_append(h, t.return_type());
}


template<typename A>
inline void
hash_array_type(A& h, Array_type const& t)
{
  hash_append(h, t.element_type());
  hash_append(h, t.extent());
}


template<typename A>
inline void
hash_tuple_type(A& h, Tuple_type const& t)
{
  hash_append(h, t.element_types());
}


template<typename A>
inline void
hash_pointer_type(A& h, Pointer_type const& t)
{
  hash_append(h, t.type());
}


template<typename A>
inline void
hash_declared_type(A& h, Declared_type const& t)
{
  hash_append(h, t.declaration());
}


// Hash a type. 
template<typename A>
void
hash_append(A& h, Type const& t)
{
  struct fn
  {
    A& h;
    void operator()(Type const& t)           { lingo_unhandled(t); }
    void operator()(Void_type const& t)      { }
    void operator()(Boolean_type const& t)   { }
    void operator()(Byte_type const& t)      { }
    void operator()(Integer_type const& t)   { hash_integer_type(h, t); }
    void operator()(Float_type const& t)     { hash_float_type(h, t); }
    void operator()(Function_type const& t)  { hash_function_type(h, t); }
    void operator()(Array_type const& t)     { hash_array_type(h, t); }
    void operator()(Tuple_type const& t)     { hash_tuple_type(h, t); }
    void operator()(Pointer_type const& t)   { hash_pointer_type(h, t); }
    void operator()(Declared_type const& t)  { hash_declared_type(h, t); }
  };
  hash_append(h, typeid(t));
  hash_append(h, t.category());
  hash_append(h, t.qualifiers());
  apply(t, fn{h});
}


// ---------------------------------------------------------------------------//
// Hashing expression

template<typename A>
void
hash_binary_expr(A& h, Binary_expr const& e)
{
  hash_append(h, e.left());
  hash_append(h, e.right());
}


template<typename A>
void
hash_id_expr(A& h, Decl_ref const& e)
{
  hash_append(h, e.declaration());
}


template<typename A>
void
hash_mem_expr(A& h, Scoped_ref const& e)
{
  hash_append(h, e.declaration());
}


template<typename A>
void
hash_append(A& h, Expr const& e)
{
  struct fn
  {
    A& h;
    void operator()(Expr const& e)         { lingo_unhandled(e); }
    void operator()(Void_expr const& e)    { /* Do nothing. */ }
    void operator()(Boolean_expr const& e) { hash_append(h, e.value()); }
    void operator()(Integer_expr const& e) { hash_append(h, e.value()); }
    void operator()(Tuple_expr const& e)   { hash_append(h, e.elements()); }
    void operator()(Decl_ref const& e)     { hash_id_expr(h, e); }
    void operator()(Scoped_ref const& e)   { hash_mem_expr(h, e); }
    void operator()(Unary_expr const& e)   { hash_append(h, e.operand()); }
    void operator()(Binary_expr const& e)  { hash_binary_expr(h, e); }
  };
  hash_append(h, typeid(e));
  apply(e, fn{h});
}


// ---------------------------------------------------------------------------//
// Hashing declarations

// Declarations are unique, so just hash on its identity.
template<typename A>
void
hash_append(A& h, Decl const& d)
{
  hash_append(h, &d);
}


// ---------------------------------------------------------------------------//
// Universal hash function

// Returns the result type or state of a hash algorithm.
template<typename A>
using Hash_state = decltype(std::declval<A>().state());

// The universal hash function. This applies a hash algorithm A to an
// object of any type T, returning the hashed state.
template<typename A>
struct hash
{
  template<typename  T>
  Hash_state<A> operator()(T const& t) const noexcept
  {
      A h;
      hash_append(h, t);
      return h.state();
  }
};


} // namespace banjo


#endif
