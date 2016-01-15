// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "initialization.hpp"
#include "conversion.hpp"
#include "inheritance.hpp"
#include "equivalence.hpp"
#include "ast.hpp"
#include "builder.hpp"


namespace beaker
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
Init&
zero_initialize(Context& cxt, Type& t)
{
  lingo_assert(!is_function_type(t));

  Builder build(cxt);

  // No initialization is performed for reference types.
  if (is_reference_type(t))
    return build.make_trivial_init();

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
    return build.make_zero_init(build.get_zero(t));

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
Init&
default_initialize(Context& cxt, Type& t)
{
  Builder build(cxt);

  if (is_reference_type(t))
    throw std::runtime_error("default initialization of reference");

  // Select a default initializer for each sub-object.
  if (is_array_type(t))
    lingo_unimplemented();

  Type& u = t.unqualified_type();

  // Select a (possibly synthesized) default constructor for u.
  if (is_class_type(u))
    lingo_unimplemented();

  // Select a (possibly synthesized) default constructor for u.
  if (is_union_type(u))
    lingo_unimplemented();

  // Otherwise, no initialization is performed.
  return build.make_trivial_init();
}


// Select a procedure to value-initialize an object.
Init&
value_initialize(Context& cxt, Type& t)
{
  Builder build(cxt);

  if (is_reference_type(t))
    throw std::runtime_error("value initialization of reference");

  // FIXME: Can you value initialize a T[]?
  if (is_array_type(t))
    lingo_unimplemented();

  Type& u = t.unqualified_type();

  // Either zero-initialize or default-initialize based on
  // the presence of user-defined constructors.
  if (is_class_type(u) || is_union_type(u))
    lingo_unimplemented();

  // Are we sure that there are no other categories of types?
  return zero_initialize(cxt, t);
}


// For non-class/union types, paren-initialization shall
// have a single argument.
inline void
check_paren_initialization(Init& i)
{
  if (is_paren_initialization(i)) {
    Paren_init& p = cast<Paren_init>(i);
    if (p.arguments().size() > 1)
      throw std::runtime_error("scalar initialize from multiple arguments");
  }
}


// Given a destination type and a syntactic initializer, select an
// initialization procedure.
//
// Note that the original initializer is a reflection of the syntax
// given, not the final initialization procedure.
Init&
initialize(Context& cxt, Type& t, Init& i)
{
  Builder build(cxt);

  // If the initializer is {...}, then perform aggregate initalization.
  if (is_brace_initialization(i))
    return initialize_aggregate(t, cast<Brace_init>(i));

  // If the destination type is a T&, then perform reference
  // initialization.
  if (is_reference_type(t))
    return initialize_reference(t, i);

  // If the destination type is T[N] or T[] and the initializer
  // is `= s` where `s` is a string literal, perform string
  // initialization.
  // if (is_array_type(t) || is_sequence_type(t))
  //   lingo_unimplemented();

  // If the initializer is (), the object is value initialized.
  if (is_paren_initialization(i)) {
    Paren_init& p = cast<Paren_init>(i);
    if (p.arguments().empty())
      return value_initialize(t);
  }

  Type& u = t.unqualified_type();

  // Find an initialization procedure for user-defined compound
  // types, given the initializer i. Note that this can find
  // user-defined conversions for `t` from the initializer.
  if (is_class_type(u) || is_union_type(u))
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
  return build.make_object_init(c);
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
Init&
initialize_reference(Context& cxt, Type& t, Init& i)
{
  lingo_unimplemented();
  // Type& ut = t.unqualified_type();
}


// -------------------------------------------------------------------------- //
// Aggregate initialization

Init&
initialize_aggregate(Context& cxt, Type& t, Brace_init& i)
{
  lingo_unimplemented();
}


} // namespace beaker
