// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "convert.hpp"
#include "ast.hpp"
#include "equivalence.hpp"
#include "print.hpp"

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
Expr&
convert_to_wider_float(Expr& e, Float_type& t)
{
  return e;
}


// An integer type can be converted to a float type.
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
// Qualifications

Expr&
convert_to_more_qualified(Expr& e, Type& t)
{
  Type& et = e.type();
  if (is_equivalent(et.unqualified_type(), t.unqualified_type())) {
    if (is_more_qualified_type(t, et))
      return *new Qualification_conv(t, e);
  }
  return e;
}

// A qualified type can be converted to a more qualified type.
//
// TODO: Handle the not-so obvious rules for pointers, arrays,
// and sequences.
Expr&
convert_qualifier(Expr& e, Type& t)
{
  if (Qualified_type* q = as<Qualified_type>(&t))
    return convert_to_more_qualified(e, *q);
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
