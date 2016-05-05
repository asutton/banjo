// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_DECL_HPP
#define BANJO_AST_DECL_HPP

#include "ast-base.hpp"
#include "specifier.hpp"



namespace banjo
{

// The base class of all declarations. Every declaration has an associated
// name, type, and set of specifiers.
//
// TODO: Although every declaration is a definition, that doesn't apply
// to parameters in quite the same way. Parameters have default arguments.
// Note that we could simply interpret a default argument as an optionally
// specified definition.
//
// FIXME: Do I really need all of these constructors?
struct Decl : Term
{
  struct Visitor;
  struct Mutator;

  // Constructors for untyped declarations.
  Decl(Name& n)
    : cxt_(), name_(&n), type_(), spec_()
  { }

  Decl(Name& n, Specifier_set s)
    : cxt_(), name_(&n), type_(), spec_(s)
  { }

  Decl(Decl& d, Name& n)
    : cxt_(&d), name_(&n), type_(), spec_()
  { }

  Decl(Decl& d, Name& n, Specifier_set s)
    : cxt_(&d), name_(&n), type_(), spec_(s)
  { }

  // Constructors for typed declarations.
  Decl(Name& n, Type& t)
    : cxt_(), name_(&n), type_(&t), spec_()
  { }

  Decl(Name& n, Type& t, Specifier_set s)
    : cxt_(), name_(&n), type_(&t), spec_(s)
  { }

  Decl(Decl& d, Name& n, Type& t)
    : cxt_(&d), name_(&n), type_(&t), spec_()
  { }

  Decl(Decl& d, Decl& cxt, Name& n, Type& t, Specifier_set s)
    : cxt_(&d), name_(&n), type_(&t), spec_(s)
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

  // Returns the type associated with the name.
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  // Returns the set of declaration specifiers for the declaration.
  Specifier_set specifiers() const { return spec_; }

  // If the declaration is a template, this returns the templated
  // declaration.
  virtual Decl const& parameterized_declaration() const { return *this; }
  virtual Decl&       parameterized_declaration()       { return *this; }

  Decl*         cxt_;
  Name*         name_;
  Type*         type_;
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


// The base class of declarations that denote objects. The type of an object 
// declaration is a reference to the declared type of the object.
//
// TODO: Verify that the type is a reference type.
struct Object_decl : Decl
{
  using Decl::Decl;
};


// Declares a variable.
struct Variable_decl : Object_decl
{
  Variable_decl(Name& n, Type& t, Def& d)
    : Object_decl(n, t), def_(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the initializer for the variable.
  Def const& initializer() const { return *def_; }
  Def&       initializer()       { return *def_; }

  Def*  def_;
};


// Declares a function.
//
// A function has three associated expressions:
//    - a type constraint which governs use,
//    - a precondition which guards entry, and
//    - a postcondition that explicitly states effects.
//
// TODO: Implement preconditions and postconditions.
struct Function_decl : Decl
{
  // FIXME: Consume arguments.
  Function_decl(Name& n, Type& t, Decl_list const& p, Def& d)
    : Decl(n, t), parms_(p), def_(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the type of this declaration.
  Function_type const& type() const;
  Function_type&       type();

  // Returns the return type of the function.
  Type const& return_type() const;
  Type&       return_type();

  // Returns the list of parameter declarations for the function.
  Decl_list const& parameters() const { return parms_; }
  Decl_list&       parameters()       { return parms_; }

  // Returns the function's constraint expression. This is valid only 
  // when  is_constrained() is true.
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


// Represents the declaration of a user-defined type. This is the base
// class of class declarations and type parameters.
struct Type_decl : Decl
{
  using Decl::Decl;

  // Returns the kind of the type. This is the same as the type's type.
  Type const& kind() const { return type(); }
  Type&       kind()       { return type(); }
};


// Represents the declaration of a class type.
struct Class_decl : Type_decl
{
  Class_decl(Name& n, Type& t, Def& d)
    : Type_decl(n, t), def_(&d)
  { }
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the definition of the class.
  Def const& definition() const { return *def_; }
  Def&       definition()       { return *def_; }

  Type*  kind_;
  Def*   def_;
};

struct Coroutine_decl :Decl
{
  Coroutine_decl(Name& n, Type& t, Decl_list const& p, Def& d)
    : Decl(n, t), ret_(&t), def_(&d), parms_(p)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }


  Def const& definition() const { return *def_; }
  Def&       definition()       { return *def_; }

  // Returns the return type of the coroutine.
  Type const& return_type() const { return *ret_; };
  Type&       return_type()       { return *ret_; }

  // Returns the list of parameter declarations for the coroutine.
  Decl_list const& parameters() const { return parms_; }
  Decl_list&       parameters()       { return parms_; }

  Type* ret_;
  Def* def_;
  Decl_list parms_;

};

// Declares a field (member variable) of a class. This stores the index 
// of the field within  the class, which is used to support code generation 
// and compile-time evaluation.

struct Field_decl : Variable_decl
{
  using Variable_decl::Variable_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the index of the field within the class.
  int index() const { return index_; }

  int index_;
};


// Declares a base class subobject.
struct Super_decl : Object_decl
{
  Super_decl(Name& n, Type& t, Def& d)
    : Object_decl(n), type_(&t), def_(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declared type of the super
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  Def const& initializer() const { return *def_; }
  Def&       initializer()       { return *def_; }

  Type * type_;
  Def* def_;
};


// Declares a method (member function) of a record.
//
// TODO: I think that the type of a method is the same as that of a function,
// except that the first parameter type must always be a (possibly qualified) 
// reference to this. That could be enforced in the constructor, I suppose.
struct Method_decl : Function_decl
{
  using Function_decl::Function_decl;

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
// TODO: Consider making a template parameter list a special
// term. We can link template parameter lists and their
// constraints. Of course, this may not be necessary.
//
// FIXME: Revisit this.
struct Template_decl : Decl
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
struct Concept_decl : Decl
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


// The base class of all parameters. This provides deriviation from
// T. Note that the constructor of this clas
template<typename T>
struct Parameter_decl : T
{
  // FIXME: Deprecate this constructor. Every parameter must be
  // declared with an index.
  template<typename... Args>
  Parameter_decl(Args&&... args)
    : T(std::forward<Args>(args)...)
  { }

  template<typename... Args>
  Parameter_decl(Index x, Args&&... args)
    : T(std::forward<Args>(args)...), ix(x)
  { }

  // Returns the index of the template parameter.
  Index  index() const { return ix; }
  Index& index()       { return ix; }

  Index ix;
};


// An object parameter of a function.
//
// TODO: Name this variable_parm to be consistent with variable
// declarations?
//
// TODO: Object parameters have single-value indexes because
// we don't allow nested parameters. How important is it that we
// actually store the index of a parameter.
struct Object_parm : Parameter_decl<Object_decl>
{
  Object_parm(Name& n, Type& t)
    : Parameter_decl<Object_decl>(n, t), init_()
  { }

  Object_parm(Name& n, Type& t, Expr& i)
    : Parameter_decl<Object_decl>(n, t), init_(&i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Expr const& default_argument() const { return *init_; }
  Expr&       default_argument()       { return *init_; }

  bool has_default_arguement() const { return init_; }

  Expr* init_;
};


// A constant value parameter of a template.
//
// TODO: Name this Constant_parm to be consistent with
// constant declarations?
struct Value_parm : Parameter_decl<Object_decl>
{
  Value_parm(Index x, Name& n, Type& t)
    : Parameter_decl<Object_decl>(x, n, t), init_()
  { }

  Value_parm(Index x, Name& n, Type& t, Expr& i)
    : Parameter_decl<Object_decl>(x, n, t), init_(&i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Expr const& default_argument() const { return *init_; }
  Expr&       default_argument()       { return *init_; }

  bool has_default_arguement() const { return init_; }

  Expr* init_;
};


// A type parameter of a template.
//
// TODO: It would be nice to derive from type-decl.
struct Type_parm : Parameter_decl<Type_decl>
{
  Type_parm(Index x, Name& n)
    : Parameter_decl<Type_decl>(x, n), def()
  { }

  Type_parm(Index x, Name& n, Type& t)
    : Parameter_decl<Type_decl>(x, n), def(&t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Type const& default_argument() const { return *def; }
  Type&       default_argument()       { return *def; }

  bool has_default_arguement() const { return def; }

  Type* def;
};


// A template parameter of a template.
//
// The nested effectively denotes the "kind" of the template. It
// must be a template declaration. The name of the parameter and
// that of the underlying declaration must be the same.
struct Template_parm : Parameter_decl<Decl>
{
  Template_parm(Name& n, Decl& t)
    : Parameter_decl<Decl>(n), temp(&t), def()
  { }

  Template_parm(Name& n, Decl& t, Init& i)
    : Parameter_decl<Decl>(n), temp(&t), def(&i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the tempalte declaration that defines the
  // signature of accepted arguments.
  Template_decl const& declaration() const { return *cast<Template_decl>(temp); }
  Template_decl&       declaration()       { return *cast<Template_decl>(temp); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Init const& default_argument() const { return *def; }
  Init&       default_argument()       { return *def; }

  bool has_default_arguement() const { return def; }

  // Returns the index of the template parameter.
  Index  index() const { return ix; }
  Index& index()       { return ix; }

  Decl*     temp;
  Init*     def;
  Index     ix;
};


// -------------------------------------------------------------------------- //
// Operations

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


Type const& declared_type(Decl const&);
Type&       declared_type(Decl&);


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
