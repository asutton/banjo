// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "initialization.hpp"
#include "ast-type.hpp"
#include "ast-expr.hpp"
#include "conversion.hpp"
#include "inheritance.hpp"
#include "builder.hpp"



namespace banjo
{

// -------------------------------------------------------------------------- //
// Zero initialization

Expr&
zero_initialize(Context& cxt, Decl& d)
{
  return zero_initialize(cxt, cast<Typed_decl>(d));
}


// Select a zero-initialization procedure for variable or constant of 
// fundamental or composite type `t`.
Expr&
zero_initialize(Context& cxt, Typed_decl& d)
{
  lingo_assert(!is<Function_decl>(d));

  Type& t = d.type();

  // No initialization is performed for reference types.
  if (d.is_reference())
    return cxt.make_trivial_init();

  // TODO: Determine the kind of zero that best matches the type (i.e., 
  // produce an appropriate literal). This could be done by conversion.
  if (is_scalar_type(t))
    return cxt.make_copy_init(cxt.get_zero(t));

  // TODO: Initialize aggregate types recursively.

  throw Internal_error("cannot zero initialize type");
}


// -------------------------------------------------------------------------- //
// Default initialization

// Select a default initialization procedure for an variable or constant
// of type `t`.
//
// A reference declaration shall not be default initialized.
Expr&
default_initialize(Context& cxt, Typed_decl& d)
{
  lingo_assert(!is<Function_decl>(d));

  if (d.is_reference()) {
    error(cxt, "default initialization of a reference");
    throw Type_error(); // FIXME: Initialization error.
  }

  // TODO: Select an appropriate constructor for user-defined types.
  //
  // TODO: Select a default initialization procedure for aggregates.

  // Otherwise, no initialization is performed.
  return cxt.make_trivial_init();
}

Expr&
default_initialize(Context& cxt, Decl& d)
{
  return default_initialize(cxt, cast<Typed_decl>(d));
}


// -------------------------------------------------------------------------- //
// Value initialization


// Select a procedure to value-initialize an object.
Expr&
value_initialize(Context& cxt, Typed_decl& d)
{
  lingo_assert(!is<Function_decl>(d));

  // A reference cannot be value initialized.
  if (d.is_reference()) {
    error(cxt, "cannot value-initialize a reference declaration");
    throw Type_error(); // FIXME: Initialization error.
  }

  // FIXME: Implement other cases for value initialization.

  // Are we sure that there are no other categories of types?
  return zero_initialize(cxt, d);
}


Expr&
value_initialize(Context& cxt, Decl& d)
{
  return value_initialize(cxt, cast<Typed_decl>(d));
}

// -------------------------------------------------------------------------- //
// Copy initialization

// Select a procedure to copy initialize a variable or constant of type
// `t` by an expression `e`. This corresponds to the initialization of
// a variable by the syntax:
//
//    T x = e;
//
// This also applies in parameter passing, function return, exception
// throwing, and handling.
//
// TODO: If either t or e is dependent, then we should perform dependent
// initialization -- that needs to be the first.
Expr&
copy_initialize(Context& cxt, Typed_decl& d, Expr& e)
{
  // TODO: If either d or e is dependent, d is dependent-initialized.

  // If e is a tuple, the object is tuple-initialized. 
  if (Tuple_expr* tup = as<Tuple_expr>(&e))
    return tuple_initialize(cxt, d, *tup);

  // If the declaration is a reference, it is reference-initialized.
  if (d.is_reference())
    return reference_initialize(cxt, d, e);

  Type& t = d.type();

  // TODO: If e has class type, search for a user-defined conversion
  // to the type of d.

  // TODO: If d has class type, search for a constructor that accepts e.

  // If all else fails, search for a standard conversion to a value
  // of type t.
  //
  // TODO: Catch exceptions and restructure the error with
  // the conversion error as an explanation.
  Expr& c = standard_conversion_to_value(cxt, e, t);
  return cxt.make_copy_init(c);
}


Expr&
copy_initialize(Context& cxt, Decl& d, Expr& e)
{
  return copy_initialize(cxt, cast<Typed_decl>(d), e);
}

// -------------------------------------------------------------------------- //
// Direct initialization

// Select a procedure to direct-initialize an object or reference of
// type `t` by a paren-enclosed list of expressions `es`. This corresponds
// to the initialization of a variable by the syntax:
//
//    T x(e1, e2, ..., en);
//
// When the list of expressions is empty, this selects value
// initialization.
//
// This also applies in new expressions, etc.
Expr&
direct_initialize(Context& cxt, Typed_decl& d, Expr_list& es)
{
  Type& t = d.type();

  // Arrays must be copy or list-initialized.
  //
  // FIXME: Provide a better diagnostic.
  if (is_array_type(t))
    throw Translation_error("invalid array initialization");

  // If no initializers are given, corresponding to () or {}, the object 
  // is value initialized.
  if (es.empty())
    return value_initialize(cxt, d);

  Expr& e = es.front();

  // If the destination type is a T&, then perform reference
  // initialization on the only element in the list of expressions.
  //
  // FIXME: Shouldn't I have an error if I try to do this;
  //
  //    ref x : T = { a, b, c };
  //
  // Unless there's some kind of magical binding that can happen.
  if (d.is_reference())
    return reference_initialize(cxt, d, e);

  // Otherwise, If the target type is dependent, perform dependent
  // conversions.
  //
  // FIXME: Why does this result in copy initialization? Also, shouldn't
  // this be first in order?
  if (is_dependent_type(t)) {
    Expr& c = dependent_conversion(cxt, e, t);
    return cxt.make_copy_init(c);
  }

  // If the initializer has a source type, then try to find a
  // user-defined conversion from s to the destination type, which
  // should be a (possibly qualified) fundamental type.
  Type& s = e.type();

  // Otherwise, If the target type is dependent, perform dependent
  // conversions.
  //
  // FIXME: Should be covered by the dependent check above?
  if (is_dependent_type(s)) {
    Expr& c = dependent_conversion(cxt, e, t);
    return cxt.make_copy_init(c);
  }

  // If all else fails, try a a standard conversion. This should be
  // the case that we have a non-class, fundamental type.
  //
  // TODO: Catch exceptions and restructure the error with
  // the conversion error as an explanation.
  Expr& c = standard_conversion_to_value(cxt, e, t);
  return cxt.make_copy_init(c);
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
  return is_equivalent(t1, t2);
}


// Two types q1-t1 and q2-t2 are reference compatible if t1 is
// reference-related to t2, and q1 is a superset of q2 (i.e.,
// t1 is as qualified as or more qualified than t2).
//
// TODO: Check for ambiguous base classes.
bool
is_reference_compatible(Type const& t1, Type const& t2)
{
  if (is_reference_related(t1, t2)) {
    Qualifier_set q1 = t1.qualifiers();
    Qualifier_set q2 = t2.qualifiers();
    return is_superset(q1, q2);
  }
  return false;
}


// Select an initialization of the reference type `d` by an expression
// `e`.
//
// FIXME: The current design effectively obviates the reference compatibility
// rules in C++. We need to rethink how that works. The simplest rule is:
// like references to objects of equivalent type bind. However, we probably
// need to account for qualifiers. I wonder if that can reduce to a standard
// conversion.
Expr&
reference_initialize(Context& cxt, Typed_decl& d, Expr& e)
{
  // TODO: If is a value expression, then we probably need to do some
  // kin do lifetime extension.

  // FIXME: The conversion is almost certainly incorrect.
  if (d.is_normal_reference() && e.type().is_reference()) {
      Expr& c = standard_conversion(cxt, e, d.type());
      return cxt.make_bind_init(c);
  }

  // TODO: Handle bindings to temporaries.

  throw Type_error("reference binding");
}


Expr&
reference_initialize(Context& cxt, Decl& d, Expr& e)
{
  return reference_initialize(cxt, cast<Typed_decl>(d), e);
}


// -------------------------------------------------------------------------- //
// Aggregate initialization

Expr&
aggregate_initialize(Context& cxt, Type& t, Expr_list& i)
{
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Tuple initialization

// Diagnose the initialization error.
static Expr&
tuple_init_error(Context& cxt, Type& t, Expr& e)
{
  error(cxt, "cannot initialize an object of type '{}' with '{}'", e, t);
  throw Type_error();
}


// Initialize a tuple-typed object with a tuple-expression.
//
// TODO: This is really a form of aggregate initialization. We want to
// initialize each object of a target tuple.
static Expr&
tuple_init(Context& cxt, Tuple_type& t, Tuple_expr& e)
{
  // TODO: Initialize each element type of t with the corresponding
  // element of e.
  // if (is_equivalent(t, e.type()))
  //   return cxt.make_copy_init(e);

  error(cxt, "cannot initialize an object of type '{}' with '{}'", e, t);
  throw Type_error();
}


// Initialize an array-typed object with a tuple-expression.
//
// TODO: This is really a form of aggregate initialization. We want to
// initialize each object of a target tuple.
//
// TODO: Implement the type checking.
static Expr&
tuple_init(Context& cxt, Array_type& t, Tuple_expr& e)
{
  lingo_unreachable();
}


// FIXME: I am currently assuming that all classes are aggregates. This
// is clearly not correct. For non-aggregates, we should search for a
// constructor.
static Expr&
tuple_init(Context& cxt, Class_type& t, Tuple_expr& e)
{
  Decl_list& mem = t.declaration().objects();
  Expr_list& els = e.elements();
  
  // Check for argument size mismatch.
  //
  // TODO: Can aggregates have default member initializers? How does that
  // work here?
  if (mem.size() < els.size()) {
    error(cxt, "cannot initialize an object of type '{}' with '{}'", e, t);
    note ("initializer has too few elements");
    throw Type_error();
  }
  if (mem.size() > els.size()) {
    error(cxt, "cannot initialize an object of type '{}' with '{}'", e, t);
    note ("initializer has too few elements");
    throw Type_error();
  }

  Expr_list inits;
  for (std::size_t i = 0; i < mem.size(); ++i) {
    Decl& var = *mem[i];
    Expr& elem = *els[i];

    // FIXME: This is probably just normal initialization.
    Expr& init = copy_initialize(cxt, var, elem);
    inits.push_back(init);
  }

  return cxt.make_aggregate_init(t, std::move(inits));
}


// Tuple compared with tuple or array
Expr&
tuple_initialize(Context& cxt, Typed_decl& d, Tuple_expr& e)
{
  struct fn
  {
    Context&    cxt;
    Tuple_expr& e;
    Expr& operator()(Type& t)       { return tuple_init_error(cxt, t, e); }
    Expr& operator()(Tuple_type& t) { return tuple_init(cxt, t, e); }
    Expr& operator()(Array_type& t) { return tuple_init(cxt, t, e); }
    Expr& operator()(Class_type& t) { return tuple_init(cxt, t, e); }
  };
  return apply(d.type(), fn{cxt, e});
}


Expr&
tuple_initialize(Context& cxt, Decl& d, Tuple_expr& e)
{
  return tuple_initialize(cxt, cast<Typed_decl>(d), e);
}



} // namespace banjo
