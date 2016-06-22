// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "equivalence.hpp"
#include "ast.hpp"

#include <typeindex>


namespace banjo
{


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
  lingo_unhandled(x1);
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
is_equivalent(Placeholder_id const& n1, Placeholder_id const& n2)
{
  return n1.number() == n2.number();
}


// they are operator-function-ids formed with the same operator
inline bool
eq_operator_id(Operator_id const& n1, Operator_id const& n2)
{
  return n1.kind() == n2.kind();
}


inline bool
eq_conversion_id(Conversion_id const& n1, Conversion_id const& n2)
{
  return is_equivalent(n1.type(), n2.type());
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
    bool operator()(Global_id const& n1)      { lingo_unreachable(); }
    bool operator()(Placeholder_id const& n1) { return is_equivalent(n1, cast<Placeholder_id>(n2)); }
    bool operator()(Operator_id const& n1)    { return eq_operator_id(n1, cast<Operator_id>(n2)); }
    bool operator()(Conversion_id const& n1)  { return eq_conversion_id(n1, cast<Conversion_id>(n2)); }
    bool operator()(Literal_id const& n1)     { return is_equivalent(n1, cast<Literal_id>(n2)); }
    bool operator()(Destructor_id const& n1)  { return is_equivalent(n1, cast<Destructor_id>(n2)); }
    bool operator()(Template_id const& n1)    { return is_equivalent(n1, cast<Template_id>(n2)); }
    bool operator()(Concept_id const& n1)     { return is_equivalent(n1, cast<Concept_id>(n2)); }
    bool operator()(Qualified_id const& n1)   { return is_equivalent(n1, cast<Qualified_id>(n2)); }
  };

  // The same objects represent the same types.
  if (&n1 == &n2)
    return true;

  // Names of different kinds are not the same.
  std::type_index ti1 = typeid(n1);
  std::type_index ti2 = typeid(n2);
  if (ti1 != ti2)
    return false;

  // Find a comparison of the types.
  return apply(n1, fn{n2});
}


// -------------------------------------------------------------------------- //
// Types

inline bool
eq_integer_type(Integer_type const& t1, Integer_type const& t2)
{
  return t1.precision() == t2.precision() && t1.sign() == t2.sign();
}


inline bool
eq_float_type(Float_type const& t1, Float_type const& t2)
{
  return t1.precision() == t2.precision();
}


inline bool
eq_function_type(Function_type const& t1, Function_type const& t2)
{
  return is_equivalent(t1.parameter_types(), t2.parameter_types())
      && is_equivalent(t1.return_type(), t2.return_type());
}


inline bool
eq_array_type(Array_type const& t1, Array_type const& t2)
{
  return is_equivalent(t1.element_type(), t2.element_type())
      && is_equivalent(t1.extent(), t2.extent());
}


inline bool
eq_tuple_type(Tuple_type const& t1, Tuple_type const& t2)
{
  return is_equivalent(t1.element_types(), t2.element_types());
}


inline bool
eq_pointer_type(Pointer_type const& t1, Pointer_type const& t2)
{
  return is_equivalent(t1.type(), t2.type());
}


inline bool
eq_declared_type(Declared_type const& t1, Declared_type const& t2)
{
  return is_equivalent(t1.declaration(), t2.declaration());
}


// Returns true if the types a and b are equivalent.
bool
is_equivalent(Type const& t1, Type const& t2)
{
  struct fn
  {
    Type const& t2;
    bool operator()(Type const& t)           { lingo_unhandled(t); }
    bool operator()(Void_type const& t1)     { return true; }
    bool operator()(Boolean_type const& t1)  { return true; }
    bool operator()(Byte_type const& t1)     { return true; }
    bool operator()(Integer_type const& t1)  { return eq_integer_type(t1, cast_as(t1, t2)); }
    bool operator()(Float_type const& t1)    { return eq_float_type(t1, cast_as(t1, t2)); }
    bool operator()(Function_type const& t1) { return eq_function_type(t1, cast_as(t1, t2)); }
    bool operator()(Array_type const& t1)    { return eq_array_type(t1, cast_as(t1, t2)); }
    bool operator()(Tuple_type const& t1)    { return eq_tuple_type(t1, cast_as(t1, t2)); }
    bool operator()(Pointer_type const& t1)  { return eq_pointer_type(t1, cast_as(t1, t2)); }
    bool operator()(Declared_type const& t1) { return eq_declared_type(t1, cast_as(t1, t2)); }
  };

  // Identical types are equivalent.  
  if (&t1 == &t2)
    return true;

  // Types describing different entities are not the same.
  if (t1.category() != t2.category())
    return false;

  // Types with different qualifications are not equivalent.
  if (t1.qualifiers() != t2.qualifiers())
    return false;

  // Types of different kinds are not the same.
  std::type_index ti1 = typeid(t1);
  std::type_index ti2 = typeid(t2);
  if (ti1 != ti2)
    return false;

  return apply(t1, fn{t2});
}


// -------------------------------------------------------------------------- //
// Expressions

template<typename T>
bool
eq_literal_expr(Literal_expr<T> const& e1, Literal_expr<T> const& e2)
{
  return e1.value() == e2.value();
}


// Two (resolved) id-expressions are equivalent when they refer to the
// same declaration.
bool
eq_decl_ref(Decl_ref const& e1, Decl_ref const& e2)
{
  return is_equivalent(e1.declaration(), e2.declaration());
}


// Two (resolved) member-expressions are equivalent when they refer to the
// same declaration.
bool
eq_member_ref(Member_ref const& e1, Member_ref const& e2)
{
  return is_equivalent(e1.declaration(), e2.declaration());
}


bool
eq_unary_expr(Unary_expr const& e1, Unary_expr const& e2)
{
  return is_equivalent(e1.operand(), e2.operand());
}


bool
eq_binary_expr(Binary_expr const& e1, Binary_expr const& e2)
{
  return is_equivalent(e1.left(), e2.left())
      && is_equivalent(e1.right(), e2.right());
}


bool
eq_call_expr(Call_expr const& e1, Call_expr const& e2)
{
  return is_equivalent(e1.function(), e2.function())
      && is_equivalent(e1.arguments(), e2.arguments());
}


bool
eq_conv(Conv const& e1, Conv const& e2)
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
    bool operator()(Expr const& e) const          { lingo_unhandled(e); }
    bool operator()(Boolean_expr const& e1) const { return eq_literal_expr(e1, cast_as(e1, e2)); }
    bool operator()(Integer_expr const& e1) const { return eq_literal_expr(e1, cast_as(e1, e2)); }
    bool operator()(Decl_ref const& e1) const     { return eq_decl_ref(e1, cast_as(e1, e2)); }
    bool operator()(Member_ref const& e1) const   { return eq_member_ref(e1, cast_as(e1, e2)); }
    bool operator()(Unary_expr const& e1) const   { return eq_unary_expr(e1, cast_as(e1, e2)); }
    bool operator()(Binary_expr const& e1) const  { return eq_binary_expr(e1, cast_as(e1, e2)); }
    bool operator()(Call_expr const& e1) const    { return eq_call_expr(e1, cast_as(e1, e2)); }
    bool operator()(Conv const& e1) const         { return eq_conv(e1, cast_as(e1, e2)); }
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
    bool operator()(Cons const& c1) const               { lingo_unhandled(c1); }
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
