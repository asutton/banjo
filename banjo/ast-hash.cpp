// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast-hash.hpp"
#include "ast.hpp"

#include <typeinfo>


namespace banjo
{

// Returns an initial hash value based on the dynamic type of T.
template<typename T>
std::size_t hash_type(T const& t)
{
  return typeid(t).hash_code();
}


// -------------------------------------------------------------------------- //
// Terms


std::size_t
hash_value(Term const& x)
{
  if (Name const* n = as<Name>(&x))
    return hash_value(*n);
  if (Type const* t = as<Type>(&x))
    return hash_value(*t);
  if (Expr const* e = as<Expr>(&x))
    return hash_value(*e);
  if (Decl const* d = as<Decl>(&x))
    return hash_value(*d);
  lingo_unreachable();
}

// -------------------------------------------------------------------------- //
// Names

inline std::size_t
hash_value(Simple_id const& n)
{
  std::size_t h = hash_type(n);
  boost::hash_combine(h, &n.symbol());
  return h;
}


inline std::size_t
hash_value(Global_id const& n)
{
  return hash_type(n);
}


inline std::size_t
hash_value(Placeholder_id const& n)
{
  std::size_t h = hash_type(n);
  boost::hash_combine(h, n.number());
  return h;
}


inline std::size_t
hash_value(Operator_id const& n)
{
  std::size_t h = hash_type(n);
  boost::hash_combine(h, n.kind());
  return h;
}


inline std::size_t
hash_value(Conversion_id const&)
{
  lingo_unreachable();
}


inline std::size_t
hash_value(Literal_id const&)
{
  lingo_unreachable();
}


inline std::size_t
hash_value(Destructor_id const&)
{
  lingo_unreachable();
}


inline std::size_t
hash_value(Template_id const&)
{
  lingo_unreachable();
}


inline std::size_t
hash_value(Concept_id const&)
{
  lingo_unreachable();
}


inline std::size_t
hash_value(Qualified_id const&)
{
  lingo_unreachable();
}


std::size_t
hash_value(Name const& n)
{
  struct fn
  {
    std::size_t operator()(Simple_id const& n)      { return hash_value(n); }
    std::size_t operator()(Global_id const& n)      { return hash_value(n); }
    std::size_t operator()(Placeholder_id const& n) { return hash_value(n); }
    std::size_t operator()(Operator_id const& n)    { return hash_value(n); }
    std::size_t operator()(Conversion_id const& n)  { return hash_value(n); }
    std::size_t operator()(Literal_id const& n)     { return hash_value(n); }
    std::size_t operator()(Destructor_id const& n)  { return hash_value(n); }
    std::size_t operator()(Template_id const& n)    { return hash_value(n); }
    std::size_t operator()(Concept_id const& n)     { return hash_value(n); }
    std::size_t operator()(Qualified_id const& n)   { return hash_value(n); }
  };
  return apply(n, fn{});
}


// -------------------------------------------------------------------------- //
// Types

template<typename T>
inline std::size_t
hash_nullary_type(T const& t)
{
  return hash_type(t);
}


inline std::size_t
hash_integer(Integer_type const& t)
{
  std::size_t h = hash_type(t);
  boost::hash_combine(h, t.sign());
  boost::hash_combine(h, t.precision());
  return h;
}

inline std::size_t
hash_float(Float_type const& t)
{
  std::size_t h = hash_type(t);
  boost::hash_combine(h, t.precision());
  return h;
}


inline std::size_t
hash_function_type(Function_type const& t)
{
  std::size_t h = hash_type(t);
  boost::hash_combine(h, t.parameter_types());
  boost::hash_combine(h, t.return_type());
  return h;
}


// The hash value of a user-defined type is that of its declaration.
inline std::size_t
hash_declared_type(Declared_type const& t)
{
  std::size_t h = hash_type(t);
  boost::hash_combine(h, t.declaration());
  return h;
}


// Compute the hash value of a type.
std::size_t
hash_value(Type const& t)
{
  struct fn
  {
    std::size_t operator()(Type const& t) const           { lingo_unhandled(t); }
    std::size_t operator()(Void_type const& t) const      { return hash_nullary_type(t); }
    std::size_t operator()(Boolean_type const& t) const   { return hash_nullary_type(t); }
    std::size_t operator()(Byte_type const& t) const      { return hash_nullary_type(t); }
    std::size_t operator()(Integer_type const& t) const   { return hash_integer(t); }
    std::size_t operator()(Float_type const& t) const     { return hash_float(t); }
    std::size_t operator()(Function_type const& t) const  { return hash_function_type(t); }
    std::size_t operator()(Class_type const& t) const     { return hash_declared_type(t); }
  };
  return apply(t, fn{});
}


// -------------------------------------------------------------------------- //
// Expressions

template<typename T>
std::size_t
hash_value(Literal_expr<T> const& e)
{
  std::size_t h = hash_type(e);
  boost::hash_combine(h, e.value());
  return h;
}


std::size_t
hash_value(Id_expr const& e)
{
  std::size_t h = hash_type(e);
  boost::hash_combine(h, e.id());
  return h;
}


std::size_t
hash_value(Decl_expr const& e)
{
  std::size_t h = hash_type(e);
  boost::hash_combine(h, e.declaration());
  return h;
}


std::size_t
hash_value(Unary_expr const& e)
{
  std::size_t h = hash_type(e);
  boost::hash_combine(h, e.operand());
  return h;
}


std::size_t
hash_value(Binary_expr const& e)
{
  std::size_t h = hash_type(e);
  boost::hash_combine(h, e.left());
  boost::hash_combine(h, e.right());
  return h;
}


std::size_t
hash_value(Call_expr const& e)
{
  std::size_t h = hash_type(e);
  boost::hash_combine(h, e.function());
  boost::hash_combine(h, e.arguments());
  return h;
}


std::size_t
hash_conv(Conv const& e)
{
  std::size_t h = hash_type(e);
  boost::hash_combine(h, e.destination());
  boost::hash_combine(h, e.source());
  return h;
}


std::size_t
hash_value(Expr const& e)
{
  struct fn
  {
    std::size_t operator()(Expr const& e) const           { banjo_unhandled_case(e); }
    std::size_t operator()(Boolean_expr const& e) const   { return hash_value(e); }
    std::size_t operator()(Integer_expr const& e) const   { return hash_value(e); }
    std::size_t operator()(Id_expr const& e) const        { return hash_value(e); }
    std::size_t operator()(Decl_expr const& e) const      { return hash_value(e); }
    std::size_t operator()(Unary_expr const& e) const     { return hash_value(e); }
    std::size_t operator()(Binary_expr const& e) const    { return hash_value(e); }
    std::size_t operator()(Call_expr const& e) const      { return hash_value(e); }

    std::size_t operator()(Conv const& e) const           { return hash_conv(e); }
  };
  return apply(e, fn{});
}


// -------------------------------------------------------------------------- //
// Declartions

// Compute the hash value of a declaration. Because declarations
// are unique, the hash is derived from the identity of the declaration.

std::size_t
hash_decl(Decl const& d)
{
  std::hash<Decl const*> h;
  return h(&d);
}


std::size_t
hash_parm(Type_parm const& d)
{
  std::size_t h = hash_type(d);
  boost::hash_combine(h, d.index());
  return h;
}


std::size_t
hash_value(Decl const& d)
{
  struct fn
  {
    std::size_t operator()(Decl const& d)      { return hash_decl(d); }
    std::size_t operator()(Type_parm const& d) { return hash_parm(d); }
  };
  return apply(d, fn{});
}


// -------------------------------------------------------------------------- //
// Constraints

std::size_t
hash_value(Concept_cons const& c)
{
  std::size_t h = hash_type(c);
  boost::hash_combine(h, c.declaration());
  boost::hash_combine(h, c.arguments());
  return h;
}


std::size_t
hash_value(Predicate_cons const& c)
{
  std::size_t h = hash_type(c);
  boost::hash_combine(h, c.expression());
  return h;
}


template<typename T>
std::size_t
hash_usage(T const& c)
{
  std::size_t h = hash_type(c);
  boost::hash_combine(h, c.expression());
  boost::hash_combine(h, c.type());
  return h;
}


std::size_t
hash_parm(Parameterized_cons const& c)
{
  std::size_t h = hash_type(c);
  boost::hash_combine(h, c.variables());
  boost::hash_combine(h, c.constraint());
  return h;
}


std::size_t
hash_value(Binary_cons const& c)
{
  std::size_t h = hash_type(c);
  boost::hash_combine(h, c.left());
  boost::hash_combine(h, c.right());
  return h;
}



std::size_t
hash_value(Cons const& c)
{
  struct fn
  {
    std::size_t operator()(Cons const& c) const           { banjo_unhandled_case(c); }
    std::size_t operator()(Concept_cons const& c) const   { return hash_value(c); }
    std::size_t operator()(Predicate_cons const& c) const { return hash_value(c); }
    std::size_t operator()(Expression_cons const& c) const { return hash_usage(c); }
    std::size_t operator()(Conversion_cons const& c) const { return hash_usage(c); }
    std::size_t operator()(Parameterized_cons const& c) const { return hash_parm(c); }
    std::size_t operator()(Binary_cons const& c) const    { return hash_value(c); }
  };
  return apply(c, fn{});
}


} // namespace banjo
