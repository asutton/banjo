// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "deduction.hpp"
#include "template.hpp"
#include "equivalence.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Deducing template arguments from a type

bool
deduce_from_type(Reference_type& p, Type& a, Substitution& sub)
{
  if (Reference_type* t = as<Reference_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
  return false;
}


bool
deduce_from_type(Qualified_type& p, Type& a, Substitution& sub)
{
  if (Qualified_type* t = as<Qualified_type>(&a)) {
    if (p.qualifier() == t->qualifier())
      return deduce_from_type(p.type(), t->type(), sub);
  }
  return false;
}


bool
deduce_from_type(Pointer_type& p, Type& a, Substitution& sub)
{
  if (Pointer_type* t = as<Pointer_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
  return false;
}


// Deduce arguments when both types have the form T[].
//
// Note that this form of deduction is not available in C++ since
// arrays decay to pointers.
bool
deduce_from_type(Sequence_type& p, Type& a, Substitution& sub)
{
  if (Sequence_type* t = as<Sequence_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
  return false;
}


// Deduce an assignment of a type parrameter to a corresponding
// argument type. For example, given
//
//    p: T
//    a: int const
//
// This will deduce the substitution of T -> int const. If a prior
// and different assignment was deduced for T, template argument
// deduction fails.
//
// FIXME: Save information to better indicate the reason for
// failure.
bool
deduce_from_type(Typename_type& p, Type& a, Substitution& sub)
{
  Decl& d = p.declaration();
  if (sub.has_mapping(d)) {
    if (Type* t = as<Type>(sub.get_mapping(d))) {
      if (!is_equivalent(a, *t))
        return false;
    } else {
      sub.map_to(d, a);
    }
    return true;
  }
  return false;
}


// Find a substitution from template parameters in `p` to template
// arguments in `a`.
//
// TODO: This is clearly incomplete. Finish implementing template
// argument deduction.
//
// TODO: For any templated type, we need to perform unification
// against their respective ids.
bool
deduce_from_type(Type& p, Type& a, Substitution& sub)
{
  struct fn
  {
    Type& a;
    Substitution& sub;

    // We can't deduce from most type patterns.
    bool operator()(Type& p)           { return true; }

    bool operator()(Auto_type& p)      { lingo_unimplemented(); }
    bool operator()(Decltype_type& p)  { lingo_unimplemented(); }
    bool operator()(Declauto_type& p)  { lingo_unimplemented(); }
    bool operator()(Function_type& p)  { lingo_unimplemented(); }
    bool operator()(Reference_type& p) { return deduce_from_type(p, a, sub); }
    bool operator()(Qualified_type& p) { return deduce_from_type(p, a, sub); }
    bool operator()(Pointer_type& p)   { return deduce_from_type(p, a, sub); }
    bool operator()(Array_type& p)     { lingo_unimplemented(); }
    bool operator()(Sequence_type& p)  { return deduce_from_type(p, a, sub); }
    bool operator()(Typename_type& p)  { return deduce_from_type(p, a, sub); }
  };
  return apply(p, fn{a, sub});
}


// Deduce a template arguments from a list of parameters and arguments.
// This succeeds only when deduction succeds for each parameter and
// argument in the corresponding lists.
//
// Here, ps is considered a function parameter list and as a
// function argument list.
//
// TODO: Correctly handle deductions against packs, etc.
bool
deduce_from_types(Type_list& ps, Type_list& as, Substitution& sub)
{
  std::size_t i = 0;
  while (i < ps.size() && i < as.size()) {
    Type& p = *ps[i];
    Type& a = *as[i];
    if (!deduce_from_type(p, a, sub))
      return false;
    ++i;
  }

  // FIXME: We should probably never get here. Or maybe we should?
  if (i != ps.size() || i != as.size())
    lingo_unreachable();

  return true;
}


} // namespace banjo
