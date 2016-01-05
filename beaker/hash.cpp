// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "hash.hpp"
#include "ast.hpp"


namespace beaker
{

std::size_t hash_value(Type const&);
std::size_t hash_value(Void_type const&);
std::size_t hash_value(Boolean_type const&);
std::size_t hash_value(Integer_type const&);
std::size_t hash_value(Float_type const&);
std::size_t hash_value(Auto_type const&);
std::size_t hash_value(Decltype_type const&);
std::size_t hash_value(Declauto_type const&);


// Default hash values for empty types.
enum {
  void_type_hash     = 0,
  bool_type_hash     = 1,
  auto_type_hash     = 2,
  declauto_type_hash = 3
};


// Compute the hash value of a type.
std::size_t
hash_value(Type const& t)
{
  struct fn
  {
    std::size_t operator()(Void_type const* t)      { return hash_value(*t); }
    std::size_t operator()(Boolean_type const* t)   { return hash_value(*t); }
    std::size_t operator()(Integer_type const* t)   { return hash_value(*t); }
    std::size_t operator()(Float_type const* t)     { return hash_value(*t); }
    std::size_t operator()(Auto_type const* t)      { return hash_value(*t); }
    std::size_t operator()(Decltype_type const* t)  { return hash_value(*t); }
    std::size_t operator()(Declauto_type const* t)  { return hash_value(*t); }
    std::size_t operator()(Qualified_type const* t) { lingo_unreachable(); }
    std::size_t operator()(Pointer_type const* t)   { lingo_unreachable(); }
    std::size_t operator()(Reference_type const* t) { lingo_unreachable(); }
    std::size_t operator()(Array_type const* t)     { lingo_unreachable(); }
    std::size_t operator()(Sequence_type const* t)  { lingo_unreachable(); }
    std::size_t operator()(Class_type const* t)     { lingo_unreachable(); }
    std::size_t operator()(Union_type const* t)     { lingo_unreachable(); }
    std::size_t operator()(Enum_type const* t)      { lingo_unreachable(); }
  };

  return apply(&t, fn{});
}


std::size_t
hash_value(Void_type const&)
{
  return void_type_hash;
}

std::size_t
hash_value(Boolean_type const&)
{
  return bool_type_hash;
}

std::size_t
hash_value(Integer_type const& t)
{
  std::size_t seed = 0;
  boost::hash_combine(seed, t.is_signed());
  boost::hash_combine(seed, t.precision());
  return seed;
}

std::size_t
hash_value(Float_type const& t)
{
  return t.precision();
}


std::size_t
hash_value(Auto_type const&)
{
  return auto_type_hash;
}


std::size_t
hash_value(Decltype_type const&)
{
  lingo_unreachable();
}


std::size_t
hash_value(Declauto_type const&)
{
  return declauto_type_hash;
}



} // namespace beaker
