// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "conversion.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "constraint.hpp"
#include "initialization.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Categorical conversions

// If needed, convert a reference to an object.
//
// In C++, this would be a glvalue to prvalue conversion.
//
// TODO: Check that e's type is complete before invoking the conversion.
//
// FIXME: Allocate using the context.
Expr&
convert_reference_to_object(Context& cxt, Expr& e)
{
  if (e.type().is_reference()) {
    Type& t = cxt.get_non_reference_type(e.type());
    return *new Value_conv(t, e);
  }
  return e;
}


// If needed, convert a function to an object.
Expr&
convert_reference_to_function(Context& cxt, Expr& e)
{
  lingo_unreachable();
}


// Perform at most one categorical conversion.
Expr&
convert_category(Context& cxt, Expr& e, Type& t)
{
  // If the destination category is a value expression, then we
  // may need a reference to value conversion.
  if (t.is_object())
    return convert_reference_to_object(cxt, e);
  if (t.is_function())
    return convert_reference_to_function(cxt, e);
  return e;
}


// -------------------------------------------------------------------------- //
// Value conversions

// The following object expressions can be implicitly converted to bool. 
//
//    - expressions of integer type
//    - expressions of floating point type
//    - expressions of pointer type
//
// The result of a such a conversion is a value expression of type bool.
Expr&
convert_to_bool(Context& cxt, Expr& e)
{
  struct fn
  {
    Context& cxt;
    Expr& e;
    Type& t;

    Expr& operator()(Type&)           { return e; }
    Expr& operator()(Integer_type& t) { return *new Boolean_conv(t, e); }
    Expr& operator()(Float_type& t)   { return *new Boolean_conv(t, e); }
    Expr& operator()(Pointer_type& t) { return *new Boolean_conv(t, e); }
  };
  return apply(e.type(), fn{cxt, e, cxt.get_bool_type(object_type)});
}


// A value of integer type can be converted to a value of a wider integer 
// type. A value of type bool can be converted to integer s.t.
// false is 0 and true is 1.
//
// TODO: Implement me.
Expr&
convert_to_integer(Context& cxt, Expr& e, Integer_type& t)
{
  return e;
}


// A value of float type can be converted to a value of a wider
// float type.
//
// TODO: Implement me.
Expr&
convert_to_wider_float(Context& cxt, Expr& e, Float_type& t)
{
  return e;
}


// An integer type can be converted to a float type.
//
// TODO: Implement me.
Expr&
convert_integer_to_float(Context& cxt, Expr& e, Float_type& t)
{
  return e;
}


// Try a floating point conversion.
Expr&
convert_to_float(Context& cxt,Expr& e, Float_type& t)
{
  if (is<Float_type>(&e.type()))
    return convert_to_wider_float(cxt, e, t);
  if (is<Integer_type>(&e.type()))
    return convert_integer_to_float(cxt, e, t);
  return e;
}


// Perform at most one value conversion from the following set:
//
//    - integer conversions
//    - float conversions
//    - numeric conversions (int to float)
//    - boolean conversions (type to bool)
//
// Note that all such conversions take value expressions as operands: a
// reference-to-value conversion must have been previously applied to
// a reference expressions.
//
// TODO: Implement integer and floating point promotion.
//
// TODO: Support conversion of T[N] to T[].
//
// TODO: Support pointer conversions.
//
// TODO: Support character conversions separately from integer
// conversions?
Expr&
convert_value(Context& cxt, Expr& e, Type& t)
{
  // Reference expressions do not participate in value conversions.
  if (e.type().is_reference())
    return e;

  // Determine which conversion to apply based on the destination
  // type of the conversion.
  struct fn
  {
    Context& cxt;
    Expr& e;
    Type& t;

    Expr& operator()(Type&)           { return e; }
    Expr& operator()(Integer_type& t) { return convert_to_integer(cxt, e, t); }
    Expr& operator()(Float_type& t)   { return convert_to_float(cxt, e, t); }
    Expr& operator()(Boolean_type& t) { return convert_to_bool(cxt, e); }
  };
  return apply(t, fn{cxt, e, t});
}


// -------------------------------------------------------------------------- //
// Type similarity
//
// Two types are similar if they have the same syntax type modulo
// sequences of qualifications. For example, the following types
// are similar (but not equivalent).
//
//    int const* const
//    int*
//
// Note that similarity is essentially an equivalence relation on types 
// that does not consider type qualifiers: all variants of a type are in 
// the same equivalence class.

bool is_similar(Type const&, Type const&);


bool
is_similar(Pointer_type const& a, Pointer_type const& b)
{
  return is_similar(a.type(), b.type());
}


// FIXME: Arrays are only similar when they have the same extent.
bool
is_similar(Array_type const& a, Array_type const& b)
{
  return is_similar(a.element_type(), b.element_type());
}


// Two tuple types are similar only when they are equivalent.
//
// TODO: There is probably a valid generalization of this. Maybe
// two tuple types are similar if all their elements are similar?
bool
is_similar(Tuple_type const& a, Tuple_type const& b)
{
  return is_equivalent(a, b);
}


bool
is_similar(Type const& a, Type const& b)
{
  struct fn
  {
    Type const& b;
    bool operator()(Type const& a)           { return is_equivalent(a, b); }
    bool operator()(Pointer_type const& a)   { return is_similar(a, cast<Pointer_type>(b)); }
    bool operator()(Array_type const& a)     { return is_similar(a, cast<Array_type>(b)); }
    bool operator()(Tuple_type const& a)     { return is_similar(a, cast<Tuple_type>(b)); }
  };

  if (typeid(a) != typeid(b))
    return false;
  else
    return apply(a, fn{b});
}


// -------------------------------------------------------------------------- //
// Qualifier signature


// A type's qualifier signature is a vector of qualifiers over the
// composition of the type (from left to right). For example:
//
//    T const* volatile*[]
//
// has the signature [c, v, 0] (where 0 represents the empty qualifier).
// The last qualifier is empty because there are no qualifiers on the
// array (right?).
//
// Recursively construct the qualifier list working from
// right to left in the type composition.
void
get_qualification_signature(Type const& t, Qualifier_list& sig)
{
  struct fn
  {
    Qualifier_list& sig;
    void operator()(Type const&)             { /* Do nothing. */}
    void operator()(Pointer_type const& t)   { get_qualification_signature(t.type(), sig); }
    void operator()(Array_type const& t)     { lingo_unhandled(t); }
    void operator()(Tuple_type const& t)     { lingo_unhandled(t); }
  };
  sig.push_back(t.qualifiers());
  apply(t, fn{sig});
}


// Returns the qualification singature of `t`.
Qualifier_list
get_qualification_signature(Type const& t)
{
  Qualifier_list s;
  get_qualification_signature(t, s);
  std::reverse(s.begin(), s.end());  // We built the list backwards
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

// A value of type T1 can be converted to a type T2 if they are similar 
// and... something about the cv-signature.
//
// Note that the top-level cv-qualifiers can be removed by this
// conversion since it applies to values (i.e., copies).
//
// The expression category of the converted expression is that of
// the source expression.
Expr&
convert_qualifier(Context& cxt, Expr& e, Type& t)
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

// Search for a sequence of conversions from expression `e` to a destination
// category c and type t.
Expr&
standard_conversion(Context& cxt, Expr& e, Type& t)
{
  // No conversions are required if the types are the same.
  if (is_equivalent(e.type(), t))
    return e;

  // Try a categorical conversion.
  Expr& c1 = convert_category(cxt, e, t);
  if (is_equivalent(c1.type(), t))
    return c1;

  // Try a value conversion.
  Expr& c2 = convert_value(cxt, c1, t);
  if (is_equivalent(c2.type(), t))
    return c2;

  // Try a qualification adjustment.
  Expr& c3 = convert_qualifier(cxt, c2, t);
  if (is_equivalent(c3.type(), t))
    return c3;

  error(cxt, "cannot convert '{}' (type '{}') to '{}'", e, e.type(), t);
  throw Type_error();
}


// Search for a conversion from the source expression e to a value 
// expression of type t.
Expr&
standard_conversion_to_value(Context& cxt, Expr& e, Type& t)
{
  return standard_conversion(cxt, e, t);
}


// -------------------------------------------------------------------------- //
// Arithmetic conversions

// Assuming the type of e1 is untested and e2 has floating point
// type, convert to the most precise floating point type.
Expr_pair
convert_to_common_float(Context& cxt, Expr& e1, Expr& e2)
{
  Float_type& f2 = cast<Float_type>(e2.type());

  // If e1 has float type, convert to the most precise type.
  if (has_floating_point_type(e1)) {
    Float_type& f1 = cast<Float_type>(e1.type());
    if (f1.precision() < f2.precision()) {
      Expr& c = convert_to_wider_float(cxt, e1, f2);
      return {c, e2};
    }
    if (f2.precision() < f1.precision()) {
      Expr& c = convert_to_wider_float(cxt, e2, f1);
      return {e1, c};
    }
    lingo_unreachable();
  }

  // If e1 has integer type, convert to e2.
  if (has_integer_type(e1)) {
    Expr& c = convert_integer_to_float(cxt, e1, f2);
    return {c, e2};
  }

  error(cxt, "no floating point conversions for '{}' and '{}'", e1, e2);
  throw Type_error();
}

Expr_pair
convert_to_common_int(Context& cxt, Expr& e1, Expr& e2)
{
  Integer_type& t1 = cast<Integer_type>(e1.type());
  Integer_type& t2 = cast<Integer_type>(e2.type());

  // If both types have the same sign, convert to the one with
  // the most precision.
  if (t1.sign() == t2.sign()) {
    if (t1.precision() < t2.precision()) {
      Expr& c = convert_to_integer(cxt, e1, t2);
      return {c, e2};
    }
    if (t2.precision() < t1.precision()) {
      Expr& c = convert_to_integer(cxt, e2, t1);
      return {e1, c};
    }
  }

  // If the unsigned operand has greater rank than the signed
  // operand, convert to the type of the unsigned operand.
  if (t1.is_unsigned() && t2.precision() < t1.precision()) {
    Expr& c = convert_to_integer(cxt, e2, t1);
    return {e1, c};
  }
  if (t2.is_unsigned() && t1.precision() < t2.precision()) {
    Expr& c = convert_to_integer(cxt, e1, t2);
    return {c, e2};
  }

  // Otherwise, both operands are converted to the corresponding
  // unsigned type of the signed operand.
  int p = t1.is_signed() ? t1.precision() : t2.precision();
  Integer_type& z = cxt.get_integer_type(false, p);
  Expr& c1 = convert_to_integer(cxt, e1, z);
  Expr& c2 = convert_to_integer(cxt, e2, z);
  return {c1, c2};
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
arithmetic_conversion(Context& cxt, Expr& e1, Expr& e2)
{
  // If the types are the same, no conversions are applied.
  if (is_equivalent(e1.type(), e2.type()))
    return {e1, e2};

  // If either operand has floating point type, convert to the type
  // with the greatest precision.
  //
  // TODO: Why isn't convert_to_common_float symmetric? This seems like
  // an issue.
  if (has_floating_point_type(e1))
    return convert_to_common_float(cxt, e2, e1);
  if (has_floating_point_type(e2))
    return convert_to_common_float(cxt, e1, e2);

  // If both operands have integer type, the following rules apply.
  if (has_integer_type(e1) && has_integer_type(e2))
    return convert_to_common_int(cxt, e1, e2);

  // TODO: No conversion from e1 to e2.
  error(cxt, "no usual arithmetic conversion of '{}' and '{}'", e1, e2);
  throw Type_error();
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
//
// Contextual conversion to bool tries to do this:
//
//    var b : bool = e;
//
// so we preform copy initialization, and then yield the converted
// expression that will be used to initialize b.
Expr&
contextual_conversion_to_bool(Context& cxt, Expr& e)
{
  // Synthesize a declaration.
  //
  // TODO: This epitomizes our need for smarter memory management. I
  // really want this declaration and its associated nodes to evaporate
  // when the function returns. Note that the initialization and
  // conversion are allocated in the main arena.
  Object_decl& var = cxt.make_object_declaration("b", cxt.get_bool_type());

  // Actually perform the copy initialization.
  Expr& init = copy_initialize(cxt, var, e);

  // Based on the initializer selected, we can either:
  //  1. return the converted value directly
  //  2. synthesize an object using a constructor
  //  3. invoke a user-defined conversion
  if (Copy_init* i = as<Copy_init>(&init))
    return i->expression();  
  
  lingo_unhandled(init);
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

  error(cxt, "no admissible conversion from '{}' to '{}'", e, t);
  throw Type_error();
}


} // namespace banjo
