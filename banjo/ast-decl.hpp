// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_DECL_HPP
#define BANJO_AST_DECL_HPP

#include "ast-base.hpp"
#include "specifier.hpp"


namespace banjo
{

// The base class of all declarations. Each declaration has a set of
// specifiers and a reference to the context in which it the entity
// is declared.
//
// TODO: Factor named and typed declarations. Variables, constants, and
// functions have names and types. Classes and namespaces have types.
// Import directives and assertions have neither.
struct Decl : Term
{
  struct Visitor;
  struct Mutator;

  Decl(Name& n)
    : spec(), cxt(nullptr), id(&n)
  { }

  Decl(Decl& cxt, Name& n)
    : spec(), cxt(&cxt), id(&n)
  { }

  virtual void accept(Visitor& v) const = 0;
  virtual void accept(Mutator& v) = 0;

  // Returns a pointer to the context to which this
  // declaration belongs. This is only null for the
  // global namespace.
  Decl const* context() const  { return cxt; }
  Decl*       context()        { return cxt; }
  void        context(Decl& d) { cxt = &d; }

  // Return the name of the declaration.
  Name const& declared_name() const;
  Name&       declared_name();

  // Returns the name with which the declaration was declared.
  // Note that this can be a qualified id.
  Name const& name() const { return *id; }
  Name&       name()       { return *id; }

  // If the declaration is a template, this returns the templated
  // declaration.
  virtual Decl const& parameterized_declaration() const { return *this; }
  virtual Decl&       parameterized_declaration()       { return *this; }

  // Returns the saved scope associated with the declaration, if any.
  // Not all declarations have an associated scope.
  virtual Scope const* scope() const { return nullptr; }
  virtual Scope*       scope()       { return nullptr; }

  Specifier_set spec;
  Decl*         cxt;
  Name*         id;
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


// Declares a variable.
struct Variable_decl : Decl
{
  Variable_decl(Name& n, Type& t, Def& d)
    : Decl(n), type_(&t), def_(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declared type of the variable.
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  // Returns the initializer for the variable.
  Def const& initializer() const     { return *def_; }
  Def&       initializer()           { return *def_; }

  Type* type_;
  Def*  def_;
};


// Declares a function.
//
// A function has three associated expressions:
//    - a type constraint which governs use,
//    - a precondition which guards entry, and
//    - a postcondition that explicitly states effects.
struct Function_decl : Decl
{
  // FIXME: Consider deprecate this.
  Function_decl(Name& n, Type& t, Decl_list const& p)
    : Decl(n), type_(&t), parms_(p), def_()
  { lingo_unreachable(); }

  // FIXME: Consume parameters.
  Function_decl(Name& n, Type& t, Decl_list const& p, Def& d)
    : Decl(n), type_(&t), parms_(p), def_(&d)
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

  // Returns the function constraints. This is valid iff
  // is_constrained() is true.
  //
  // TODO: Implelemnt pre- and post-conditions.
  Expr const& constraint() const     { return *constr_; }
  Expr&       constraint()           { return *constr_; }

  // Returns true if this declaration has function constraints.
  bool is_constrained() const { return constr_; }

  // Returns the function's definition.
  Def const& definition() const    { return *def_; }
  Def&       definition()          { return *def_; }

  Type*     type_;
  Decl_list parms_;
  Expr*     constr_;
  Def*      def_;
};


// Represents the declaration of a user-defined type.
//
// TODO: Support kinds and/or metatypes.
struct Type_decl : Decl
{
  // FIXME: Deprecate this constructor.
  Type_decl(Name& n)
    : Decl(n)
  { lingo_unreachable(); }

  Type_decl(Name& n, Type& t, Def& d)
    : Decl(n), kind_(&t), def_(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the kind of the type.
  Type const& kind() const { return *kind_; }
  Type&       kind()       { return *kind_; }

  // Returns the definition of the type.
  Def const& definition() const { return *def_; }
  Def&       definition()       { return *def_; }

  Type* kind_;
  Def*  def_;
};


// Declares a template.
//
// A template has a single constraint expression corresponding
// to a requires clause. Note that this is transformed into
// a logical proposition for the purpose of constraint checking
// and comparison.
//
// TODO: Consider making a template parameter list a special
// term. We can linke template parameter lists and their
// constraints. Of course, this may not be necessary.
struct Template_decl : Decl
{
  Template_decl(Decl_list const& p, Decl& d)
    : Decl(d.name()), parms(p), cons(nullptr), decl(&d)
  {
    lingo_assert(!d.context());
    d.context(*this);
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


// Declares a variable, constant, or function parameter.
//
// FIXME: I don't like this class.
struct Object_decl : Decl
{
  Object_decl(Name& n, Type& t)
    : Decl(n), type_(&t), init_()
  { }

  Object_decl(Name& n, Type& t, Expr& e)
    : Decl(n), type_(&t), init_(&e)
  { }

  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  Type* type_;
  Expr* init_;
};


// An object paramter of a function.
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
    : Parameter_decl<Object_decl>(n, t)
  { }

  Object_parm(Name& n, Type& t, Expr& i)
    : Parameter_decl<Object_decl>(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Expr const& default_argument() const { return *init_; }
  Expr&       default_argument()       { return *init_; }

  bool has_default_arguement() const { return init_; }
};


// A constant value parameter of a template.
//
// TODO: Name this Constant_parm to be consistent with
// constant declarations?
struct Value_parm : Parameter_decl<Object_decl>
{
  Value_parm(Index x, Name& n, Type& t)
    : Parameter_decl<Object_decl>(x, n, t)
  { }

  Value_parm(Index x, Name& n, Type& t, Expr& i)
    : Parameter_decl<Object_decl>(x, n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the default argument for the parameter.
  // This is valid iff has_default_arguement() is true.
  Expr const& default_argument() const { return *init_; }
  Expr&       default_argument()       { return *init_; }

  bool has_default_arguement() const { return init_; }
};


// A type parameter of a template.
//
// TODO: It would be nice to derive from type-decl.
struct Type_parm : Parameter_decl<Decl>
{
  Type_parm(Index x, Name& n)
    : Parameter_decl<Decl>(x, n), def()
  { }

  Type_parm(Index x, Name& n, Type& t)
    : Parameter_decl<Decl>(x, n), def(&t)
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
