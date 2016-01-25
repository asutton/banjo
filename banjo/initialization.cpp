// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "initialization.hpp"
#include "conversion.hpp"
#include "inheritance.hpp"
#include "equivalence.hpp"
#include "ast.hpp"
#include "builder.hpp"


namespace banjo
{

// Select a zero-initialization procedure for an object or
// reference of type `t`. Note that zero initialization never
// chooses a constructor.
//
// TODO: What about sequence types and incomplete types?
// Presumably object types are required to be complete prior
// to initialization (i.e., we need the size for memset).
//
// It's possible that T[] counts as a scalar (that's what it
// code-gens to).
Expr&
zero_initialize(Context& cxt, Type& t)
{
  lingo_assert(!is_function_type(t));

  Builder build(cxt);

  // No initialization is performed for reference types.
  if (is_reference_type(t))
    return build.make_trivial_init(t);

  // Zero initialize each sub-object in turn.
  if (is_array_type(t))
    lingo_unimplemented();

  Type& u = t.unqualified_type();

  // Zero each sub-object in turn.
  if (is_class_type(u))
    lingo_unimplemented();

  // Zero initialize the first sub-object.
  if (is_union_type(u))
    lingo_unimplemented();

  if (is_scalar_type(u))
    return build.make_zero_init(t, build.get_zero(t));

  // FIXME: I'm not sure that we should have an error here.
  throw std::runtime_error("cannot zero initialize type");
}


// Select a default initialization procedure for an object
// of type `t`.
//
// TODO: What about sequence types? What does it mean to
// default initialize a T[].
//
// TODO: Consider making these 0 initialized by defualt and
// using a special syntax to select trivial initialization.
Expr&
default_initialize(Context& cxt, Type& t)
{
  Builder build(cxt);

  if (is_reference_type(t))
    throw std::runtime_error("default initialization of reference");

  // Select a default initializer for each sub-object.
  if (is_array_type(t))
    lingo_unimplemented();

  // Select a (possibly synthesized) default constructor for u.
  if (is_maybe_qualified_class_type(t))
    lingo_unimplemented();

  // Select a (possibly synthesized) default constructor for u.
  if (is_maybe_qualified_union_type(t))
    lingo_unimplemented();

  // Otherwise, no initialization is performed.
  return build.make_trivial_init(t);
}


// Select a procedure to value-initialize an object.
Expr&
value_initialize(Context& cxt, Type& t)
{
  Builder build(cxt);

  if (is_reference_type(t))
    throw std::runtime_error("value initialization of reference");

  // FIXME: Can you value initialize a T[]?
  if (is_array_type(t))
    lingo_unimplemented();

  // Either zero-initialize or default-initialize based on
  // the presence of user-defined constructors.
  if (is_maybe_qualified_class_type(t) || is_maybe_qualified_union_type(t))
    lingo_unimplemented();

  // Are we sure that there are no other categories of types?
  return zero_initialize(cxt, t);
}


// For non-class/union types, paren-initialization shall
// have a single argument.
inline void
check_paren_initialization(Expr& i)
{
  if (is_paren_initialization(i)) {
    Paren_init& p = cast<Paren_init>(i);
    if (p.arguments().size() > 1)
      throw std::runtime_error("scalar initialized from multiple arguments");
  }
}


// Given a destination type and a syntactic initializer, select an
// initialization procedure.
//
// Note that the original initializer is a reflection of the syntax
// given, not the final initialization procedure.
Expr&
initialize(Context& cxt, Type& t, Init& i)
{
  Builder build(cxt);

  // If the initializer is {...}, then perform aggregate initalization.
  if (is_brace_initialization(i))
    return aggregate_initialize(cxt, t, cast<Brace_init>(i));

  // If the destination type is a T&, then perform reference
  // initialization.
  if (is_reference_type(t))
    return reference_initialize(cxt, cast<Reference_type>(t), i);

  // If the destination type is T[N] or T[] and the initializer
  // is `= s` where `s` is a string literal, perform string
  // initialization.
  // if (is_array_type(t) || is_sequence_type(t))
  //   lingo_unimplemented();

  // If the initializer is (), the object is value initialized.
  if (is_paren_initialization(i)) {
    Paren_init& p = cast<Paren_init>(i);
    if (p.arguments().empty())
      return value_initialize(cxt, t);
  }

  // Find an initialization procedure for user-defined compound
  // types, given the initializer i. Note that this can find
  // user-defined conversions for `t` from the initializer.
  if (is_maybe_qualified_class_type(t) || is_maybe_qualified_union_type(t))
    lingo_unimplemented();
  else
    check_paren_initialization(i);

  // If the initializer has a source type, then try to find a
  // user-defined conversion from s to the destination type.
  if (Type* s = i.source_type()) {
    Type& u = s->unqualified_type();
    if (is_class_type(u))
      lingo_unimplemented();
  }

  // If all else fails, try a a standard conversion.
  //
  // TODO: Explicitly disregard qualifiers here, or allow that to
  // happen in the conversion rules.
  //
  // NOTE: We must be guaranteed a source expression in this case.
  //
  // TODO: Catch exceptions and restructure the error with
  // the conversion error as an explanation.
  Expr& c = standard_conversion(*i.source(), t);
  return build.make_object_init(t, c);
}


// Perform copy initialization. Find an initialization procedure
// for an object of type `t` by the given expression. This performs
// initializations as for a variable of type T as if we had
// written:
//
//    T x = e;
//
// The full range of initializations and conversions apply.
Expr&
copy_initialize(Context& cxt, Type& t, Expr& e)
{
  Builder build(cxt);
  return initialize(cxt, t, build.make_equal_init(e));
}


// Perform direct initialization.
//
// TODO: This isn't just direct initialization. We need to account
// for whether braces or parens are used. Maybe we should have two
// function: brace_initialize() and paren_initialize().
Expr&
direct_initialize(Context&, Type&, Expr_list const&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Reference initialization


// A type q-T1 is reference-related to a type q-T2 if T1 and T2 are
// the same type or T1 is a base class of T2.
//
// TODO: Implement the base class test.
bool
is_reference_related(Type const& t1, Type const& t2)
{
  Type const& u1 = t1.unqualified_type();
  Type const& u2 = t2.unqualified_type();
  return is_equivalent(u1, u2);
}


// Returns true if t1 is noexcept T and t2 is T (where T is a
// function type). If neither t1 nor t2 are function types, this
// returns false.
//
// TODO: Implement me and put me in the right module.
//
// TODO: Require function types as a precondition?
bool
is_stricter_function(Type const& t1, Type const& t2)
{
  return false;
}


// Two types q1-t1 and q-t2 are reference compatible if either:
//
//    - t1 is reference-related to t2, or
//    - t2 is a stricter function type than t2
//
// and q1 is a superset of q2 (i.e., t1 is as qualified as or more
// qualified than t2).
//
// TODO: Check for ambiguous base classes.
bool
is_reference_compatible(Type const& t1, Type const& t2)
{
  if (is_reference_related(t1, t2) || is_stricter_function(t1, t2)) {
    Qualifier_set q1 = t1.qualifier();
    Qualifier_set q2 = t2.qualifier();
    return is_superset(q1, q2);
  }
  return false;
}


// Construct a reference initializer. This will produce temporary
// bindings as needed.
//
// TODO: Without rvaule references, every reference is an lvalue
// reference.
//
// TODO: A reference binding may invoke a conversion in order
// to bind to a sub-objet or a user-defined conversion. However,
// these aren't conversions in the standard sense.
Expr&
reference_initialize(Context& cxt, Reference_type& t1, Init& i)
{
  Builder build(cxt);

  // Reference initializaiton must have a source object.
  Expr* s = i.source();
  if (!s)
    throw std::runtime_error("reference initialized from multiple objects");
  Type& t2 = s->type();

  // The initializer has reference type.
  if (is_reference_type(t2)) {
    // If t1 is reference-compatible with t2, then bind directly.
    //
    // TODO: If we bind to a base class, we might need to apply a
    // base class conversion in order to explicitly adjust pointer
    // offsets.
    if (is_reference_compatible(t1, t2))
      return build.make_reference_init(t1, *s);

    // t2 has class type and has a user-defined conversion that is
    // reference compatible with t1, then bind the the to the
    // result of that conversion.
    if (is_class_type(t2))
      ; // Fall through for now...
  }

  // The reference must be a const reference.
  //
  // TODO: Handle const reference bindings to compound objects.
  if (t1.type().qualifier() == const_qual) {

  }

  // TODO: Handle bindings to temporaries.

  throw std::runtime_error("cannot bind reference");
}


// -------------------------------------------------------------------------- //
// Aggregate initialization

Expr&
aggregate_initialize(Context& cxt, Type& t, Brace_init& i)
{
  lingo_unimplemented();
}


} // namespace banjo
