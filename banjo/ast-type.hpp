// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_TYPE_HPP
#define BANJO_AST_TYPE_HPP

#include "ast-base.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Type category

// A type describes an object, reference, or function; the category of a
// type is the entity it describes. 
//
// A function is a kind of reference, except that its storage is not
// defined by abstract machine.
//
// The type of every declaration and expression is exactly one of these
// categories.
//
// NOTE: This is an adaptation of C++'s value categories. C++ lvalues and
// xvalues are normal and normal an consuming references. Function types
// are also included in the lvalue category, but we make that distinction
// explicit. C++ prvalues are objects in this system.
//
// TODO: Support bitfields.
//
// TODO: Is "dependent" a category or a qualifier?
enum Type_category : int
{
  unknown_type,   // The described entity is unknown
  object_type,    // Describes an object
  reference_type, // Describes a normal reference
  function_type,  // Describes a function
};


// -------------------------------------------------------------------------- //
// Type qualifiers

// Every category of types has additional variants, determined by a subset
// of qualifiers. These are
//
//    - const:
//      A const object or reference type is a or refers to a non-modifiable 
//      object. 
//             
//    - volatile:
//      A volatile object or reference type is a or refers to an object whose
//      values can be modified by processes outside of the current thread
//      of execution.
//
//    - meta:
//      A meta object is a compile-time entity and requires constant
//      initialization. Meta objects are implicitly non-modifiable and
//      cannot be volatile. A meta function is one that is always evaluated
//      at its point of use.
//
//    - consume:
//      A consume reference refers to an object whose value may be consumed 
//      by a subsequent operation (the object's original value is reset).
//
//    - noexcept:
//      A noexcept function does not propagate exceptions.
//
// Note objects references can be both const and volatile. Functions shall
// be neither object-qualified nor reference-qualified. Likewise, objects
// and references shall not be function-qualified.
//
// NOTE: Sets of qualifiers corresponding to different categories are
// aligned within the bytes of the underlying type. 
//
// TODO: Extend the partial ordering on object qualifiers to include
// the meta qualifier. Also, is a meta object implicitly const?
//
// TODO: Are there any other qualifiers that we want?
enum Qualifier_set : int
{
  empty_qual    = 0,

  // Object qualifiers
  const_qual    = 1 << 0,
  volatile_qual = 1 << 1,
  meta_qual     = 1 << 2, // Meta also qualifies functions

  // Reference qualifiers
  consume_qual  = 1 << 8,

  // Function qualifiers
  noexcept_qaul = 1 << 12,

  // Combinations
  cv_qual        = const_qual | volatile_qual,

  // Names the set of object qualifiers
  object_qual    = const_qual | volatile_qual,

  // Names the set of object qualifiers.
  reference_qual = consume_qual,

  // Names the set off function qualifiers.
  function_qual  = noexcept_qaul | meta_qual
};


inline Qualifier_set&
operator|=(Qualifier_set& a, Qualifier_set b)
{
  return a = Qualifier_set(a | b);
}


inline Qualifier_set
get_object_qualifiers(Qualifier_set q)
{
  return Qualifier_set(q & object_qual);
}


inline Qualifier_set
get_reference_qualifiers(Qualifier_set q)
{
  return Qualifier_set(q & reference_qual);
}


inline Qualifier_set
get_function_qualifiers(Qualifier_set q)
{
  return Qualifier_set(q & function_qual);
}


// Returns true if a is a superset of b.
inline bool
is_superset(Qualifier_set a, Qualifier_set b)
{
  return (a & b) == b;
}


// Returns true if a is strictly more qualified than b.
inline bool
is_more_qualified(Qualifier_set a, Qualifier_set b)
{
  return is_superset(a, b) && a != b;
}


// -------------------------------------------------------------------------- //
// Types

// A type describes objects, references, and functions.
//
// NOTE: The type system is not inherently canonicalized.
//
// TODO: Re-document this class.
//
// TODO: Ensure that qualifier queries apply to the right category.
struct Type : Term
{
  struct Visitor;
  struct Mutator;

  Type()
    : cat_(), qual_()
  { }

  Type(Type_category c)
    : cat_(c), qual_()
  { }

  Type(Type_category c, Qualifier_set q)
    : cat_(c), qual_(q)
  { }


  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&)       = 0;

  // Returns the category of this type.
  Type_category category() const { return cat_; }
  bool is_object() const    { return cat_ == object_type; }
  bool is_reference() const { return cat_ == reference_type; }
  bool is_function() const  { return cat_ == function_type; }

  // Returns the qualifiers (for various categories) for this type. 
  Qualifier_set qualifiers() const { return qual_; }
  Qualifier_set object_qualifiers() const { return get_object_qualifiers(qual_); }
  Qualifier_set reference_qualifiers() const { return get_reference_qualifiers(qual_); }
  Qualifier_set function_qualifiers() const { return get_function_qualifiers(qual_); }
  
  // Returns true if the type is qualified.
  bool is_qualified() const        { return qual_ != empty_qual; }

  // Object qualifiers
  bool is_const() const    { return qual_ & const_qual; }
  bool is_volatile() const { return qual_ & volatile_qual; }
  bool is_cv() const       { return is_const() && is_volatile(); }

  // Reference qualifiers
  bool is_consume() const  { return qual_ & consume_qual; }

  // Function qualifiers
  bool is_noexcept() const { return qual_ & noexcept_qaul; }

  Type_category  cat_;
  Qualifier_set  qual_;
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


// -------------------------------------------------------------------------- //
// Fundamental types

// The void type. Expressions of void type do not compute objects.
struct Void_type : Type, Allocatable<Void_type>
{
  Void_type(Qualifier_set q = {})
    : Type(object_type, q)
  { }

  Void_type(Type_category c, Qualifier_set q = {})
    : Type(c, q)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The boolean type.
struct Boolean_type : Type, Allocatable<Boolean_type>
{
  Boolean_type(Qualifier_set q = {})
    : Type(object_type, q)
  { }

  Boolean_type(Type_category c, Qualifier_set q = {})
    : Type(c, q)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the type of a byte. A byte is the fundamental unit of 
// storage and is comprised of an implementation defined sequence of bits.
//
// TODO: Should this be an integer type, or should it really be distinct?
struct Byte_type : Type, Allocatable<Byte_type>
{
  Byte_type(Qualifier_set q = {})
    : Type(object_type, q)
  { }

  Byte_type(Type_category c, Qualifier_set q = {})
    : Type(c, q)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The integer types.
//
// TODO: Add a flag that distinguishes between native spellings
// of types and their precise representation? For example, "int"
// is lexically different than "int32", and those differences
// might be meaningful in the output. Do the same for float.
struct Integer_type : Type, Allocatable<Integer_type>
{
  Integer_type(Integer_type const& t)
    : Type(t), sign_(t.sign_), prec_(t.prec_)
  { }

  Integer_type(bool s = true, int p = 32, Qualifier_set q = {})
    : Type(object_type, q), sign_(s), prec_(p)
  { }

  Integer_type(Type_category c, bool s = true, int p = 32, Qualifier_set q = {})
    : Type(c, q), sign_(s), prec_(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  bool sign() const        { return sign_; }
  bool is_signed() const   { return sign_; }
  bool is_unsigned() const { return !sign_; }
  int  precision() const   { return prec_; }

  bool sign_;
  int  prec_;
};


// The IEEE 754 floating point types.
struct Float_type : Type, Allocatable<Float_type>
{
  Float_type(Float_type const& t)
    : Type(t), prec_(t.prec_)
  { }
  
  Float_type(Type_category c, int p = 64, Qualifier_set q = {})
    : Type(c, q), prec_(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  int precision() const { return prec_; }

  int prec_;
};


// -------------------------------------------------------------------------- //
// Compound types

// A function type.
//
// Note that function types are never qualified.
struct Function_type : Type, Allocatable<Function_type>
{
  Function_type(Type_list const& p, Type& r)
    : Type(function_type), parms_(p), ret_(&r)
  { }

  Function_type(Type_list&& p, Type& r)
    : Type(function_type), parms_(std::move(p)), ret_(&r)
  { }

  Function_type(Type_category c, Type_list const& p, Type& r)
    : Type(c), parms_(p), ret_(&r)
  { }

  Function_type(Type_category c, Type_list&& p, Type& r)
    : Type(c), parms_(std::move(p)), ret_(&r)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the parameter types of the function.
  Type_list const& parameter_types() const { return parms_; }
  Type_list&       parameter_types()       { return parms_; }

  // Returns the return type of the function.
  Type const& return_type() const { return *ret_; }
  Type&       return_type()       { return *ret_; }

  Type_list parms_;
  Type*     ret_;
};


// An array type with constant extent.
//
// An array type is not qualified, but its elements can be.
//
// TODO: Save the computed extent. In fact, should we be storing
// the integer extend instead of the expression?
struct Array_type : Type, Allocatable<Array_type>
{
  Array_type(Type_category c, Type& t, Expr& e) 
    : Type(c), type_(&t), expr_(&e) 
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
// A tuple type is not qualified, but its elements can be.
struct Tuple_type : Type, Allocatable<Tuple_type>
{
  Tuple_type(Type_category c, Type_list const& t) 
    : Type(c), elems_(t) 
  { } 

  Tuple_type(Type_list&& t) 
    : elems_(std::move(t))
  { }
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the element types of the tuple.
  Type_list const& element_types() const { return elems_; }
  Type_list&       element_types()       { return elems_; }
  
  Type_list elems_;
};


// A pointer type.
struct Pointer_type : Type, Allocatable<Pointer_type>
{
  Pointer_type(Type_category c, Type& t, Qualifier_set q = {})
    : Type(c, q), type_(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the pointed-at type.
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  Type* type_;
};


// -------------------------------------------------------------------------- //
// User-defined types

// Represents any type that has a declaration (i.e., not a built-in) type. 
//
// A declared type refers to its declaration, which defines its properties.
//
// Note that placeholders and type variables are also declared types.
struct Declared_type : Type
{
  Declared_type(Declared_type const& t)
    : Type(t), decl_(t.decl_)
  { }

  Declared_type(Type_category c, Decl& d, Qualifier_set q = {})
    : Type(c, q), decl_(&d)
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
struct Class_type : Declared_type, Allocatable<Class_type>
{
  using Declared_type::Declared_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the class type.
  Class_decl const& declaration() const;
  Class_decl&       declaration();
};


// A typename type is declared by a type parameter.
struct Typename_type : Declared_type, Allocatable<Typename_type>
{
  using Declared_type::Declared_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
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
struct Auto_type : Declared_type, Allocatable<Auto_type>
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
struct Synthetic_type : Declared_type, Allocatable<Synthetic_type>
{
  using Declared_type::Declared_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// -------------------------------------------------------------------------- //
// Miscellaneous types

// The type decltype(e). The actual type is deduced from the expression.
//
// FIXME: Put this into a category somewhere...
struct Decltype_type : Type, Allocatable<Decltype_type>
{
  using Type::Type;

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
struct Type_type : Type, Allocatable<Type_type>
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents an unparsed type.
struct Unparsed_type : Type, Allocatable<Unparsed_type>
{
  Unparsed_type(Unparsed_type const& t)
    : Type(t), toks_(t.toks_)
  { }

  Unparsed_type(Token_seq&& toks)
    : Type(), toks_(std::move(toks))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the sequence of tokens comprising the unparsed type.
  Token_seq const& tokens() const { return toks_; }
  Token_seq&       tokens()       { return toks_; }

  Token_seq toks_;
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
  return is<Boolean_type>(t);
}


// Returns true if `t` is an integer type.
inline bool
is_integer_type(Type const& t)
{
  return is<Integer_type>(t);
}


// Returns true if `t` is a floating point type.
inline bool
is_floating_point_type(Type const& t)
{
  return is<Float_type>(t);
}


// Returns true if `t` is a function type.
inline bool
is_function_type(Type const& t)
{
  return is<Function_type>(t);
}


// Returns true if `t` is a pointer type.
inline bool
is_pointer_type(Type const& t)
{
  return is<Pointer_type>(t);
}


// Returns true if `t` is an array type.
inline bool
is_array_type(Type const& t)
{
  return is<Array_type>(t);
}


// Returns true if `t` is a tuple type.
inline bool
is_tuple_type(Type const& t)
{
  return is<Tuple_type>(t);
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


// Apply a function to the basic type.
template<typename F, typename T = typename std::result_of<F(Void_type const&)>::type>
inline decltype(auto)
apply(Type const& t, F fn)
{
  Generic_type_visitor<F, T> vis(fn);
  return accept(t, vis);
}


// Apply a function to the basic type.
template<typename F, typename T = typename std::result_of<F(Void_type&)>::type>
inline decltype(auto)
apply(Type& t, F fn)
{
  Generic_type_mutator<F, T> vis(fn);
  return accept(t, vis);
}


} // namesapce banjo


#endif
