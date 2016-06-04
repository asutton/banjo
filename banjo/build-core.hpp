// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILD_CORE_HPP
#define BANJO_BUILD_CORE_HPP

// Provides types and functions for the builder components. Note that
// this header pulls the entire type system.
//
// TODO: Build a simple bump allocator that all factories backend into.
// The underlying region of memory should be controlled by a global function,
// allowing short-lived caches of objects.

#include "token.hpp"
#include "hashing.hpp"

#include <forward_list>
#include <unordered_set>


namespace banjo
{

struct Context;


// The singleton factory maintains a single instance of an object.
template<typename T>
struct Singleton_factory
{
  Singleton_factory()
    : obj_()
  { }

  ~Singleton_factory()
  {
    delete obj_;
  }

  template<typename... Args>
  T& operator()(Args&&... args)
  {
    if (!obj_)
      obj_ = new T(std::forward<Args>(args)...);
    return *obj_;
  }

  T* obj_;
};


// A basic object factory allocates new objects by pushing them to a
// list.
//
// TODO: Use a bump allocator instead of a list since we don't really
// need the underlying memory.
template<typename T>
struct Basic_factory : std::forward_list<T>
{
  template<typename... Args>
  T& operator()(Args&&... args) 
  {
    this->emplace_front(std::forward<Args>(args)...);
  }
};


// The hashed unique factory maintains an unordered set of objects
// that are unique based on their equality. 
template<typename T, typename Hash, typename Eq>
struct Hashed_unique_factory : std::unordered_set<T, Hash, Eq>
{
  template<typename... Args>
  T& operator()(Args&&... args)
  {
    auto ins = this->emplace(std::forward<Args>(args)...);
    return *const_cast<T*>(&*ins.first); // Yuck.
  }
};


// The builder base provides common types and functions for derived
// builder classes. Note that each builder class maintains a reference
// to the context. This provides runtime information about compiler
// configuration, and access to other sensitive globals (e.g., the
// symbol table, and diagnostics).
struct Builder_base
{
  // Hash function for terms.
  using Hash = hash<fnv1a_hash>;

  // Equivalence relation on terms.
  template<typename T>
  struct Eq
  {
    bool operator()(T const& a, T const& b) const
    {
      return is_equivalent(a, b);
    }
  };

  // Factory types.
  template<typename T>
  using Single_factory = banjo::Singleton_factory<T>;

  template<typename T>
  using Basic_factory = banjo::Basic_factory<T>;

  template<typename T>
  using Unique_factory = Hashed_unique_factory<T, Hash, Eq<T>>;


  Builder_base(Context& cxt)
    : cxt(cxt)
  { }

  Context& cxt;
};


} // namespace banjo


#endif