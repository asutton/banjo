// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-type.hpp"
#include "ast-decl.hpp"


namespace banjo
{

Name const&
Declared_type::name() const
{
  return declaration().name();
}


Name&
Declared_type::name()
{
  return declaration().name();
}


Type_decl const&
Declared_type::declaration() const
{
  return cast<Type_decl>(*decl_);
}


Type_decl&
Declared_type::declaration()
{
  return cast<Type_decl>(*decl_);
}


Class_decl const&
Class_type::declaration() const
{
  return cast<Class_decl>(*decl_);
}


Class_decl&
Class_type::declaration()
{
  return cast<Class_decl>(*decl_);
}



// -------------------------------------------------------------------------- //
// Dependent types

inline bool
any_dependent_types(Type_list const& ts)
{
  return std::any_of(ts.begin(), ts.end(), is_dependent_type);
}


inline bool
is_dependent_function_type(Function_type const& t)
{
  return any_dependent_types(t.parameter_types())
      || is_dependent_type(t.return_type());
}


// Returns true if `t` is dependent.
//
// TODO: This implementation is not yet complete. It doesn't handle,
// e.g., dependent template specializations, dependent members, etc.
bool
is_dependent_type(Type const& t)
{
  struct fn
  {
    bool operator()(Type const& t)          { return false; }
    bool operator()(Function_type const& t) { return is_dependent_function_type(t); }
    bool operator()(Array_type const& t)    { return is_dependent_type(t.element_type()); }
    bool operator()(Tuple_type const& t)    { return any_dependent_types(t.element_types()); }
    bool operator()(Pointer_type const& t)  { return is_dependent_type(t.type()); }
    bool operator()(Auto_type const& t)     { return true; }
  };
  return apply(t, fn{});
}


} // namepace banjo
