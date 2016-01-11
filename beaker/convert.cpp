// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "convert.hpp"
#include "ast.hpp"
#include "equivalence.hpp"
#include "print.hpp"

#include <typeindex>
#include <iostream>


namespace beaker
{

// -------------------------------------------------------------------------- //
// Categorical conversions

// Returns a possibly converted expression. If no conversions
// are possible, this returns the original expression.
//
// An expression with reference type T& can be converted to
// an expression with non-reference type T. Note that this will
// never apply to functions since an object cannot be declared
// with function type.
//
// In C++, this a glvalue to prvalue conversion.
//
// FIXME: Check that e's type is complete before invoking the
// conversion.
Expr&
convert_object_to_value(Expr& e, Type& t)
{
  if (Reference_type* et = as<Reference_type>(&e.type()))
    return *new Value_conv(et->type(), e);
  return e;
}


// Perform at most one categorical conversion. There is currently
// just one that could performed: object-to-value.
Expr&
convert_category(Expr& e, Type& t)
{
  if (!is<Reference_type>(&t))
    return convert_object_to_value(e, t);
  return e;
}


// -------------------------------------------------------------------------- //
// Value conversions

// A value of integer type can be converted to bool.
Expr&
convert_to_bool(Expr& e, Boolean_type& t)
{
  if (is<Integer_type>(&e.type()))
    return *new Boolean_conv(t, e);
  return e;
}


// A value of integer type can be converted to a value of a
// wider integer type.
//
// A value of type bool can be converted to integer s.t.
// false is 0 and true is 1.
//
// FIXME: An int-to-int conversion requires some form of sign
// extension. That depends on the target type. Perhaps use
// different conversions for these values?
//
// Also use a different conversion for bool-to-int?
Expr&
convert_to_wider_integer(Expr& e, Integer_type& t)
{
  // A value of integer type can be converted...
  if (Integer_type* et = as<Integer_type>(&e.type())) {
    if (et->precision() < t.precision())
      return *new Integer_conv(t, e);
    else
      return e;
  }

  // A value of type bool can be converted...
  if (is<Boolean_type>(&e.type()))
    return *new Integer_conv(t, e);

  return e;
}


// A value of float type can be converted to a value of a wider
// float type.
//
// TODO: Implement me.
Expr&
convert_to_wider_float(Expr& e, Float_type& t)
{
  return e;
}


// An integer type can be converted to a float type.
//
// TODO: Implement me.
Expr&
convert_integer_to_float(Expr& e, Float_type& t)
{
  return e;
}


// Try a floating point conversion.
Expr&
convert_to_float(Expr& e, Float_type& t)
{
  if (is<Float_type>(&e.type()))
    return convert_to_wider_float(e, t);
  if (is<Integer_type>(&e.type()))
    return convert_integer_to_float(e, t);
  return e;
}


// Perform at most one value conversion from the following set:
//
//    - integer conversions
//    - float conversions
//    - numeric conversions (int to float)
//    - boolean conversions
//
// TODO: Support conversion of T[N] to T[].
//
// TODO: Support pointer conversions.
//
// TODO: Support character conversions separately from integer
// conversions?
//
// TODO: Why are references not converted in C++?
Expr&
convert_value(Expr& e, Type& t)
{
  // Value conversions do not apply to reeference types.
  if (is<Reference_type>(&e.type()))
    return e;

  // Ignore qualifications in this comparison. Value categories
  // include the cv-qualified versions of types.
  Type& u = t.unqualified_type();

  // Try a boolean conversion.
  if (Boolean_type* b = as<Boolean_type>(&u))
    return convert_to_bool(e, *b);

  // Try an integer conversion.
  if (Integer_type* z = as<Integer_type>(&u))
    return convert_to_wider_integer(e, *z);

  // Try one of the floating point conversions.
  if (Float_type* f = as<Float_type>(&u))
    return convert_to_float(e, *f);

  return e;
}


// -------------------------------------------------------------------------- //
// Type similarity
//
// Two types are similar if they have the same syntax modulo
// qualifications.

bool is_similar(Type const&, Type const&);
bool is_similar(Pointer_type const&, Pointer_type const&);
bool is_similar(Array_type const&, Array_type const&);
bool is_similar(Sequence_type const&, Sequence_type const&);

bool
is_similar(Type const& a, Type const& b)
{
  struct fn
  {
    Type const& b;
    bool operator()(Void_type const& a)      { return is_equivalent(a, cast<Void_type>(b)); }
    bool operator()(Boolean_type const& a)   { return is_equivalent(a, cast<Boolean_type>(b)); }
    bool operator()(Integer_type const& a)   { return is_equivalent(a, cast<Integer_type>(b)); }
    bool operator()(Float_type const& a)     { return is_equivalent(a, cast<Float_type>(b)); }
    bool operator()(Auto_type const& a)      { return is_equivalent(a, cast<Auto_type>(b)); }
    bool operator()(Decltype_type const& a)  { return is_equivalent(a, cast<Decltype_type>(b)); }
    bool operator()(Declauto_type const& a)  { return is_equivalent(a, cast<Declauto_type>(b)); }
    bool operator()(Function_type const& a)  { return is_equivalent(a, cast<Function_type>(b)); }
    bool operator()(Reference_type const& a) { return is_equivalent(a, cast<Reference_type>(b)); }
    bool operator()(Qualified_type const&)   { lingo_unreachable(); }
    bool operator()(Pointer_type const& a)   { return is_similar(a, cast<Pointer_type>(b)); }
    bool operator()(Array_type const& a)     { return is_similar(a, cast<Array_type>(b)); }
    bool operator()(Sequence_type const& a)  { return is_similar(a, cast<Sequence_type>(b)); }
    bool operator()(Class_type const& a)     { return is_equivalent(a, cast<Class_type>(b)); }
    bool operator()(Union_type const& a)     { return is_equivalent(a, cast<Union_type>(b)); }
    bool operator()(Enum_type const& a)      { return is_equivalent(a, cast<Enum_type>(b)); }
    bool operator()(Typename_type const& a)  { return is_equivalent(a, cast<Typename_type>(b)); }
  };

  Type const& ua = a.unqualified_type();
  Type const& ub = b.unqualified_type();
  std::type_index t1 = typeid(ua);
  std::type_index t2 = typeid(ub);
  if (t1 != t2)
    return false;
  return apply(ua, fn{ub});
}


bool
is_similar(Pointer_type const& a, Pointer_type const& b)
{
  return is_similar(a.type(), b.type());
}


bool
is_similar(Array_type const& a, Array_type const& b)
{
  // TODO: Check for equivalence of the extent before
  // recursing on the element type.
  lingo_unimplemented();
}


bool
is_similar(Sequence_type const& a, Sequence_type const& b)
{
  return is_similar(a.type(), b.type());
}


// -------------------------------------------------------------------------- //
// Qualifier signature
//
// A type's qualifier signature is a vector of qualifiers over the
// composition of the type (from left to right). For example:
//
//    T const* volatile*[]
//
// has the signature [c, v, 0] (where 0 represents the empty qualifier).

// Recursively construct the qualifier list working from
// right to left in the type composition.
void
get_qualification_signature(Type const& t, Qualifier_list& sig)
{
  struct fn
  {
    Qualifier_list& sig;
    void operator()(Void_type const&)        { }
    void operator()(Boolean_type const&)     { }
    void operator()(Integer_type const&)     { }
    void operator()(Float_type const&)       { }
    void operator()(Auto_type const&)        { }
    void operator()(Decltype_type const&)    { }
    void operator()(Declauto_type const&)    { }
    void operator()(Function_type const&)    { }
    void operator()(Reference_type const&)   { }
    void operator()(Qualified_type const& t) { lingo_unreachable(); }
    void operator()(Pointer_type const& t)   { get_qualification_signature(t.type(), sig); }
    void operator()(Array_type const& t)     { lingo_unimplemented(); }
    void operator()(Sequence_type const& t)  { get_qualification_signature(t.type(), sig); }
    void operator()(Class_type const&)       { }
    void operator()(Union_type const&)       { }
    void operator()(Enum_type const&)        { }
    void operator()(Typename_type const&)    { }
  };

  // Determine the qualifier for the type component.
  if (Qualified_type const* q = as<Qualified_type>(&t))
    sig.push_back(q->qualifier());
  else
    sig.push_back(0);

  apply(t.unqualified_type(), fn{sig});
}


// Returns the qualification singature of `t`.
Qualifier_list
get_qualification_signature(Type const& t)
{
  Qualifier_list s;
  get_qualification_signature(t, s);

  // The list is built backwards. Reverse it.
  std::reverse(s.begin(), s.end());

  return s;
}


// Returns true if the qualifier q contains const.
inline bool
has_const(int q)
{
  return q & const_qual;
}


// Returns true if the qualifier q contains volatile.
inline bool
has_volatile(int q)
{
  return q & const_qual;
}


// Determine if the qualification signature a can be converted
// to b. This is the case when...
//
// TODO: Document me. This is [conv.qual]/p3.2-3.
bool
can_convert_signature(Qualifier_list const& a, Qualifier_list const& b)
{
  lingo_assert(a.size() == b.size());
  std::size_t n = 0;
  for (std::size_t i = 1; i < a.size(); ++i) {
    // If const is in a, then const must be in b.
    if (has_const(a[i]) && !has_const(b[i]))
      return false;

    // And similarly for volatile.
    if (has_volatile(a[i]) && !has_volatile(b[i]))
      return false;

    // If the qualifiers differ, then each qualifier 0 < k < i
    // must have const. There could be many differences.
    // Save the furthest in the chain and check after later.
    if (a[i] != b[i])
      n = i;
  }

  // Check for const propagation in differing signatures.
  for (std::size_t k = 1; k < n; ++k) {
    if (!has_const(b[k]))
      return false;
  }

  return true;
}


// -------------------------------------------------------------------------- //
// Qualifications

// A value of type T1 can be converted to a type T2 if they are
// similar and... something about the cv-signature.
//
// Note that the top-level cv-qualifiers can be removed by this
// conversion since it applies to values (i.e., copies).
Expr&
convert_qualifier(Expr& e, Type& t)
{
  if (is_similar(e.type(), t)) {
    Qualifier_list sa = get_qualification_signature(e.type());
    Qualifier_list sb = get_qualification_signature(t);
    if (can_convert_signature(sa, sb))
      return *new Qualification_conv(t, e);
  }
  return e;
}


// -------------------------------------------------------------------------- //
// Implicit conversion

// Try to find a conversion from a source expression `e` and
// a target type `t`.
//
// FIXME: Should `t` be an object type? That is we should perform
// conversions iff we can declare an object of type T?
Expr&
convert(Expr& e, Type& t)
{
  Expr& c1 = convert_category(e, t);
  if (is_equivalent(c1.type(), t))
    return c1;

  Expr& c2 = convert_value(c1, t);
  if (is_equivalent(c2.type(), t))
    return c2;

  Expr& c3 = convert_qualifier(c2, t);
  if (is_equivalent(c3.type(), t))
    return c3;

  // FIXME: Emit better diagnostics.
  throw std::runtime_error("conversion error");
}


// Try to find a conversion from a source expression `e` and
// a target type `t`.
Expr&
convert(Expr const& e, Type const& t)
{
  // Just forward to the non-const version of this function.
  // We strip the const qualifier because we're going to be
  // building new terms.
  return convert(*modify(&e), *modify(&t));
}


} // namespace beaker
