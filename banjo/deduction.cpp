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

void
deduce_from_type(Reference_type& p, Type& a, Substitution& sub)
{
  if (Reference_type* t = as<Reference_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
}


void
deduce_from_type(Qualified_type& p, Type& a, Substitution& sub)
{
  if (Qualified_type* t = as<Qualified_type>(&a)) {
    if (p.qualifier() == t->qualifier())
      return deduce_from_type(p.type(), t->type(), sub);
  }
}


void
deduce_from_type(Pointer_type& p, Type& a, Substitution& sub)
{
  if (Pointer_type* t = as<Pointer_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
}


// Deduce arguments when both types have the form T[].
//
// Note that this form of deduction is not available in C++ since
// arrays decay to pointers.
void
deduce_from_type(Sequence_type& p, Type& a, Substitution& sub)
{
  if (Sequence_type* t = as<Sequence_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
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
// FIXME: This is fundamentally broken as we are essentially deducing
// a mapping for every template parameter. In general, we want the
// substitution to be seeded with parameters that must be deduced and
// ignore the ones that are not.
void
deduce_from_type(Typename_type& p, Type& a, Substitution& sub)
{
  Decl& d = p.declaration();

  // Check for multiple deductions against `a`.
  //
  // FIXME: Throw a better expression.
  if (Type* t = as<Type>(sub.get(d)))
    if (!is_equivalent(a, *t))
      throw std::runtime_error("deduction error");

  sub.send(d, a);
}


// Find a substitution from template parameters in `p` to template
// arguments in `a`.
//
// TODO: This is clearly incomplete. Finish implementing template
// argument deduction.
//
// TODO: For any templated type, we need to perform unification
// against their respective ids.
void
deduce_from_type(Type& p, Type& a, Substitution& sub)
{
  struct fn
  {
    Type& a;
    Substitution& sub;
    void operator()(Type& p)           { }
    void operator()(Auto_type& p)      { lingo_unimplemented(); }
    void operator()(Decltype_type& p)  { lingo_unimplemented(); }
    void operator()(Declauto_type& p)  { lingo_unimplemented(); }
    void operator()(Function_type& p)  { lingo_unimplemented(); }
    void operator()(Reference_type& p) { deduce_from_type(p, a, sub); }
    void operator()(Qualified_type& p) { deduce_from_type(p, a, sub); }
    void operator()(Pointer_type& p)   { deduce_from_type(p, a, sub); }
    void operator()(Array_type& p)     { lingo_unimplemented(); }
    void operator()(Sequence_type& p)  { deduce_from_type(p, a, sub); }
    void operator()(Typename_type& p)  { deduce_from_type(p, a, sub); }
  };
  apply(p, fn{a, sub});
}



// -------------------------------------------------------------------------- //
// Deducing template arguments during partial ordering




} // namespace banjo
