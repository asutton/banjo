// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "overload.hpp"
#include "context.hpp"
#include "equivalence.hpp"

#include <iostream>


namespace banjo
{

Name const&
Overload_set::name() const
{
  return front().name();
}


Name&
Overload_set::name()
{
  return modify(front().name());
}


#if 0
std::ostream&
operator<<(std::ostream& os, Overload_set const& ovl)
{
  for (Decl const& d : ovl)
    os << d << '\n';
  return os;
}
#endif


// -------------------------------------------------------------------------- //
// Overloadable declarations

// TODO: Diagnose errors at the location of the declaration, and not
// the input location... it's not in the right position anyway.

static inline bool
conflicting_declaration(Context& cxt, Decl& prev, Decl& given)
{
  error(cxt, "declaration of '{}' conflicts with a previous declaration", given.name());
  return false;
}


static inline bool
non_overloadable_declaration(Context& cxt, Decl& prev, Decl& given)
{
  error(cxt, "cannot overload '{}' with a previous declaration", given.name());
  return false;
}


// Function declarations that differ only in the return type
// cannot be overloaded.
bool
can_overload(Context& cxt, Function_decl& prev, Function_decl& given)
{
  Function_type& t1 = prev.type();
  Function_type& t2 = given.type();
  if (is_equivalent(t1.parameter_types(), t2.parameter_types())) {
    if (!is_equivalent(t1.return_type(), t2.return_type())) {
      return non_overloadable_declaration(cxt, prev, given);
    }
  }
  return true;
}


// Returns true if we can categorically overload the given declaration
// with a previous declaration. Only functions and function templates can
// be overloaded.
//
// TODO: This will need to be adjusted when we add member functions.
bool
can_overload(Context& cxt, Decl& prev, Decl& given)
{
  // Only functions and function templates can be overloaded. There
  // are some restrictions on the overloading of functions.
  if (Function_decl* f1 = as<Function_decl>(&prev)) {
    if (Function_decl* f2 = as<Function_decl>(&given))
      return can_overload(cxt, *f1, *f2);
  }

  // Only functions and funtion templates can be overloaded.
  Decl& d1 = prev.parameterized_declaration();
  Decl& d2 = given.parameterized_declaration();
  if (!is_function(d1) || !is_function(d2))
    return conflicting_declaration(cxt, prev, given);

  return true;
}


} // namespace banjo
