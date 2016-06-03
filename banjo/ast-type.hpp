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
// A reference (to an object) can be implicitly converted to an object
// for the purpose of initializing constants, variables, and parameters.
// A reference variable or parameter can also bind to an object.
//
// There are (currently) two kinds of references:
//
//    - A *consume reference* refers to an object whose value may be
//      consumed by a subsequent operation.
//
//    - A *normal reference* has no semantic meaning beyond referring
//      to an object in storage.
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
enum Type_category : int
{
  unknown_cat,  // The described entity is unknown
  object_cat,   // Describes an object
  normal_cat,   // Describes a normal reference
  consume_cat,  // Describes a consume reference 
  function_cat, // Describes a function
};


// -------------------------------------------------------------------------- //
// Object qualifiers

// Every object type (the set of types that can define objects in storage)
// has three additional variants related to the access properties of its
// objects. These are defined by two properties:
//
//    - A *const-qualified* object type defines a non-modifiable object.
//
//    - A *volatile-qualified* object type defines a type whose value can
//      be modified by processes outside of current thread of execution
//      (e.g., a shared global variable or a memory-mapped register file).
//
// Objects can be both const- and volatile-qualified.
//
// Object qualifiers do not apply to reference or function types.
//
// TODO: Are there any other qualifiers that we want?
enum Qualifier_set : int
{
  empty_qual    = 0,
  const_qual    = 1 << 0,
  volatile_qual = 1 << 1,
  cv_qual       = const_qual | volatile_qual
};


inline Qualifier_set&
operator|=(Qualifier_set& a, Qualifier_set b)
{
  return a = Qualifier_set(a | b);
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

struct Basic_type;


// A type describes objects, references, and functions. There are three
// components of a type: its category, its object qualifiers, and its
// basis.
//
// The basis of a type describes its set of values (e.g., int, int*, int[3])
// in terms of its representation. The basis of a type is represented by an 
// abstract syntax tree, in which all terms are derived from Basic_type.
//
// The object type and qualifiers are local enumerations and affect the
// interface and object access semantics.
//
// The underlying shape component for a type is canonicalized. This means 
// that, for every type in a program, there is exactly one shape object
// describing that type. When copied, the shape is shared, not cloned.
//
// TODO: Save space by merging the qualifier and object flags into a
// single field.
struct Type
{
  struct Visitor;
  struct Mutator;

  // Construct an empty type.
  Type()
    : base_(nullptr), cat_(), qual_()
  { }

  // Type qualifiers shall only apply to object types.
  Type(Basic_type& t, Type_category c = {}, Qualifier_set q = {})
    : base_(&t), cat_(c), qual_(q)
  {
    lingo_assert(c != object_cat ? q == empty_qual : true);
  }

  void accept(Visitor&) const;
  void accept(Mutator&);

  // Returns the basis of the type.
  Basic_type const& basis() const { return *base_; }
  Basic_type&       basis()       { return *base_; }

  // Returns true if the type has no shape.
  bool is_empty() const { return base_ == nullptr; }

  explicit operator bool() const { return base_; }

  // Returns reference information for this type.
  Type_category category() const { return cat_; }
  bool is_object() const            { return cat_ == object_cat; }
  bool is_function() const          { return cat_ == function_cat; }
  bool is_normal_reference() const  { return cat_ == normal_cat; }
  bool is_consume_reference() const { return cat_ == consume_cat; }
  bool is_reference() const         { return is_normal_reference() || is_consume_reference(); }

  // Returns the object qualifiers for this type. 
  Qualifier_set qualifiers() const { return qual_; }
  bool is_qualified() const        { return qual_ == empty_qual; }
  bool is_const() const            { return qual_ & const_qual; }
  bool is_volatile() const         { return qual_ & volatile_qual; }
  bool is_cv() const               { return is_const() && is_volatile(); }

  Basic_type*    base_;
  Type_category  cat_;
  Qualifier_set  qual_;
};


// The base class of all types. Note that basic types are canonicalized;
// there is exactly one instance of each value of that type. This means
// that equality is the same as identity.
struct Basic_type
{
  virtual ~Basic_type() { }

  using Visitor = Type::Visitor;
  using Mutator = Type::Mutator;

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&)       = 0;
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


inline void
Type::accept(Visitor& v) const
{
  return base_->accept(v);
}


inline void
Type::accept(Mutator& v)
{
  return base_->accept(v);
}



// A helper function that true if the type t has the shape indicated by T.
//
// Example:
//
//  void f(Type t) {
//    cout << is<Boolean_type>(t);
//  }
//
template<typename T>
inline bool
is(Type t)
{
  return is<T>(t.basis());
}


// A helper function that attempts to convert the type's shape to the type
// indicated by T. Returns null if the conversion fails. Note that converting 
// to a shape's type will discard object and reference qualifiers on the outer 
// type.
//
// Example:
//
//  void f(Type t) {
//    Void_type* v = as<Void_type>(t);
//  }
//
template<typename T>
inline T*
as(Type t)
{
  return as<T>(&t.basis());
}


template<typename T>
inline T const*
as(Type const& t)
{
  return as<T>(&t.basis());
}


// A helper function that convert the type's shape to the type indicated by 
// T. Behavior is undefined if the conversion fails. Note that converting to 
// a shape's type will discard object and reference qualifiers.
//
// Example:
//
//  void f(Type t) {
//    Void_type& v = cast<Void_type>(t);
//  }
//
template<typename T>
inline T&
cast(Type& t)
{
  return cast<T>(t.basis());
}


template<typename T>
inline T const&
cast(Type const& t)
{
  return cast<T>(t.basis());
}


// -------------------------------------------------------------------------- //
// Fundamental types

// The void type. Expressions of void type do not compute objects.
struct Void_type : Basic_type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The boolean type.
struct Boolean_type : Basic_type
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
struct Integer_type : Basic_type
{
  Integer_type(bool s = true, int p = 32)
    : sign_(s), prec_(p)
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


// Represents the type of a byte. A byte is the fundamental unit of 
// storage and is comprised of an implementation defined sequence of bits.
//
// TODO: Should this be an integer type, or should it really be distinct?
struct Byte_type : Basic_type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The IEEE 754 floating point types.
struct Float_type : Basic_type
{
  Float_type(int p = 64)
    : prec(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  int precision() const { return prec; }

  int prec;
};


// -------------------------------------------------------------------------- //
// Compound types

// A function type.
//
// Note that function types are never qualified.
struct Function_type : Basic_type
{
  Function_type(Type_list const& p, Type r)
    : parms_(p), ret_(r)
  { }

  Function_type(Type_list&& p, Type r)
    : parms_(std::move(p)), ret_(r)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the parameter types of the function.
  Type_list const& parameter_types() const { return parms_; }

  // Returns the return type of the function.
  Type  return_type() const { return ret_; }

  Type_list parms_;
  Type      ret_;
};


// An array type with constant extent.
//
// An array type is not qualified, but its elements can be.
//
// TODO: Save the computed extent. In fact, should we be storing
// the integer extend instead of the expression?
struct Array_type : Basic_type
{
  Array_type(Type t, Expr& e) 
    : type_(t), expr_(&e) 
  { } 
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the element type of the array.
  Type element_type() const { return type_; }

  // Returns the expression defining the extent of the array.
  Expr const& extent() const { return *expr_; }
  Expr&       extent()       { return *expr_; }

  Type type_;
  Expr* expr_;
};


// A tuple type.
//
// A tuple type is not cv-qualified, but its elements can be.
struct Tuple_type : Basic_type
{
  Tuple_type(Type_list const& t) 
    : types_(t) 
  { } 

  Tuple_type(Type_list&& t) 
    : types_(std::move(t))
  { }
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
  
  // Returns the element types of the tuple.
  Type_list const& element_types() const { return types_; }
  
  Type_list types_;
};


// A pointer type.
struct Pointer_type : Basic_type
{
  Pointer_type(Type t)
    : type_(t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the pointed-at type.
  Type type() const { return type_; }

  Type type_;
};


// -------------------------------------------------------------------------- //
// User-defined types

// Represents any type that has a declaration (i.e., not a built-in) type. 
// Note that placeholders and type variables are also declared types.
struct Declared_type : Basic_type
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


// -------------------------------------------------------------------------- //
// Miscellaneous types

// The type decltype(e). The actual type is deduced from the expression.
//
// FIXME: Put this into a category somewhere...
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
struct Type_type : Basic_type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents an unparsed type.
struct Unparsed_type : Basic_type
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
is_boolean_type(Type t)
{
  return is<Boolean_type>(t);
}


// Returns true if `t` is an integer type.
inline bool
is_integer_type(Type t)
{
  return is<Integer_type>(t);
}


// Returns true if `t` is a floating point type.
inline bool
is_floating_point_type(Type t)
{
  return is<Float_type>(t);
}


// Returns true if `t` is a function type.
inline bool
is_function_type(Type t)
{
  return is<Function_type>(t);
}


// Returns true if `t` is a pointer type.
inline bool
is_pointer_type(Type t)
{
  return is<Pointer_type>(t);
}


// Returns true if `t` is an array type.
inline bool
is_array_type(Type t)
{
  return is<Array_type>(t);
}


// Returns true if `t` is a tuple type.
inline bool
is_tuple_type(Type t)
{
  return is<Tuple_type>(t);
}


// Returns true if `t` is a class type.
inline bool
is_class_type(Type t)
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
apply(Basic_type const& t, F fn)
{
  Generic_type_visitor<F, T> vis(fn);
  return accept(t, vis);
}


// Apply a function to the basic type.
template<typename F, typename T = typename std::result_of<F(Void_type&)>::type>
inline decltype(auto)
apply(Basic_type& t, F fn)
{
  Generic_type_mutator<F, T> vis(fn);
  return accept(t, vis);
}


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_type const&)>::type>
inline decltype(auto)
apply(Type const& t, F fn)
{
  return apply(t.basis(), fn);
}


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_type&)>::type>
inline decltype(auto)
apply(Type& t, F fn)
{
  return apply(t.basis(), fn);
}




} // namesapce banjo

#endif
