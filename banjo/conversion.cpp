// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "conversion.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "constraint.hpp"
#include "initialization.hpp"
#include "printer.hpp"

#include <typeindex>
#include <iostream>


namespace banjo
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
// In C++, this is a glvalue to prvalue conversion.
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
// extension. That depends on the destination type. Perhaps use
// different conversions for these values?
//
// Also use a different conversion for bool-to-int?
Expr&
convert_to_wider_integer(Expr& e, Integer_type& t)
{
  // A value of integer type can be converted...
  if (has_integer_type(e)) {
    Integer_type& et = cast<Integer_type>(e.type());
    // TODO: Be more precise about the conversion that's
    // actually going to happen. Especially, if we convert
    // sign and widen simultaneously.
    if (et.precision() < t.precision())
      return *new Integer_conv(t, e);
    else if (et.sign() != t.sign())
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
// sequences of qualifications. For example, the following types
// are similar:
//
//    int const* const
//    int*

bool is_similar(Type const&, Type const&);


bool
is_similar(Reference_type const& a, Reference_type const& b)
{
  return is_similar(a.type(), b.type());
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
  return is_similar(a.type(), b.type());
}


bool
is_similar(Tuple_type const& a, Tuple_type const& b)
{
  auto ita = a.type_list().begin();
  auto itb = b.type_list().begin();
  do {
    if(!is_similar(*ita,*itb)) return false;
    ita++;itb++;
  } while(ita != a.type_list().end() && itb != b.type_list().end());
  return true;
}


bool
is_similar(Slice_type const& a, Slice_type const& b)
{
  return is_similar(a.type(), b.type());
}


bool
is_similar(Dynarray_type const& a, Dynarray_type const& b)
{
  return is_similar(a.type(), b.type());
}


bool
is_similar(Type const& a, Type const& b)
{
  struct fn
  {
    Type const& b;
    bool operator()(Type const& a)           { return is_equivalent(a, b); }
    bool operator()(Qualified_type const&)   { lingo_unreachable(); }
    bool operator()(Pointer_type const& a)   { return is_similar(a, cast<Pointer_type>(b)); }
    bool operator()(Array_type const& a)     { return is_similar(a, cast<Array_type>(b)); }
    bool operator()(Tuple_type const& a)     { return is_similar(a, cast<Tuple_type>(b)); }
    bool operator()(Slice_type const& a)     { return is_similar(a, cast<Slice_type>(b)); }
    bool operator()(Dynarray_type const& a)  { return is_similar(a, cast<Dynarray_type>(b)); }
  };

  Type const& ua = a.unqualified_type();
  Type const& ub = b.unqualified_type();

  if (typeid(ua) != typeid(ub))
    return false;
  else
    return apply(ua, fn{ub});
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
    void operator()(Type const&)             { }
    void operator()(Qualified_type const& t) { lingo_unreachable(); }
    void operator()(Pointer_type const& t)   { get_qualification_signature(t.type(), sig); }
    void operator()(Array_type const& t)     { lingo_unreachable(); }
    void operator()(Tuple_type const& t)     { lingo_unreachable(); }
    void operator()(Slice_type const& t)     { get_qualification_signature(t.type(), sig); }
    void operator()(Dynarray_type const& t)  { lingo_unreachable(); }
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
// Standard conversions

// Try to find a standard conversion sequence from a source
// expression `e` and a destination type `t`.
//
// FIXME: Should `t` be an object type? That is we should perform
// conversions iff we can declare an object of type T?
Expr&
standard_conversion(Expr& e, Type& t)
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

  throw Type_error("cannot convert '{}' (type '{}') to '{}'", e, e.type(), t);
}


bool is_tuple_equiv_to_array(Tuple_type& t1, Array_type& t2)
{
  for(auto it = t1.type_list().begin(); it != t1.type_list().end(); it++) {
    if(!is_equivalent(*it,t2.type())) return false;
  }
  return true;
}


// Try to find a conversion from a source expression `e` and
// a destination type `t`.
Expr&
standard_conversion(Expr const& e, Type const& t)
{
  // Just forward to the non-const version of this function.
  // We strip the const qualifier because we're going to be
  // building new terms.
  return standard_conversion(modify(e), modify(t));
}


// -------------------------------------------------------------------------- //
// Arithmetic conversions

// Assuming the type of e1 is untested and e2 has floating point
// type, convert to the most precise floating point type.
Expr_pair
convert_to_common_float(Expr& e1, Expr& e2)
{
  Float_type& f2 = cast<Float_type>(e2.type());

  // If e1 has float type, convert to the most precise.
  if (has_floating_point_type(e1)) {
    Float_type& f1 = cast<Float_type>(e1.type());
    if (f1.precision() < f2.precision())
      return {convert_to_wider_float(e1, f2), e2};
    if (f2.precision() < f1.precision())
      return {e1, convert_to_wider_float(e2, f1)};
  }

  // If e1 has integer type, convert to e2.
  if (has_integer_type(e1)) {
    return {convert_integer_to_float(e1, f2), e2};
  }

  throw Type_error("no floating point conversions for '{}' and '{}'", e1, e2);
}

Expr_pair
convert_to_common_int(Expr& e1, Expr& e2)
{
  Integer_type& t1 = cast<Integer_type>(e1.type());
  Integer_type& t2 = cast<Integer_type>(e2.type());

  // If both types have the same sign, convert to the one with
  // the most precision.
  if (t1.sign() == t2.sign()) {
    if (t1.precision() < t2.precision())
      return {convert_to_wider_integer(e1, t2), e2};
    if (t2.precision() < t1.precision())
      return {e1, convert_to_wider_integer(e2, t1)};
  }

  // If the unsigned operand has greater rank than the signed
  // operand, convert to the type of the unsigned operand.
  if (t1.is_unsigned() && t2.precision() < t1.precision())
    return {e1, convert_to_wider_integer(e2, t1)};
  if (t2.is_unsigned() && t1.precision() < t2.precision())
    return {convert_to_wider_integer(e1, t2), e2};

  // Otherwise, both operands are converted to the corresponding
  // unsigned type of the signed operand.
  //
  // FIXME: Use a Builder (hence context) for this conversion.
  int p = t1.is_signed() ? t1.precision() : t2.precision();
  Integer_type& c = *new Integer_type(false, p);
  return {convert_to_wider_integer(e1, c), convert_to_wider_integer(e2, c)};
}


// Perform the usual arithmetic conversions on `e1` and `e2`. This
// tries to find a common type for `e1` and `e2` and convert both
// expressions to that type.
//
// NOTE: In C++ lvalue-to-rvalue conversions are required on a
// per-expression basis, independently of converting to a common
// type. Also, non-user-defined types are unqualified prior to
// analysis. It would be easier if we found an absolute common
// type and then instantiated a declaration suitable for overload
// resolution. Maybe.
//
// TODO: Handle conversions for character types (or promote to a
// corresponding integer type?).
//
// TODO: How does bool work with this set of conversions? Promote
// bool to int?
//
// TODO: Can we unify this with the common type required by the
// conditional expression? Note that the arithmetic version converts
// to values, and the conditional expression can retain references.
Expr_pair
arithmetic_conversion(Expr& e1, Expr& e2)
{
  // If the types are the same, no conversions are applied.
  if (is_equivalent(e1.type(), e2.type()))
    return {e1, e2};

  // If either operand has floating point type, convert to the type
  // with the greatest precision.
  if (has_floating_point_type(e1))
    return convert_to_common_float(e2, e1);
  if (has_floating_point_type(e2))
    return convert_to_common_float(e1, e2);

  // If both oerands have integer type, the following rules apply.
  if (has_integer_type(e1) && has_integer_type(e2))
    return convert_to_common_int(e1, e2);

  // TODO: No conversion from e1 to e2.
  throw Type_error("no usual arithmetic conversions for '{}' and '{}'", e1, e2);
}


Expr_pair
arithmetic_conversion(Expr const& e1, Expr const& e2)
{
  return arithmetic_conversion(modify(e1), modify(e2));
}


// -------------------------------------------------------------------------- //
// Conversion sequences

// Return the conversion sequence for the given expression.
//
// Because conversions are unary operators, we can simply walk
// the list and accumulate information as we go.
//
// TODO: Add support for detecting ellipsis conversions
// and also user-defined conversions.
Conversion_seq
get_conversion_sequence(Expr& e)
{
  struct fn
  {
    Standard_conversion_seq& seq;

    // Do nothing for non-conversion expressions.
    Expr* operator()(Expr&)                { return nullptr; }

    // Set the corresponding conversion level.
    Expr* operator()(Value_conv& e)         { seq.transformation(e); return &e.source(); }
    Expr* operator()(Qualification_conv& e) { seq.adjustment(e); return &e.source(); }
    Expr* operator()(Boolean_conv& e)       { seq.conversion(e); return &e.source(); }
    Expr* operator()(Integer_conv& e)       { seq.conversion(e); return &e.source(); }
    Expr* operator()(Float_conv& e)         { seq.conversion(e); return &e.source(); }
    Expr* operator()(Numeric_conv& e)       { seq.conversion(e); return &e.source(); }

    // This is handled elsewhere.
    Expr* operator()(Ellipsis_conv& e)      { lingo_unreachable(); }
  };

  // Handle the trivial case first.
  //
  // TODO: Set type information?
  if (is_ellipsis_conversion(e))
    return Conversion_seq(Ellipsis_conversion_seq{});

  // Unwind standard conversion from the expression.
  Expr* p = &e;
  Standard_conversion_seq seq;
  while (is_standard_conversion(*p))
    p = apply(*p, fn{seq});

  // TODO: If p is a user-defined conversion, build a user-defined
  // conversion sequence with seq as the "after" component,
  // and then unwind the standard conversions for the "before"
  // part.

  return Conversion_seq(seq);
}


Conversion_seq
get_conversion_sequence(Expr const& e)
{
  return get_conversion_sequence(modify(e));
}


// -------------------------------------------------------------------------- //
// Ordering of conversion sequences


// FIXME: Finish implementing this.
Conversion_comp
compare(Standard_conversion_seq const& s1, Standard_conversion_seq const& s2)
{
  // If s1 is a proper subsequence of s2.
  // TODO: How is a subsequence defined?

  // or s1 is better than s2 according to some rules...

  // ... some stuff about refernce bindings
  return indistinct_conv;
}



Conversion_comp
compare(Conversion_seq const& a, Conversion_seq const& b)
{
  // A standad conversion sequence is better then a user-define
  // conversion sequence and an ellipsis conversion sequence.
  if (a.kind() == std_conv_seq && b.kind() != std_conv_seq)
    return better_conv;
  if (b.kind() == std_conv_seq && a.kind() != std_conv_seq)
    return worse_conv;

  // A user-defined conversion sequence is better than an ellipsis
  // conversion sequence.
  if (a.kind() == user_conv_seq && b.kind() == ellipsis_conv_seq)
    return better_conv;
  if (b.kind() == user_conv_seq && a.kind() == ellipsis_conv_seq)
    return worse_conv;

  // Distinguish between converison sequences of the same kind.
  if (a.kind() == std_conv_seq && b.kind() == std_conv_seq)
    return compare(a.standard_conversions(), b.standard_conversions());

  // We cannot distinguis the conversion sequences.
  return indistinct_conv;
}


// -------------------------------------------------------------------------- //
// Contextual conversions


// Determine if e can be contextually converted to bool, and if so,
// returns the expression that produces a boolean value from `e`.
Expr&
contextual_conversion_to_bool(Context& cxt, Expr& e)
{
  // Contextual conversion to bool tries to do this:
  //
  //    bool b(e);
  //
  // so we preform direct initialization.
  Expr& init = direct_initialize(cxt, cxt.get_bool_type(), e);

  // Based on the initializer selected, we can either
  // 1. return the converted value directly
  // 2. synthesize an object using a constructor
  // 3. invoke a user-defined conversion
  if (Copy_init* i = as<Copy_init>(&init))
    return i->expression();
  banjo_unhandled_case(init);
}


// -------------------------------------------------------------------------- //
// Dependent conversions

// Search for dependent conversions. This is done in the case that:
//
//    - e has dependent type,
//    - t is a dependent type,
//    - or both.
//
// A dependent conversion is either a standard, conversion conisisting
// of a object-to-value conversion and a qualification adjustment, or
// it is a conversion admitted by a constraint.
Expr&
dependent_conversion(Context& cxt, Expr& e, Type& t)
{
#if 0
  // In certain contexts, no conversions are applied.
  if (cxt.in_requirements())
    return e;
  if (!cxt.current_template_constraints())
    return e;

  // Determine if we can reach the destination type by a
  // standard set of conversions on the dependent source
  // expression. Discard that conversion and replace it with
  // a dependent conversion.
  try {
    // FIXME: If an object-to-value conversion is applied, then
    // we need to also ensure that the type is copy constructible.
    // Note that copy constructible would also entail move
    // constructible.
    Expr& c = standard_conversion(e, t);
    (void)c;
    return *new Dependent_conv(t, e);
  } catch (Translation_error&) {
    // Fall through...
  }

  // Search for a conversion to t among the listed constraints.
  Expr& cons = *cxt.current_template_constraints();
  if (Expr* c = admit_conversion(cxt, cons, e, t)) {
    return *c;
  }
#endif

  throw Type_error(cxt, "no admissible conversion from '{}' to '{}'", e, t);
}


} // namespace banjo
