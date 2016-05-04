// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-eq.hpp"
#include "ast.hpp"

#include <typeindex>


namespace banjo
{


// Returns true.
template<typename T>
bool
always_equal(T const& t1, T const& t2)
{
  return true;
}


// -------------------------------------------------------------------------- //
// Terms

bool
is_equivalent(Term const& x1, Term const& x2)
{
  // The same objects represent the same types.
  if (&x1 == &x2)
    return true;

  // Types of different kinds are not the same.
  std::type_index ti1 = typeid(x1);
  std::type_index ti2 = typeid(x2);
  if (ti1 != ti2)
    return false;

  if (Type const* t1 = as<Type>(&x1))
    return is_equivalent(*t1, cast<Type>(x2));
  if (Expr const* t1 = as<Expr>(&x1))
    return is_equivalent(*t1, cast<Expr>(x2));
  if (Decl const* t1 = as<Decl>(&x1))
    return is_equivalent(*t1, cast<Decl>(x2));
  banjo_unhandled_case(x1);
}



// -------------------------------------------------------------------------- //
// Names

// Two simple-ids are equal iff they have the same spelling. This is the
// case when the underlying symbols are the same.
inline bool
is_equivalent(Simple_id const& n1, Simple_id const& n2)
{
  return &n1.symbol() == &n2.symbol();
}


inline bool
is_equivalent(Global_id const& n1, Global_id const& n2)
{
  lingo_unreachable();
}


inline bool
is_equivalent(Placeholder_id const& n1, Placeholder_id const& n2)
{
  return n1.number() == n2.number();
}

// they are operator-function-ids formed with the same operator
inline bool
is_equivalent(Operator_id const& n1, Operator_id const& n2)
{
  return n1.kind() == n2.kind();
}


inline bool
is_equivalent(Conversion_id const& n1, Conversion_id const& n2)
{
  lingo_unreachable();
}


inline bool
is_equivalent(Literal_id const& n1, Literal_id const& n2)
{
  lingo_unreachable();
}


inline bool
is_equivalent(Destructor_id const& n1, Destructor_id const& n2)
{
  lingo_unreachable();
}


inline bool
is_equivalent(Template_id const& n1, Template_id const& n2)
{
  lingo_unreachable();
}


inline bool
is_equivalent(Concept_id const& n1, Concept_id const& n2)
{
  lingo_unreachable();
}


inline bool
is_equivalent(Qualified_id const& n1, Qualified_id const& n2)
{
  lingo_unreachable();
}


bool
is_equivalent(Name const& n1, Name const& n2)
{
  struct fn
  {
    Name const& n2;
    bool operator()(Simple_id const& n1)      { return is_equivalent(n1, cast<Simple_id>(n2)); }
    bool operator()(Global_id const& n1)      { return is_equivalent(n1, cast<Global_id>(n2)); }
    bool operator()(Placeholder_id const& n1) { return is_equivalent(n1, cast<Placeholder_id>(n2)); }
    bool operator()(Operator_id const& n1)    { return is_equivalent(n1, cast<Operator_id>(n2)); }
    bool operator()(Conversion_id const& n1)  { return is_equivalent(n1, cast<Conversion_id>(n2)); }
    bool operator()(Literal_id const& n1)     { return is_equivalent(n1, cast<Literal_id>(n2)); }
    bool operator()(Destructor_id const& n1)  { return is_equivalent(n1, cast<Destructor_id>(n2)); }
    bool operator()(Template_id const& n1)    { return is_equivalent(n1, cast<Template_id>(n2)); }
    bool operator()(Concept_id const& n1)     { return is_equivalent(n1, cast<Concept_id>(n2)); }
    bool operator()(Qualified_id const& n1)   { return is_equivalent(n1, cast<Qualified_id>(n2)); }
  };

  // The same objects represent the same types.
  if (&n1 == &n2)
    return true;

  // Types of different kinds are not the same.
  std::type_index ti1 = typeid(n1);
  std::type_index ti2 = typeid(n2);
  if (ti1 != ti2)
    return false;

  // Find a comparison of the types.
  return apply(n1, fn{n2});
}


// -------------------------------------------------------------------------- //
// Types


bool
is_equivalent(Integer_type const& t1, Integer_type const& t2)
{
  return t1.is_signed() == t2.is_signed() && t1.precision() == t2.precision();
}


bool
is_equivalent(Float_type const& t1, Float_type const& t2)
{
  return t1.precision() == t2.precision();
}


bool
is_equivalent(Function_type const& t1, Function_type const& t2)
{
  return is_equivalent(t1.parameter_types(), t2.parameter_types())
      && is_equivalent(t1.return_type(), t2.return_type());
}


bool
is_equivalent(Qualified_type const& t1, Qualified_type const& t2)
{
  return t1.qualifier() == t2.qualifier() && is_equivalent(t1.type(), t2.type());
}


bool
is_equivalent(Unary_type const& t1, Unary_type const& t2)
{
  return is_equivalent(t1.type(), t2.type());
}


// TODO: The extent of the arrays must be equivalent.
bool
is_equivalent(Array_type const& t1, Array_type const& t2)
{
  return is_equivalent(t1.type(), t2.type());
}


bool
is_equivalent(Tuple_type const& t1, Tuple_type const& t2)
{
  return is_equivalent(t1.type_list(), t2.type_list());
}


bool
is_equivalent(Dynarray_type const& t1, Dynarray_type const& t2)
{
  return is_equivalent(t1.type(),t2.type());
}


// Two declared types are equivalent when they have the same
// declaration.
bool
is_eq_declared_type(Declared_type const& t1, Declared_type const& t2)
{
  return is_equivalent(t1.declaration(), t2.declaration());
}


// Returns true if the types a and b are equivalent.
//
// TODO: Finish implementing this function.
bool
is_equivalent(Type const& t1, Type const& t2)
{
  struct fn
  {
    Type const& t2;
    bool operator()(Type const& t1) const           { lingo_unhandled(t1); }
    bool operator()(Void_type const& t1) const      { return always_equal(t1, cast<Void_type>(t2)); }
    bool operator()(Boolean_type const& t1) const   { return always_equal(t1, cast<Boolean_type>(t2)); }
    bool operator()(Byte_type const& t1) const      { return always_equal(t1, cast<Byte_type>(t2)); }
    bool operator()(Integer_type const& t1) const   { return is_equivalent(t1, cast<Integer_type>(t2)); }
    bool operator()(Float_type const& t1) const     { return is_equivalent(t1, cast<Float_type>(t2)); }
    bool operator()(Function_type const& t1) const  { return is_equivalent(t1, cast<Function_type>(t2)); }
    bool operator()(Qualified_type const& t1) const { return is_equivalent(t1, cast<Qualified_type>(t2)); }
    bool operator()(Unary_type const& t1) const     { return is_equivalent(t1, cast<Unary_type>(t2)); }
    bool operator()(Array_type const& t1) const     { return is_equivalent(t1, cast<Array_type>(t2)); }
    bool operator()(Tuple_type const& t1) const     { return is_equivalent(t1, cast<Tuple_type>(t2)); }
    bool operator()(Dynarray_type const& t1) const  { return is_equivalent(t1, cast<Dynarray_type>(t2)); }
    bool operator()(Declared_type const& t1) const  { return is_eq_declared_type(t1, cast_as(t1, t2)); }
  };

  // The same objects represent the same types.
  if (&t1 == &t2)
    return true;

  // Types of different kinds are not the same.
  std::type_index ti1 = typeid(t1);
  std::type_index ti2 = typeid(t2);
  if (ti1 != ti2)
    return false;

  // Find a comparison of the types.
  return apply(t1, fn{t2});
}


// -------------------------------------------------------------------------- //
// Expressions

template<typename T>
bool
is_equivalent(Literal_expr<T> const& e1, Literal_expr<T> const& e2)
{
  return e1.value() == e2.value();
}


bool
is_equivalent(Decl_expr const& e1, Decl_expr const& e2)
{
  return is_equivalent(e1.declaration(), e2.declaration());
}


bool
is_equivalent(Unary_expr const& e1, Unary_expr const& e2)
{
  return is_equivalent(e1.operand(), e2.operand());
}


bool
is_equivalent(Binary_expr const& e1, Binary_expr const& e2)
{
  return is_equivalent(e1.left(), e2.left())
      && is_equivalent(e1.right(), e2.right());
}


bool
is_equivalent(Call_expr const& e1, Call_expr const& e2)
{
  return is_equivalent(e1.function(), e2.function())
      && is_equivalent(e1.arguments(), e2.arguments());
}


bool
is_equivalent(Conv const& e1, Conv const& e2)
{
  return is_equivalent(e1.destination(), e2.destination())
      && is_equivalent(e1.source(), e2.source());
}


bool
is_equivalent(Expr const& e1, Expr const& e2)
{
  struct fn
  {
    Expr const& e2;
    bool operator()(Expr const& e) const          { banjo_unhandled_case(e); }
    bool operator()(Boolean_expr const& e1) const { return is_equivalent(e1, cast<Boolean_expr>(e2)); }
    bool operator()(Integer_expr const& e1) const { return is_equivalent(e1, cast<Integer_expr>(e2)); }
    bool operator()(Decl_expr const& e1) const    { return is_equivalent(e1, cast<Decl_expr>(e2)); }
    bool operator()(Unary_expr const& e1) const   { return is_equivalent(e1, cast<Unary_expr>(e2)); }
    bool operator()(Binary_expr const& e1) const  { return is_equivalent(e1, cast<Binary_expr>(e2)); }
    bool operator()(Call_expr const& e1) const    { return is_equivalent(e1, cast<Call_expr>(e2)); }

    bool operator()(Conv const& e1) const         { return is_equivalent(e1, cast<Conv>(e2)); }
  };

  // The same objects represent the same types.
  if (&e1 == &e2)
    return true;

  // Types of different kinds are not the same.
  std::type_index ti1 = typeid(e1);
  std::type_index ti2 = typeid(e2);
  if (ti1 != ti2)
    return false;

  // Delegate to specific rules.
  return apply(e1, fn{e2});
}


// -------------------------------------------------------------------------- //
// Declarations
//
// Two declarations are equivalent when they declare the same
// entity.
//
// TODO: When we allow redeclaration, then this comparison must be
// use the entity, not object identity.


// Two template parameters are equivalent when they have equal
// indexes (depth, offset). For example:
//
//    template<typename T> void f(T);
//    template<typename U> void f(U*);
//
// Here, T and U are equivalent.
inline bool
is_equivalent(Type_parm const& d1, Type_parm const& d2)
{
  return d1.index() == d2.index();
}


// FIXME: This is wrong.
bool
is_equivalent(Decl const& a, Decl const& b)
{
  struct fn
  {
    Decl const& d2;
    bool operator()(Decl const& d1) const      { return &d1 == &d2; }
    bool operator()(Type_parm const& d1) const { return is_equivalent(d1, cast<Type_parm>(d2)); }
  };
  return &a == &b;
}


// -------------------------------------------------------------------------- //
// Constraints

// Two unexpanded constraints are equivalent when they refer to
// the same declaration and have the same template arguments.
bool
is_equivalent(Concept_cons const& c1, Concept_cons const& c2)
{
  return is_equivalent(c1.declaration(), c2.declaration())
      && is_equivalent(c1.arguments(), c2.arguments());
}


bool
is_equivalent(Predicate_cons const& c1, Predicate_cons const& c2)
{
  return is_equivalent(c1.expression(), c2.expression());
}


template<typename T>
bool
is_eq_usage(T const& c1, T const& c2)
{
  return is_equivalent(c1.expression(), c2.expression())
      && is_equivalent(c1.type(), c2.type());
}


// FIXME: Also compare template parameters?
bool
is_eq_parm(Parameterized_cons const& c1, Parameterized_cons const& c2)
{
  return is_equivalent(c1.variables(), c2.variables())
      && is_equivalent(c1.constraint(), c2.constraint());
}


bool
is_equivalent(Binary_cons const& c1, Binary_cons const& c2)
{
  return is_equivalent(c1.left(), c2.left())
      && is_equivalent(c1.right(), c2.right());
}


bool
is_equivalent(Cons const& c1, Cons const& c2)
{
  struct fn
  {
    Cons const& c2;
    bool operator()(Cons const& c1) const               { banjo_unhandled_case(c1); }
    bool operator()(Concept_cons const& c1) const       { return is_equivalent(c1, cast<Concept_cons>(c2)); }
    bool operator()(Predicate_cons const& c1) const     { return is_equivalent(c1, cast<Predicate_cons>(c2)); }
    bool operator()(Expression_cons const& c1) const    { return is_eq_usage(c1, cast<Expression_cons>(c2)); }
    bool operator()(Conversion_cons const& c1) const    { return is_eq_usage(c1, cast<Conversion_cons>(c2)); }
    bool operator()(Parameterized_cons const& c1) const { return is_eq_parm(c1, cast<Parameterized_cons>(c2)); }
    bool operator()(Binary_cons const& c1) const        { return is_equivalent(c1, cast<Binary_cons>(c2)); }
  };

  // The same objects represent the same types.
  if (&c1 == &c2)
    return true;

  // Types of different kinds are not the same.
  std::type_index ti1 = typeid(c1);
  std::type_index ti2 = typeid(c2);
  if (ti1 != ti2)
    return false;

  // Delegate to specific rules.
  return apply(c1, fn{c2});
}


} // namespace banjo
