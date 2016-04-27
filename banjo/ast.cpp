// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast.hpp"


namespace banjo
{


// Returns true if `t` is an object type. That is, any type
// except function types and reference types.
//
// This function is not defined for placeholder types and
// dependent types.
//
// TODO: Is void an object type? Currently, yes.
//
// TODO: Does the type category of a cv-T really depend T? This
// will almost certainly return true. What is this (()->void) const&?
//
// FIXME: Can a dependent type be categorized? We should probably
// assert that as a precondition.
//
// FIXME: The value category of a decltype-type can be determined
// if its expression is not type-dependent.
bool
is_value_type(Type const& t)
{
  struct fn
  {
    bool operator()(Type const& t)           { lingo_unhandled(t); }
    bool operator()(Void_type const&)        { return true; }
    bool operator()(Boolean_type const&)     { return true; }
    bool operator()(Integer_type const&)     { return true; }
    bool operator()(Float_type const&)       { return true; }
    bool operator()(Auto_type const&)        { lingo_unreachable(); }
    bool operator()(Function_type const&)    { return false; }
    bool operator()(Qualified_type const& t) { return is_value_type(t.type()); }
    bool operator()(Pointer_type const&)     { return true; }
    bool operator()(Reference_type const&)   { return false; }
    bool operator()(Array_type const&)       { return true; }
    bool operator()(Tuple_type const&)       { return true; }
    bool operator()(Slice_type const&)       { return true; }
    bool operator()(Dynarray_type const&)    { return true; }
    bool operator()(Coroutine_type const&)   { return false; }
  };
  return apply(t, fn{});
}


} // namespace banjo
