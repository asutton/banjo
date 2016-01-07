// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "equivalence.hpp"
#include "ast.hpp"

#include <typeindex>


namespace beaker
{

bool is_equivalent(Type const&, Type const&);
bool is_equivalent(Void_type const&, Void_type const&);
bool is_equivalent(Boolean_type const&, Boolean_type const&);
bool is_equivalent(Integer_type const&, Integer_type const&);
bool is_equivalent(Float_type const&, Float_type const&);
bool is_equivalent(Auto_type const&, Auto_type const&);
bool is_equivalent(Decltype_type const&, Decltype_type const&);
bool is_equivalent(Declauto_type const&, Declauto_type const&);
bool is_equivalent(Function_type const&, Function_type const&);


// Returns true if the types a and b are equivalent.
//
// TODO: Finish implementing this function.
bool
is_equivalent(Type const& a, Type const& b)
{
  struct fn
  {
    Type const& b;
    bool operator()(Void_type const& a) const      { return is_equivalent(a, cast<Void_type>(b)); }
    bool operator()(Boolean_type const& a) const   { return is_equivalent(a, cast<Boolean_type>(b)); }
    bool operator()(Integer_type const& a) const   { return is_equivalent(a, cast<Integer_type>(b)); }
    bool operator()(Float_type const& a) const     { return is_equivalent(a, cast<Float_type>(b)); }
    bool operator()(Auto_type const& a) const      { return is_equivalent(a, cast<Auto_type>(b)); }
    bool operator()(Decltype_type const& a) const  { return is_equivalent(a, cast<Decltype_type>(b)); }
    bool operator()(Declauto_type const& a) const  { return is_equivalent(a, cast<Declauto_type>(b)); }
    bool operator()(Function_type const& a) const  { return is_equivalent(a, cast<Function_type>(b)); }
    bool operator()(Qualified_type const& a) const { lingo_unreachable(); }
    bool operator()(Pointer_type const& a) const   { lingo_unreachable(); }
    bool operator()(Reference_type const& a) const { lingo_unreachable(); }
    bool operator()(Array_type const& a) const     { lingo_unreachable(); }
    bool operator()(Sequence_type const& a) const  { lingo_unreachable(); }
    bool operator()(Class_type const& a) const     { lingo_unreachable(); }
    bool operator()(Union_type const& a) const     { lingo_unreachable(); }
    bool operator()(Enum_type const& a) const      { lingo_unreachable(); }
    bool operator()(Typename_type const& a) const  { lingo_unreachable(); }
  };

  // The same objects represent the same types.
  if (&a == &b)
    return true;

  // Types of different kinds are not the same.
  std::type_index t1 = typeid(a);
  std::type_index t2 = typeid(b);
  if (t1 != t2)
    return false;

  // Find a comparison of the types.
  return apply(a, fn{b});
}


bool
is_equivalent(Void_type const&, Void_type const&)
{
  return true;
}


bool
is_equivalent(Boolean_type const& a, Boolean_type const& b)
{
  return true;
}


bool
is_equivalent(Integer_type const& a, Integer_type const& b)
{
  return a.is_signed() == b.is_signed() && a.precision() == b.precision();
}


bool
is_equivalent(Float_type const& a, Float_type const& b)
{
  return a.precision() == b.precision();
}


bool
is_equivalent(Auto_type const& a, Auto_type const& b)
{
  return true;
}


bool
is_equivalent(Decltype_type const& a, Decltype_type const& b)
{
  lingo_unreachable();
}


bool
is_equivalent(Declauto_type const& a, Declauto_type const& b)
{
  return true;
}


bool
is_equivalent(Function_type const& a, Function_type const& b)
{
  lingo_unreachable();
}


} // namespace beaker
