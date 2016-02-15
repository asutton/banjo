// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast_type.hpp"
#include "ast_decl.hpp"


namespace banjo
{

Name const&
User_defined_type::name() const
{
  return declaration().name();
}


Name&
User_defined_type::name()
{
  return declaration().name();
}


Class_decl const&
Class_type::declaration() const
{
  return *cast<Class_decl>(decl);
}


Class_decl&
Class_type::declaration()
{
  return *cast<Class_decl>(decl);
}


Union_decl const&
Union_type::declaration() const
{
  return *cast<Union_decl>(decl);
}


Union_decl&
Union_type::declaration()
{
  return *cast<Union_decl>(decl);
}


Enum_decl const&
Enum_type::declaration() const
{
  return *cast<Enum_decl>(decl);
}


Enum_decl&
Enum_type::declaration()
{
  return *cast<Enum_decl>(decl);
}


Type_parm const&
Typename_type::declaration() const
{
  return *cast<Type_parm>(decl);
}


Type_parm&
Typename_type::declaration()
{
  return *cast<Type_parm>(decl);
}


// -------------------------------------------------------------------------- //
// Dependent types

inline bool
any_dependent_type(Type_list const& ts)
{
  return std::any_of(ts.begin(), ts.end(), is_dependent_type);
}


inline bool
is_dependent_function_type(Function_type const& t)
{
  return any_dependent_type(t.parameter_types())
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
    bool operator()(Type const& t)           { return false; }
    bool operator()(Function_type const& t)  { return is_dependent_function_type(t); }
    bool operator()(Qualified_type const& t) { return is_dependent_type(t.type()); }
    bool operator()(Reference_type const& t) { return is_dependent_type(t.type()); }
    bool operator()(Pointer_type const& t)   { return is_dependent_type(t.type()); }
    bool operator()(Array_type const& t)     { return is_dependent_type(t.type()); }
    bool operator()(Sequence_type const& t)  { return is_dependent_type(t.type()); }
    bool operator()(Typename_type const& t)  { return true; }
  };
  return apply(t, fn{});
}


} // namepace banjo
