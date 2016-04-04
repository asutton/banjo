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

// Byte type.
// Not sure if we should have precision? Are all bytes going to be 8 bits or is
// it architecture dependent? Also are we going to have signed bytes?
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
// We could eliminate this node entirely, and the declauto
// node, except that we would to encode the deduction mechanism
// into the tree somehow. Maybe make deduction a property of
// of the typename declaration (i.e., a decltype decl).
struct Auto_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The type decltype(e).
struct Decltype_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// The type decltype(auto).
struct Declauto_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
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


struct Qualified_type : Type
{
  Qualified_type(Type& t, Qualifier_set q)
    : ty(&t), qual(q)
  {
    lingo_assert(q != empty_qual);
    lingo_assert(!is<Qualified_type>(ty));
  }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  // Returns the qualifier for this type. Note that these
  // override functions in type.
  Qualifier_set qualifier() const   { return qual; }
  bool          is_const() const    { return qual & const_qual; }
  bool          is_volatile() const { return qual & volatile_qual; }

  // Returns the unqualfiied version of this type.
  Type const& unqualified_type() const { return type(); }
  Type&       unqualified_type()       { return type(); }

  Type*         ty;
  Qualifier_set qual;
};


struct Pointer_type : Type
{
  Pointer_type(Type& t)
    : ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
};


struct Reference_type : Type
{
  Reference_type(Type& t)
    : ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  // Returns the non-reference version of this type.
  Type const& non_reference_type() const { return type(); }
  Type&       non_reference_type()       { return type(); }

  Type* ty;
};


struct Array_type : Type
{
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Expr const& extent() const { return *ext; }
  Expr&       extent()       { return *ext; }

  Type* ty;
  Expr* ext;
};


// The type of an unspecified sequence of objects. An array
// of unknown bound.
struct Sequence_type : Type
{
  Sequence_type(Type& t)
    : ty(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
};


// The base class of all user-defined types.
struct User_defined_type : Type
{
  User_defined_type(Decl& d)
    : decl(&d)
  { }

  // Returns the name of the user-defined type.
  Name const& name() const;
  Name&       name();

  // Returns the declaration of the user-defined type.
  Decl const& declaration() const { return *decl; }
  Decl&       declaration()       { return *decl; }

  Decl* decl;
};


struct Class_decl;
struct Union_decl;
struct Enum_decl;
struct Type_parm;


// TODO: Factor a base class for all of these: user-defined type.
struct Class_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the class type.
  Class_decl const& declaration() const;
  Class_decl&       declaration();
};


struct Union_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the union type.
  Union_decl const& declaration() const;
  Union_decl&       declaration();
};


struct Enum_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the enum type.
  Enum_decl const& declaration() const;
  Enum_decl&       declaration();
};


// The type of a type parameter declaration.
//
// FIXME: Guarantee that d is a Type_parm.
struct Typename_type : User_defined_type
{
  using User_defined_type::User_defined_type;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration of the typename type.
  Type_parm const& declaration() const;
  Type_parm&       declaration();
};


// Represents a unique, ininterpreted type. The synthetic type
// refers to the declaration from which it was synthesized.
//
// TODO: Do we always need a declaration, or can we just synthesize
// types from thin air?
struct Synthetic_type : User_defined_type
{
  using User_defined_type::User_defined_type;

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


// Returns true if `t` is a sequence type.
inline bool
is_sequence_type(Type const& t)
{
  return is<Sequence_type>(&t);
}


// Returns true if `t` is a class type.
inline bool
is_class_type(Type const& t)
{
  return is<Class_type>(&t);
}


// Returns true if `t` is a (possibly qualified) class type.
inline bool
is_maybe_qualified_class_type(Type const& t)
{
  return is_class_type(t.unqualified_type());
}


// Returns true if `t` is a union type.
inline bool
is_union_type(Type const& t)
{
  return is<Union_type>(&t);
}


// Returns true if `t` is a (possibly qualified) union type.
inline bool
is_maybe_qualified_union_type(Type const& t)
{
  return is_union_type(t.unqualified_type());
}


// Returns true if `t` is a scalar type.
inline bool
is_scalar_type(Type const& t)
{
  return is_boolean_type(t)
      || is_integer_type(t)
      || is_floating_point_type(t)
      || is_pointer_type(t)
      || is_sequence_type(t);
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
