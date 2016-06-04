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
template<typename A, typename T>
inline void
hash_append(A& h, std::type_info const& ti)
{
  hash_append(h, ti.hash_code());
}


// ---------------------------------------------------------------------------//
// Hashing types


// Hash a basic type. These are canonicalized, so we hash their on their
// identity. Note that this also means that we don't have to hash their
// type codes either.
template<typename A>
void
hash_append(A& h, Basic_type const& t)
{
  hash_append(h, &t);
}


// Hash a type. 
template<typename A>
void
hash_append(A& h, Type t)
{
  hash_append(h, t.category());
  hash_append(h, t.qualifiers());
  hash_append(h, t.basis());
}



template<typename A>
void
hash_append(A& h, Type_list const& ts)
{
  for (Type t : ts)
    hash_append(h, t);
  hash_append(h, ts.size());
}


// ---------------------------------------------------------------------------//
// Hashing names

template<typename A>
inline void
hash_simple_id(A& h, Simple_id const& n)
{
  hash_append(h, typeid(n));
  hash_append(h, &n.symbol());
}


template<typename A>
inline void
hash_global_id(A& h, Global_id const& n)
{
  hash_append(h, typeid(n));
}


template<typename A>
inline void
hash_placeholder_id(A& h, Placeholder_id const& n)
{
  hash_append(h, typeid(n));
  hash_append(h, n.number());
}


template<typename A>
inline void
hash_operator_id(A& h, Operator_id const& n)
{
  hash_append(h, typeid(n));
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
    void operator()(Global_id const& n)      { return hash_global_id(h, n); }
    void operator()(Placeholder_id const& n) { return hash_placeholder_id(h, n); }
    void operator()(Operator_id const& n)    { return hash_operator_id(h, n); }
  };
  apply(n, fn{});
}


// ---------------------------------------------------------------------------//
// Miscellaneous

template<typename A, typename T>
void
hash_append(A& h, List<T> const& list)
{
  for (T const& t : list)
    hash_append(h, t);
  hash_append(list.size());
}


// std::size_t hash_append(Term const&);
// std::size_t hash_append(Expr const&);
// std::size_t hash_append(Decl const&);
// std::size_t hash_append(Cons const&);




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



// template<typename T>
// struct Term_hash
// {
//   bool operator()(T const* t) const
//   {
//     return hash_value(*t);
//   }
// };


// using Name_hash = Term_hash<Name>;
// using Type_hash = Term_hash<Type>;
// using Expr_hash = Term_hash<Expr>;
// using Cons_hash = Term_hash<Cons>;


} // namespace banjo


#endif
