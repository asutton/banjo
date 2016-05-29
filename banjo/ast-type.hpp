// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_TYPE_HPP
#define BANJO_AST_TYPE_HPP

#include "ast-base.hpp"
#include "qualifier.hpp"


namespace banjo
{

// The base class of all types. 
//
// Each object type has a finite number qualified variants. Qualifiers
// include. In particular a qualified type can be const, volatile, or
// both. 
//
// Note that the qualification of a type must not be changed during
// its lifetime. Use the builder to request types of different
// qualification.
//
// TODO: Do we want more qualifiers? Atomic? Restrict?
struct Type : Term
{
  struct Visitor;
  struct Mutator;

  Type()
    : qual_(empty_qual)
  { }

  Type(Qualifier_set q)
    : qual_(q)
  { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&)       = 0;

  // Returns the qualifier for this type. 
  Qualifier_set qualifiers() const { return qual_; }
  bool is_qualified() const { return qual_ == empty_qual; }
  bool is_const() const     { return qual_ & const_qual; }
  bool is_volatile() const  { return qual_ & volatile_qual; }
  bool is_cv() const        { return is_const() && is_volatile(); }

  Qualifier_set qual_;
};


struct Type::Visitor
{
#define define_node(Node) virtual void visit(Node const&) = 0;
#include "ast-type.def"
#undef define_node
};


struct Type::Mutator
{
#define define_node(Node) virtual void visit(Node&) = 0;
#include "ast-type.def"
#undef define_node
};


// The void type.
struct Void_type : Type
{
  using Type::Type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The boolean type.
struct Boolean_type : Type
{
  using Type::Type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The integer types.
//
// TODO: Add a flag that distinguishes between native spellings
// of types and their precise representation? For example, "int"
// is lexically different than "int32", and those differences
// might be meaningful in the output. Do the same for float.
struct Integer_type : Type
{
  Integer_type(bool s = true, int p = 32)
    : Type(), sgn_(s), prec_(p)
  { }

  Integer_type(Qualifier_set q, bool s = true, int p = 32)
    : Type(q), sgn_(s), prec_(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  bool sign() const        { return sgn_; }
  bool is_signed() const   { return sgn_; }
  bool is_unsigned() const { return !sgn_; }
  int  precision() const   { return prec_; }

  bool sgn_;
  int  prec_;
};


// Represents the type of a byte. A byte is the fundamental unit of storage 
// and is comprised of an implementation defined sequence of bits. 
struct Byte_type : Type
{
  using Type::Type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The IEEE 754 floating point types.
struct Float_type : Type
{
  Float_type(int p = 64)
    : Type(), prec(p)
  { }

  Float_type(Qualifier_set q, int p = 64)
    : Type(q), prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  int precision() const { return prec; }

  int prec;
};


// A function type.
//
// Note that function types are never qualified.
struct Function_type : Type
{
  Function_type(Type_list const& p, Type& r)
    : Type(), parms(p), ret(&r)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type_list const& parameter_types() const { return parms; }
  Type_list&       parameter_types()       { return parms; }

  Type const&      return_type() const     { return *ret; }
  Type&            return_type()           { return *ret; }

  Type_list parms;
  Type*     ret;
};


// An array type with constant extent.
//
// An array type is not qualified, but its elements can be.
//
// TODO: Save the computed extent. In fact, should we be storing
// the integer extend instead of the expression?
struct Array_type : Type
{
  Array_type(Type& t, Expr& e) 
    : Type(), type_(&t), expr_(&e) 
  { } 
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the element type of the array.
  Type const& element_type() const { return *type_; }
  Type&       element_type()       { return *type_; }

  // Returns the expression defining the extent of the array.
  Expr const& extent() const { return *expr_; }
  Expr&       extent()       { return *expr_; }

  Type* type_;
  Expr* expr_;
};


// A tuple type.
//
// A tuple type is not cv-qualified, but its elements can be.
struct Tuple_type : Type
{
  Tuple_type(Type_list&& t) 
    : types_(std::move(t))
  { }

  Tuple_type(Type_list const& t) 
    : types_(t) 
  { } 
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
  
  Type_list const& element_types() const { return types_; }
  Type_list&       element_types()       { return types_; }
  
  Type_list types_;
};


// A pointer type.
struct Pointer_type : Type
{
  Pointer_type(Type& t)
    : Type(), type_(&t)
  { }

  Pointer_type(Qualifier_set q, Type& t)
    : Type(q), type_(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the pointed-at type.
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  Type* type_;
};


// Represents any type that has a declaration (i.e., not a built-in)
// type. Note that placeholders and type variables are also declared
// types.
struct Declared_type : Type
{
  Declared_type(Decl& d)
    : Type(), decl_(&d)
  { }

  Declared_type(Qualifier_set q, Decl& d)
    : Type(q), decl_(&d)
  { }

  // Returns the name of the user-defined type.
  Name const& name() const;
  Name&       name();

  // Returns the declaration of the user-defined type.
  Type_decl const& declaration() const;
  Type_decl&       declaration();

  Decl* decl_;
};


// A class type is declared by a class.
struct Class_type : Declared_type
{
  using Declared_type::Declared_type;

  // Returns the declaration of the class type.
  Class_decl const& declaration() const;
  Class_decl&       declaration();

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A typename type is declared by a type parameter.
struct Typename_type : Declared_type
{
  using Declared_type::Declared_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the type of a coroutine.
//
// TODO: Do I really want a coroutine to be a class type. These can't
// have or act as base classes, etc.
struct Coroutine_type : Class_type
{
  using Class_type::Class_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the list of parameters used to initialize the
  // coroutine closure.
  Type_list const& parameter_types() const { return params; }
  Type_list&       parameter_types()       { return params; }

  // Returns the type yielded by the coroutine in every evaluation.
  Type const& return_type() const     { return *ret; }
  Type&       return_type()           { return *ret; }

  Type_list params;
  Type*     ret;
};


// The auto type.
//
// TODO: Allow a deduction constraint on placeholder types. This would be 
// checked after deduction. Extend this for decltype(auto) types as well.
//
// FIXME: The model for auto types works like this:
//
//    auto x = e;
//
// Is essentially shorthand for writing:
//
//    typename T;
//    T x = e;
//
// So x would have typename-type and not auto-type. If we
// add constraints, we might represent that internally as
// this.
//
//    typename T;
//    T|C x = e;  // |C means give C.
//
// FIXME: Do we need decltype(auto) or something similar? That could act
// as a specifier that affects deduction of a type. The rules are close to
// forwarding, but can give different results... I think.
struct Auto_type : Declared_type
{
  using Declared_type::Declared_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents a unique, uninterpreted type. The synthetic type
// refers to the declaration from which it was synthesized.
//
// TODO: Do we always need a declaration, or can we just synthesize
// types from thin air?
struct Synthetic_type : Declared_type
{
  using Declared_type::Declared_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The type decltype(e). The actual type is deduced from the
// expression.
struct Decltype_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The type of a user-defined type.
//
// NOTE: This is currently used to model the "absence" of a meta-type 
// for a class declaration. There's probably something better that
// we could call this, or eliminate it completely in favor of a better
// design?
//
// TODO: Rename to Type_kind? Something else?
struct Type_type : Type
{
  Type_type()
    : Type()
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents an unparsed type.
struct Unparsed_type : Type
{
  Unparsed_type(Token_seq&& toks)
    : toks(std::move(toks))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Token_seq const& tokens() const { return toks; }
  Token_seq&       tokens()       { return toks; }

  Token_seq toks;
};


// -------------------------------------------------------------------------- //
// Queries on types
//
// TODO: For the is_*_type() predicates, should we account
// for qualified types? For example, most rules asking for
// a class type also cover qualified class types also.


// Returns true if `t` is the boolean type.
inline bool
is_boolean_type(Type const& t)
{
  return is<Boolean_type>(&t);
}


// Returns true if `t` is an integer type.
inline bool
is_integer_type(Type const& t)
{
  return is<Integer_type>(&t);
}


// Returns true if `t` is a floating point type.
inline bool
is_floating_point_type(Type const& t)
{
  return is<Float_type>(&t);
}


// Returns true if `t` is a function type.
inline bool
is_function_type(Type const& t)
{
  return is<Function_type>(&t);
}


// Returns true if `t` is a pointer type.
inline bool
is_pointer_type(Type const& t)
{
  return is<Pointer_type>(&t);
}


// Returns true if `t` is an array type.
inline bool
is_array_type(Type const& t)
{
  return is<Array_type>(&t);
}


// Returns true if `t` is a tuple type.
inline bool
is_tuple_type(Type const& t)
{
  return is<Tuple_type>(&t);
}


// Returns true if `t` is a class type.
inline bool
is_class_type(Type const& t)
{
  return is<Class_type>(t);
}


// Returns true if `t` is a scalar type.
inline bool
is_scalar_type(Type const& t)
{
  return is_boolean_type(t)
      || is_integer_type(t)
      || is_floating_point_type(t)
      || is_pointer_type(t);
}


// Returns true if t is a compound type.
inline bool
is_compound_type(Type const& t)
{
  return is_array_type(t)
      || is_tuple_type(t)
      || is_class_type(t);
}


bool is_simple_type(Type const&);
bool is_object_type(Type const&);
bool is_dependent_type(Type const&);


// -------------------------------------------------------------------------- //
// Visitors

// A generic visitor for types.
template<typename F, typename T>
struct Generic_type_visitor : Type::Visitor, Generic_visitor<F, T>
{
  Generic_type_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

#define define_node(Node) void visit(Node const& t) { this->invoke(t); }
#include "ast-type.def"
#undef define_node
};


// A generic mutator for types.
template<typename F, typename T>
struct Generic_type_mutator : Type::Mutator, Generic_mutator<F, T>
{
  Generic_type_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

#define define_node(Node) void visit(Node& t) { this->invoke(t); }
#include "ast-type.def"
#undef define_node
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_type const&)>::type>
inline decltype(auto)
apply(Type const& t, F fn)
{
  Generic_type_visitor<F, T> vis(fn);
  return accept(t, vis);
}


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_type&)>::type>
inline decltype(auto)
apply(Type& t, F fn)
{
  Generic_type_mutator<F, T> vis(fn);
  return accept(t, vis);
}


} // namesapce banjo

#endif
