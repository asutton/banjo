// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "deduction.hpp"
#include "equivalence.hpp"

#include <iostream>


namespace banjo
{

void deduce_type(Reference_type&, Type&, Substitution&);
void deduce_type(Qualified_type&, Type&, Substitution&);
void deduce_type(Pointer_type&, Type&, Substitution&);
void deduce_type(Sequence_type&, Type&, Substitution&);
void deduce_type(Typename_type&, Type&, Substitution&);


// Find a substitution from template parameters in `p` to template
// arguments in `a`.
//
// TODO: This is clearly incomplete. Finish implementing template
// argument deduction.
//
// TODO: For any templated type, we need to perform unification
// against their respective ids.
void
deduce(Type& p, Type& a, Substitution& sub)
{
  struct fn
  {
    Type& a;
    Substitution& sub;
    void operator()(Void_type& p)      { }
    void operator()(Boolean_type& p)   { }
    void operator()(Integer_type& p)   { }
    void operator()(Float_type& p)     { }
    void operator()(Auto_type& p)      { lingo_unimplemented(); }
    void operator()(Decltype_type& p)  { lingo_unimplemented(); }
    void operator()(Declauto_type& p)  { lingo_unimplemented(); }
    void operator()(Function_type& p)  { lingo_unimplemented(); }
    void operator()(Reference_type& p) { deduce_type(p, a, sub); }
    void operator()(Qualified_type& p) { deduce_type(p, a, sub); }
    void operator()(Pointer_type& p)   { deduce_type(p, a, sub); }
    void operator()(Array_type& p)     { lingo_unimplemented(); }
    void operator()(Sequence_type& p)  { deduce_type(p, a, sub); }
    void operator()(Class_type& p)     { }
    void operator()(Union_type& p)     { }
    void operator()(Enum_type& p)      { }
    void operator()(Typename_type& p)  { deduce_type(p, a, sub); }
  };

  apply(p, fn{a, sub});
}


void
deduce_type(Reference_type& p, Type& a, Substitution& sub)
{
  if (Reference_type* t = as<Reference_type>(&a))
    return deduce(p.type(), t->type(), sub);
}


void
deduce_type(Qualified_type& p, Type& a, Substitution& sub)
{
  if (Qualified_type* t = as<Qualified_type>(&a)) {
    if (p.qualifier() == t->qualifier())
      return deduce(p.type(), t->type(), sub);
  }
}


void
deduce_type(Pointer_type& p, Type& a, Substitution& sub)
{
  if (Pointer_type* t = as<Pointer_type>(&a))
    return deduce(p.type(), t->type(), sub);
}


// Deduce arguments when both types have the form T[].
//
// Note that this form of deduction is not available in C++ since
// arrays decay to pointers.
void
deduce_type(Sequence_type& p, Type& a, Substitution& sub)
{
  if (Sequence_type* t = as<Sequence_type>(&a))
    return deduce(p.type(), t->type(), sub);
}


void
deduce_type(Typename_type& p, Type& a, Substitution& sub)
{
  Decl& d = p.declaration();

  // Check for multiple deductins against `a`.
  //
  // FIXME: Is this right?
  if (Type* t = as<Type>(sub.get(d)))
    if (!is_equivalent(a, *t))
      throw std::runtime_error("deduction error");

  sub.send(d, a);
}


} // namespace banjo
