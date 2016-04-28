// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_TYPE_HPP
#define BANJO_AST_TYPE_HPP

#include "ast-base.hpp"
#include "qualifier.hpp"


namespace banjo
{

// The base class of all types.
struct Type : Term
{
  struct Visitor;
  struct Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&)       = 0;

  // Returns the qualifier for this type.
  virtual Qualifier_set qualifier() const   { return empty_qual; }
  virtual bool          is_const() const    { return false; }
  virtual bool          is_volatile() const { return false; }
  bool                  is_qualified() { return qualifier() != empty_qual; }

  // Returns the unqualified version of this type.
  virtual Type const& unqualified_type() const { return *this; }
  virtual Type&       unqualified_type()       { return *this; }

  // Returns the non-reference version of this type.
  virtual Type const& non_reference_type() const { return *this; }
  virtual Type&       non_reference_type()       { return *this; }
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
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The boolean type.
struct Boolean_type : Type
{
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
    : sgn(s), prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  bool sign() const        { return sgn; }
  bool is_signed() const   { return sgn; }
  bool is_unsigned() const { return !sgn; }
  int  precision() const   { return prec; }

  bool sgn;
  int  prec;
};


// Represents the type of a byte. A byte is the fundamental unit of storage 
// and is comprised of an implementation defined sequence of bits. 
struct Byte_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The IEEE 754 floating point types.
struct Float_type : Type
{
  Float_type(int p = 64)
    : prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  int precision() const { return prec; }

  int prec;
};



// A function type.
struct Function_type : Type
{
  Function_type(Type_list const& p, Type& r)
    : parms(p), ret(&r)
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


// Represents any type that has a declaration (i.e., not a built-in)
// type. Note that placeholders and type variables are also declared
// types.
struct Declared_type : Type
{
  Declared_type(Decl& d)
    : decl_(&d)
  { }

  // Returns the name of the user-defined type.
  Name const& name() const;
  Name&       name();

  // Returns the declaration of the user-defined type.
  Type_decl const& declaration() const;
  Type_decl&       declaration();

  Decl* decl_;
};


// A user-defined type refers to its declaration.
//
// FIXME: Rename to Class_type.
struct Class_type : Declared_type
{
  using Declared_type::Declared_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The base class of all unary type constructors.
struct Unary_type : Type
{
  Unary_type(Type& t)
    : type_(&t)
  { }

  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  Type* type_;
};


// A qualified type.
struct Qualified_type : Unary_type
{
  Qualified_type(Type& t, Qualifier_set q)
    : Unary_type(t), qual(q)
  {
    lingo_assert(q != empty_qual);
    lingo_assert(!is<Qualified_type>(t));
  }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the qualifier for this type. Note that these
  // override functions in type.
  Qualifier_set qualifier() const   { return qual; }
  bool          is_const() const    { return qual & const_qual; }
  bool          is_volatile() const { return qual & volatile_qual; }

  // Returns the unqualified version of this type.
  Type const& unqualified_type() const { return type(); }
  Type&       unqualified_type()       { return type(); }

  Qualifier_set qual;
};


struct Pointer_type : Unary_type
{
  using Unary_type::Unary_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents a reference to an object.
struct Reference_type : Unary_type
{
  using Unary_type::Unary_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the non-reference version of this type.
  Type const& non_reference_type() const { return type(); }
  Type&       non_reference_type()       { return type(); }
};


struct Array_type : Type
{
  Array_type(Type& t, Expr& e) : ty(&t),ext(&e) {} 
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Expr const& extent() const { return *ext; }
  Expr&       extent()       { return *ext; }

  Type* ty;
  Expr* ext;
};


struct Tuple_type : Type
{
  Tuple_type(Type_list const& t) : ty(t) {} 
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
  
  Type_list const& type_list() const { return ty; }
  Type_list&       type_list()       { return ty; }
  
  Type_list ty;
};


// The type of an unspecified slice of an array. This is essentially a
// pointer to a subset of an array. This type does not contain information
// related to offset, length, and stride.
//
// TODO: This type could contain information about offset, length, and
// stride based on its construction from e.g., an array. In fact, that's
// probably an excellent idea. We just need an expression that can construct
// these objects.
struct Slice_type : Unary_type
{
  using Unary_type::Unary_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


struct Dynarray_type : Type
{
  Dynarray_type(Type& t, Expr& e) : ty(&t),ext(&e) {} 
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Expr const& extent() const { return *ext; }
  Expr&       extent()       { return *ext; }

  Type* ty;
  Expr* ext;
};


// The type of a parameter pack.
struct Pack_type : Unary_type
{
  using Unary_type::Unary_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The auto type.
//
// TODO: Allow a deduction constraint on placeholder types.
// This would be checked after deduction. Extend this for
// decltype(auto) types as well.
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


// The type decltype(e). The actual type is deduced from the
// expression.
struct Decltype_type : Type
{
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


// The type of types.
struct Type_type : Type
{
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


// Returns true if `t` is a reference type.
inline bool
is_reference_type(Type const& t)
{
  return is<Reference_type>(&t);
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


// Returns true if `t` is a dynarray type.
inline bool
is_dynarray_type(Type const& t)
{
  return is<Dynarray_type>(&t);
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
