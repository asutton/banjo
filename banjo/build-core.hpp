// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILD_CORE_HPP
#define BANJO_BUILD_CORE_HPP

// Provides types and functions for the builder components.

#include "token.hpp"
#include "language.hpp"

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
  template<typename T>
  struct Hash
  {
    std::size_t operator()(T const& t) const
    {
      return hash_value(t);
    }
  };

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
  using Unique_factory = Hashed_unique_factory<T, Hash<T>, Eq<T>>;


  Builder_base(Context& cxt)
    : cxt(cxt)
  { }

  Context& cxt;
};


} // namespace banjo


#endif