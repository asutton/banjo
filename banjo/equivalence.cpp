// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "equivalence.hpp"
#include "ast.hpp"

#include <typeindex>


namespace banjo
{


template<typename T>
inline bool
is_equivalent(List<T> const& a, List<T> const& b)
{
  auto cmp = [](T const& a, T const& b) {
    return is_equivalent(a, b);
  };
  return std::equal(a.begin(), a.end(), b.begin(), b.end(), cmp);
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
  lingo_unimplemented();
}


inline bool
is_equivalent(Placeholder_id const& n1, Placeholder_id const& n2)
{
  lingo_unimplemented();
}


inline bool
is_equivalent(Operator_id const& n1, Operator_id const& n2)
{
  lingo_unimplemented();
}


inline bool
is_equivalent(Conversion_id const& n1, Conversion_id const& n2)
{
  lingo_unimplemented();
}


inline bool
is_equivalent(Literal_id const& n1, Literal_id const& n2)
{
  lingo_unimplemented();
}


inline bool
is_equivalent(Destructor_id const& n1, Destructor_id const& n2)
{
  lingo_unimplemented();
}


inline bool
is_equivalent(Template_id const& n1, Template_id const& n2)
{
  lingo_unimplemented();
}


inline bool
is_equivalent(Concept_id const& n1, Concept_id const& n2)
{
  lingo_unimplemented();
}


inline bool
is_equivalent(Qualified_id const& n1, Qualified_id const& n2)
{
  lingo_unimplemented();
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
is_equivalent(Void_type const&, Void_type const&)
{
  return true;
}


bool
is_equivalent(Boolean_type const&, Boolean_type const&)
{
  return true;
}


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


// TODO: When are two placeholder types equivalent?
bool
is_equivalent(Auto_type const&, Auto_type const&)
{
  return false;
}


bool
is_equivalent(Decltype_type const& a, Decltype_type const& b)
{
  lingo_unimplemented();
}


// TODO: When are placeholder types equivalent?
bool
is_equivalent(Declauto_type const& t1, Declauto_type const& t2)
{
  return false;
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
is_equivalent(Reference_type const& t1, Reference_type const& t2)
{
  return is_equivalent(t1.type(), t2.type());
}


bool
is_equivalent(Pointer_type const& t1, Pointer_type const& t2)
{
  return is_equivalent(t1.type(), t2.type());
}


// TODO: The extent of the arrays must be equivalent.
bool
is_equivalent(Array_type const&, Array_type const&)
{
  lingo_unimplemented();
}


bool
is_equivalent(Sequence_type const& t1, Sequence_type const& t2)
{
  return is_equivalent(t1.type(), t2.type());
}


// TODO: This isn't quite right. It depends on declaration chains.
bool
is_equivalent(User_defined_type const& t1, User_defined_type const& t2)
{
  return &t1.declaration() == &t2.declaration();
}


// Two synthetic types are equivalent only when they are identical.
// Here, t1 and t2 are known not to be identical.
inline bool
is_equivalent(Synthetic_type const& t1, Synthetic_type const& t2)
{
  return false;
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
    bool operator()(Void_type const& t1) const         { return is_equivalent(t1, cast<Void_type>(t2)); }
    bool operator()(Boolean_type const& t1) const      { return is_equivalent(t1, cast<Boolean_type>(t2)); }
    bool operator()(Integer_type const& t1) const      { return is_equivalent(t1, cast<Integer_type>(t2)); }
    bool operator()(Float_type const& t1) const        { return is_equivalent(t1, cast<Float_type>(t2)); }
    bool operator()(Auto_type const& t1) const         { return is_equivalent(t1, cast<Auto_type>(t2)); }
    bool operator()(Decltype_type const& t1) const     { return is_equivalent(t1, cast<Decltype_type>(t2)); }
    bool operator()(Declauto_type const& t1) const     { return is_equivalent(t1, cast<Declauto_type>(t2)); }
    bool operator()(Function_type const& t1) const     { return is_equivalent(t1, cast<Function_type>(t2)); }
    bool operator()(Qualified_type const& t1) const    { return is_equivalent(t1, cast<Qualified_type>(t2)); }
    bool operator()(Reference_type const& t1) const    { return is_equivalent(t1, cast<Reference_type>(t2)); }
    bool operator()(Pointer_type const& t1) const      { return is_equivalent(t1, cast<Pointer_type>(t2)); }
    bool operator()(Array_type const& t1) const        { return is_equivalent(t1, cast<Array_type>(t2)); }
    bool operator()(Sequence_type const& t1) const     { return is_equivalent(t1, cast<Sequence_type>(t2)); }
    bool operator()(User_defined_type const& t1) const { return is_equivalent(t1, cast<User_defined_type>(t2)); }
    bool operator()(Typename_type const& t1) const     { lingo_unimplemented(); }
    bool operator()(Synthetic_type const& t1) const    { return is_equivalent(t1, cast<Synthetic_type>(t2)); }
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

bool
is_equivalent(Boolean_expr const& e1, Boolean_expr const& e2)
{
  return e1.value() == e2.value();
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
is_equivalent(Expr const& e1, Expr const& e2)
{
  struct fn
  {
    Expr const& e2;
    bool operator()(Expr const&) const            { lingo_unimplemented(); }
    bool operator()(Boolean_expr const& e1) const { return is_equivalent(e1, cast<Boolean_expr>(e2)); }
    bool operator()(Unary_expr const& e1) const   { return is_equivalent(e1, cast<Unary_expr>(e2)); }
    bool operator()(Binary_expr const& e1) const  { return is_equivalent(e1, cast<Binary_expr>(e2)); }
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
// Constraints

bool
is_equivalent(Predicate_cons const& c1, Predicate_cons const& c2)
{
  return is_equivalent(c1.expression(), c2.expression());
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
    bool operator()(Cons const& c1) const           { lingo_unimplemented(); }
    bool operator()(Predicate_cons const& c1) const { return is_equivalent(c1, cast<Predicate_cons>(c2)); }
    bool operator()(Binary_cons const& c1) const    { return is_equivalent(c1, cast<Binary_cons>(c2)); }
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
