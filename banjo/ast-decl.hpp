// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_DECL_HPP
#define BANJO_AST_DECL_HPP

#include "ast-type.hpp"


namespace banjo
{

// This type is used to explicitly initialize a declaration that does
// not have a name.
enum unnamed_t
{
  unnamed
};


// -------------------------------------------------------------------------- //
// Declaration specifiers

// A specifier set records a set of declaration specifiers.
enum Specifier_set : int
{
  empty_spec     = 0,
  static_spec    = 1 << 0,
  dynamic_spec   = 1 << 1,  // TODO: Find a use.
  implicit_spec  = 1 << 2,  // TODO: Find a use.
  explicit_spec  = 1 << 3,
  virtual_spec   = 1 << 4,
  abstract_spec  = 1 << 5,
  inline_spec    = 1 << 6,
  public_spec    = 1 << 7,
  private_spec   = 1 << 8,
  protected_spec = 1 << 9,
  ref_spec       = 1 << 10,
  consume_spec   = 1 << 11,
  in_spec        = 1 << 12,
  out_spec       = 1 << 13,
  mutable_spec   = 1 << 14,
  forward_spec   = 1 << 15,
  meta_spec      = 1 << 16,
  
  internal_spec  = 1 << 31, // Internal to the language
};


inline Specifier_set&
operator|=(Specifier_set& a, Specifier_set b)
{
  return a = Specifier_set(a | b);
}


inline Specifier_set
operator|(Specifier_set a, Specifier_set b)
{
  return Specifier_set((int)a | (int)b);
}


// The base class of all declarations. A declaration associates a name with
// information in the context in which it appears.
struct Decl : Term
{
  struct Visitor;
  struct Mutator;

  // Used for unnamed declarations.
  Decl(unnamed_t, Specifier_set s = {})
    : cxt_(), name_(), spec_(s)
  { }

  Decl(Decl& d, unnamed_t, Specifier_set s = {})
    : cxt_(&d), name_(), spec_(s)
  { }

  Decl(Name& n, Specifier_set s = {})
    : cxt_(), name_(&n), spec_(s)
  { }

  Decl(Decl& d, Name& n, Specifier_set s = {})
    : cxt_(&d), name_(&n), spec_(s)
  { }

  virtual void accept(Visitor& v) const = 0;
  virtual void accept(Mutator& v) = 0;

  // Returns a pointer to the type or function in which the declaration
  // is declared. If null, the declaration is in the global scope.
  Decl const* context() const  { return cxt_; }
  Decl*       context()        { return cxt_; }

  // Returns the unqualified identifier of the declared entity.
  Name const& name() const { return *name_; }
  Name&       name()       { return *name_; }

  // Returns true if the declaration has a name.
  bool is_named() const { return name_; }

  // Returns the set of declaration specifiers for the declaration.
  Specifier_set specifiers() const { return spec_; }

  // If the declaration is a template, this returns the templated
  // declaration.
  virtual Decl const& parameterized_declaration() const { return *this; }
  virtual Decl&       parameterized_declaration()       { return *this; }

  Decl*         cxt_;
  Name*         name_;
  Specifier_set spec_;
};


struct Decl::Visitor
{
#define define_node(Node) virtual void visit(Node const&) = 0;
#include "ast-decl.def"
#undef define_node
};


struct Decl::Mutator
{
#define define_node(Node) virtual void visit(Node&) = 0;
#include "ast-decl.def"
#undef define_node
};


// A typed declaration associates a name with a typed entity. Examples
// are variables and functions.
struct Typed_decl : Decl
{
  Typed_decl(Name& n, Type& t, Specifier_set s = {})
    : Decl(n, s), type_(&t)
  { }

  Typed_decl(Decl& d, Name& n, Type& t, Specifier_set s = {})
    : Decl(d, n, s), type_(&t)
  { }

  // Returns the type associated with the name.
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  // Reference specifiers
  bool is_reference() const;
  bool is_normal_reference() const  { return spec_ & ref_spec; }
  bool is_consume_reference() const { return spec_ & consume_spec; }
  bool is_forward_reference() const { return spec_ & forward_spec; }

  Type*         type_;
};


inline bool
Typed_decl::is_reference() const
{
  return is_normal_reference() || is_consume_reference() || is_forward_reference();
}


// Represents a unit of translation: a sequence of statements comprising
// a part of a program or module. 
//
// This is modeled as a declaration even though it has no name or type.
// We do this because many of the translation facilities are defined in
// terms of associations with declarations. It would be nice if translation
// units were not declarations, but this would be a big change.
struct Translation_unit : Decl, Allocatable<Translation_unit>
{
  Translation_unit() 
    : Decl(unnamed)
  { }

  Translation_unit(Stmt_list&& ss)
    : Decl(unnamed), stmts_(std::move(ss))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the list of statements.
  Stmt_list const& statements() const { return stmts_; }
  Stmt_list&       statements()       { return stmts_; }
  
  Stmt_list stmts_;
};


// The base class of object and reference declarations.
//
// NOTE: The naming follows the C++ convention where a variable declares
// either an object (with storage) or a reference.
//
// TODO: Add constructors accepting a context?
struct Variable_decl : Typed_decl, Allocatable<Variable_decl>
{
  Variable_decl(Name& n, Type& t, Def& d, Specifier_set s = {})
    : Typed_decl(n, t, s), def_(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the initializer for the variable.
  Def const& initializer() const { return *def_; }
  Def&       initializer()       { return *def_; }

  Def*  def_;
};


// Declares a mapping from inputs to outputs. 
struct Mapping_decl : Typed_decl
{
  Mapping_decl(Name& n, Type& t, Decl_list const& p, Def& d, Specifier_set s = {})
    : Typed_decl(n, t, s), parms_(p), def_(&d)
  { }

  Mapping_decl(Name& n, Type& t, Decl_list&& p, Def& d, Specifier_set s = {})
    : Typed_decl(n, t, s), parms_(std::move(p)), def_(&d)
  { }

  // Returns the list of parameter declarations for the function.
  Decl_list const& parameters() const { return parms_; }
  Decl_list&       parameters()       { return parms_; }

  // Returns the type of this declaration.
  Function_type const& type() const;
  Function_type&       type();

  // Returns the return type of the function.
  Type const& return_type() const;
  Type&       return_type();

  // Returns the function's constraint expression. This is valid only 
  // when is_constrained() is true.
  Expr const& constraint() const { return *constr_; }
  Expr&       constraint()       { return *constr_; }

  // Returns true if this declaration has function constraints.
  bool is_constrained() const { return constr_; }

  // Returns the function's definition.
  Def const& definition() const { return *def_; }
  Def&       definition()       { return *def_; }

  Decl_list parms_;
  Expr*     constr_;
  Def*      def_;
};


// Declares a function. A function maps input arguments to output values.
// Functions can be evaluated at compile time, if used within a constant
// expression context.
struct Function_decl : Mapping_decl, Allocatable<Function_decl>
{
  using Mapping_decl::Mapping_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the function call operator associated with the function.
  Function_decl const& call() const { return cast<Function_decl>(*call_); }
  Function_decl&       call()       { return cast<Function_decl>(*call_); }

  Decl* call_;
};


// Represents the declaration of a user-defined type. This is the base
// class of class declarations and type parameters.
struct Type_decl : Decl
{
  using Decl::Decl;
};


// Represents the declaration of a class type.
//
// Note that class declarations are un-typed.
// 
// TODO: Extend the type system to define high-order types? I'm not
// quite sure what we'd do with those.
struct Class_decl : Type_decl, Allocatable<Class_decl>
{
  Class_decl(Name& n, Def& d)
    : Type_decl(n), def_(&d)
  { }
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the definition of the class.
  Def const& definition() const { return *def_; }
  Def&       definition()       { return *def_; }

  // Returns the list of member variables.
  Decl_list const& objects() const;
  Decl_list&       objects();

  Type*  kind_;
  Def*   def_;
};


// The base class of member objects and references stores the index
// of the member within its class. 
struct Field_decl : Variable_decl, Allocatable<Field_decl>
{
  using Variable_decl::Variable_decl;

  using Allocatable<Field_decl>::make;
  using Allocatable<Field_decl>::unmake;

  // Returns the index of the field within the class.
  int index() const { return index_; }

  int index_;
};


// Declares a base class subobject. Note that super declarations always
// have an empty definition.
//
// TODO: We could actually provide a default member initializer for
// base classes.
struct Super_decl : Field_decl, Allocatable<Super_decl>
{
  using Field_decl::Field_decl;

  using Allocatable<Super_decl>::make;
  using Allocatable<Super_decl>::unmake;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Declares a member function of a class.
//
// TODO: I think that the type of a method is the same as that of a function,
// except that the first parameter type must always be a (possibly qualified) 
// reference to this. That could be enforced in the constructor, I suppose.
//
// TODO: Add derived classes for constructors and destructors.
struct Method_decl : Function_decl, Allocatable<Method_decl>
{
  using Function_decl::Function_decl;

  using Allocatable<Method_decl>::make;
  using Allocatable<Method_decl>::unmake;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Declares a template.
//
// A template has a single constraint expression corresponding
// to a requires clause. Note that this is transformed into
// a logical proposition for the purpose of constraint checking
// and comparison.
//
// TODO: Consider making a template parameter list a special term. We can 
// link template parameter lists and their constraints. Of course, this 
// may not be necessary.
//
// FIXME: Revisit this.
struct Template_decl : Decl, Allocatable<Template_decl>
{
  Template_decl(Decl_list const& p, Decl& d)
    : Decl(d.name()), parms(p), cons(nullptr), decl(&d)
  {
    lingo_assert(!d.context());
    d.cxt_ = this;
  }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the template parameters of the declaration.
  Decl_list const& parameters() const { return parms; }
  Decl_list&       parameters()       { return parms; }

  // Returns the constraint associated with the template.
  // This is valid iff is_constrained() is true.
  Expr const& constraint() const  { return *cons; }
  Expr&       constraint()        { return *cons; }

  // Set the template constraints.
  //
  // TODO: Is this used anywhere?
  void        constrain(Expr& e)  { cons = &e; }

  // Returns true if the template declaration has constraints.
  bool is_constrained() const { return cons; }

  // Returns the underlying pattern.
  Decl const& parameterized_declaration() const { return *decl; }
  Decl&       parameterized_declaration()       { return *decl; }

  Decl_list parms;
  Expr*     cons;
  Decl*     decl;
};


// Represents a concept definition.
//
// FIXME: Revisit this.
struct Concept_decl : Decl, Allocatable<Concept_decl>
{
  Concept_decl(Name& n, Decl_list const& ps)
    : Decl(n), parms(ps), def(nullptr)
  { }

  Concept_decl(Name& n, Decl_list const& ps, Def& d)
    : Decl(n), parms(ps), def(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the template parameters of the declaration.
  Decl_list const& parameters() const { return parms; }
  Decl_list&       parameters()       { return parms; }

  // Returns the constraint associated with the template. Behavior
  // is only defined when the definition has been assigned. This
  // will generally be the case since every concept declaration
  // must be defined. There are times during processing where it
  // may not be true.
  Def const& definition() const  { return *def; }
  Def&       definition()        { return *def; }

  // Returns true when a definition has been associated with a
  // concept.
  bool is_defined() const { return def; }

  Decl_list parms;
  Def*      def;
};


// An index records the depth and offset of a parameter.
struct Index : std::pair<int, int>
{
  using std::pair<int, int>::pair;

  int depth() const  { return first; }
  int offset() const { return second; }
};


// The parameterized base class of all parameters. This is a CRTP class
// that a) contributes to the definition of parameters by adding a parameter
// index, and b) defines versions of make() and unmake() that allocate
// correctly.
//
// We don't parameter classes deriving from Allocatable because it will 
// contribute another alloc_ pointer to the object. The make() and unmake() 
// functions are (sadly) identical to those in Allocatable.
//
// TODO: Define parameters that accept an index.
//
// TODO: For all derived classes, add accessors and checks to determine
// if the default argument is present.
template<typename T>
struct Parameter : T
{
  using T::T;

  // Returns the index of the template parameter.
  Index  index() const { return ix_; }
  Index& index()       { return ix_; }

  Index ix_;
};


// A parameter containing or referring to an object.
struct Variable_parm : Parameter<Variable_decl>, Allocatable<Variable_parm>
{
  using Parameter<Variable_decl>::Parameter;

  using Allocatable<Variable_parm>::make;
  using Allocatable<Variable_parm>::unmake;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A type parameter of a template.
//
// TODO: Base this on Type_decl instead of Class_decl? Classes are heavy;
// this may not be.
struct Type_parm : Parameter<Class_decl>, Allocatable<Type_parm>
{
  using Parameter<Class_decl>::Parameter;

  using Allocatable<Type_parm>::make;
  using Allocatable<Type_parm>::unmake;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A template parameter of a template.
struct Template_parm : Parameter<Template_decl>, Allocatable<Template_parm>
{
  using Parameter<Template_decl>::Parameter;

  using Allocatable<Template_parm>::make;
  using Allocatable<Template_parm>::unmake;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// -------------------------------------------------------------------------- //
// Operations

// Returns the declared type of the declaration. The declaration shall be
// a typed declaration.
Type& declared_type(Decl&);

inline Type& 
declared_type(Typed_decl& d)
{
  return d.type();
}


// Returns true if `d` is a (normal) function.
inline bool
is_function(Decl const& d)
{
  return is<Function_decl>(&d);
}


// Returns true if d is a function template.
inline bool
is_function_template(Decl const& d)
{
  if (Template_decl const* t = as<Template_decl>(&d))
    return is_function(t->parameterized_declaration());
  return false;
}


// -------------------------------------------------------------------------- //
// Visitors

// A generic visitor for declarations.
template<typename F, typename T>
struct Generic_decl_visitor : Decl::Visitor, Generic_visitor<F, T>
{
  Generic_decl_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

#define define_node(Node) void visit(Node const& t) { this->invoke(t); }
#include "ast-decl.def"
#undef define_node
};


// A generic mutator for declarations.
template<typename F, typename T>
struct Generic_decl_mutator : Decl::Mutator, Generic_mutator<F, T>
{
  Generic_decl_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

#define define_node(Node) void visit(Node& t) { this->invoke(t); }
#include "ast-decl.def"
#undef define_node
};


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl const&)>::type>
inline decltype(auto)
apply(Decl const& d, F fn)
{
  Generic_decl_visitor<F, T> vis(fn);
  return accept(d, vis);
}


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl&)>::type>
inline decltype(auto)
apply(Decl& d, F fn)
{
  Generic_decl_mutator<F, T> vis(fn);
  return accept(d, vis);
}


} // namespace banjo

#endif
