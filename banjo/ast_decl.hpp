// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_DECL_HPP
#define BANJO_AST_DECL_HPP

#include "ast_base.hpp"
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
  virtual void visit(Variable_decl const&)  { }
  virtual void visit(Constant_decl const&)  { }
  virtual void visit(Function_decl const&)  { }
  virtual void visit(Class_decl const&)     { }
  virtual void visit(Union_decl const&)     { }
  virtual void visit(Enum_decl const&)      { }
  virtual void visit(Namespace_decl const&) { }
  virtual void visit(Template_decl const&)  { }
  virtual void visit(Concept_decl const&)   { }
  virtual void visit(Axiom_decl const&)     { }
  virtual void visit(Object_parm const&)    { }
  virtual void visit(Value_parm const&)     { }
  virtual void visit(Type_parm const&)      { }
  virtual void visit(Template_parm const&)  { }
  virtual void visit(Variadic_parm const&)  { }
};


struct Decl::Mutator
{
  virtual void visit(Variable_decl&)  { }
  virtual void visit(Constant_decl&)  { }
  virtual void visit(Function_decl&)  { }
  virtual void visit(Class_decl&)     { }
  virtual void visit(Union_decl&)     { }
  virtual void visit(Enum_decl&)      { }
  virtual void visit(Namespace_decl&) { }
  virtual void visit(Template_decl&)  { }
  virtual void visit(Concept_decl&)   { }
  virtual void visit(Axiom_decl&)     { }
  virtual void visit(Object_parm&)    { }
  virtual void visit(Value_parm&)     { }
  virtual void visit(Type_parm&)      { }
  virtual void visit(Template_parm&)  { }
  virtual void visit(Variadic_parm&)  { }
};


// Declares a variable, constant, or function parameter.
struct Object_decl : Decl
{
  Object_decl(Name& n, Type& t)
    : Decl(n), ty(&t), init()
  { }

  Object_decl(Name& n, Type& t, Expr& e)
    : Decl(n), ty(&t), init(&e)
  { }

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
  Expr* init;
};


// Declares a class, union, enum.
struct Type_decl : Decl
{
  Type_decl(Name& n)
    : Decl(n), def()
  { }

  Type_decl(Name& n, Def& i)
    : Decl(n), def(&i)
  { }

  Def const& definition() const { return *def; }
  Def&       definition()       { return *def; }

  bool is_defined() const { return def; }

  Def* def;
};


// Declares a variable.
struct Variable_decl : Object_decl
{
  Variable_decl(Name& n, Type& t)
    : Object_decl(n, t)
  { }

  Variable_decl(Name& n, Type& t, Expr& i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the initializer for the variable. This is
  // defined iff has_initializer() is true.
  Expr const& initializer() const     { return *init; }
  Expr&       initializer()           { return *init; }
  bool        has_initializer() const { return init; }
};


// Declares a symbolic constant.
struct Constant_decl : Object_decl
{
  Constant_decl(Name& n, Type& t)
    : Object_decl(n, t)
  { }

  Constant_decl(Name& n, Type& t, Expr& i)
    : Object_decl(n, t, i)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the initializer for the variable. This is
  // defined iff has_initializer() is true.
  Expr const& initializer() const     { return *init; }
  Expr&       initializer()           { return *init; }
  bool        has_initializer() const { return init; }
};


// Declares a function.
//
// A function has three associated exprssions:
//    - a type constraint which governs use,
//    - a precondition which guards entry, and
//    - a postcondition that explicitly states effects.
struct Function_decl : Decl
{
  Function_decl(Name& n, Type& t, Decl_list const& p)
    : Decl(n), ty(&t), parms(p), def()
  { }

  Function_decl(Name& n, Type& t, Decl_list const& p, Def& d)
    : Decl(n), ty(&t), parms(p), def(&d)
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
  Decl_list const& parameters() const { return parms; }
  Decl_list&       parameters()       { return parms; }

  // Returns the function constraints. This is valid iff
  // is_constrained() is true.
  Expr const& constraint() const     { return *constr; }
  Expr&       constraint()           { return *constr; }

  // TODO: Implelemnt pre- and post-conditions.
  // Expr const& precondition() const  { return *constr; }
  // Expr const& postcondition() const { return *constr; }

  Def const& definition() const    { return *def; }
  Def&       definition()          { return *def; }

  // Returns true if this declaration has function constraints.
  bool is_constrained() const { return constr; }

  // Returns true iff this declaration is also a definition.
  bool is_definition() const { return def; }

  Type*     ty;
  Decl_list parms;
  Expr*     constr;
  Expr*     pre;
  Expr*     post;
  Def*      def;
};


// Represents the declaration of a class.
struct Class_decl : Type_decl
{
  using Type_decl::Type_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the definition for the class, if given. Behavior is
  // defined iff is_definition() is true.
  Class_def const& definition() const;
  Class_def&       definition();

  // Returns true if the declaration is also a definition.
  bool is_definition() const { return def; }
};


struct Union_decl : Type_decl
{
  using Type_decl::Type_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Union_def const& definition() const;
  Union_def&       definition();
};


struct Enum_decl : Type_decl
{
  using Type_decl::Type_decl;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Enum_def const& definition() const;
  Enum_def&       definition();
};


// Represents the definition of a namespace and its enclosed declarations.
// Each namespace definition points to a (shared) scope that contains the
// aggregated declarations of all declarations of the same namespace. Note
// that re-opened namespaces are distinct declarations that share the same
// scope. For example:
//
//    naemspace N {
//      int x;
//    } // #1
//
//    namespace N {
//      int y;
//    } // #2
//
// At #1, there is a single namespace declaration named `N`, containing
// only the declaration of `x`. At #2, there are 2 namespace declarations
// of `N`. They share the same scope, which contains the declarations of
// `x` and `y`.
//
// TODO: Every namespace has an anonymous namespace.
//
// TODO: Handle using directives.
struct Namespace_decl : Decl
{
  Namespace_decl(Name&);
  Namespace_decl(Decl&, Name&);

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  bool is_global() const    { return cxt == nullptr; }
  bool is_anonymous() const;

  // Returns a list of members in this namespace.
  Decl_list const& members() const { return decls; }
  Decl_list&       members()       { return decls; }

  // Returns the totoal set of declarations within the namespace.
  Scope const* scope() const { return lookup; }
  Scope*       scope()       { return lookup; }

  Decl_list decls;
  Scope*    lookup;
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


// Represents the declaration of an axiom or semantic requirements.
struct Axiom_decl : Decl
{
  Axiom_decl(Name& n, Decl_list const& ds, Stmt& s)
    : Decl(n), parms(ds), reqs(&s)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the list of parameters in terms of which the requirements
  // are written.
  Decl_list const& parameters() const { return parms; }
  Decl_list&       parameters()       { return parms; }

  // Returns the list of requirements.
  Stmt const& body() const { return *reqs; }
  Stmt&       body()       { return *reqs; }

  Decl_list parms;
  Stmt*     reqs;
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
  Expr const& default_argument() const { return *init; }
  Expr&       default_argument()       { return *init; }

  bool has_default_arguement() const { return init; }
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
  Expr const& default_argument() const { return *init; }
  Expr&       default_argument()       { return *init; }

  bool has_default_arguement() const { return init; }
};


// Represents an unspecified sequence of arguments. This
// is distinct from a parameter pack.
//
// Note that we allow the variadic parameter to be named
// although the variadic parameter has a canonical name (...).
//
// TODO: Make this the type of an annamed parameter?
struct Variadic_parm : Decl
{
  Variadic_parm(Name& n)
    : Decl(n)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
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

  void visit(Variable_decl const& d)  { this->invoke(d); }
  void visit(Constant_decl const& d)  { this->invoke(d); }
  void visit(Function_decl const& d)  { this->invoke(d); }
  void visit(Class_decl const& d)     { this->invoke(d); }
  void visit(Union_decl const& d)     { this->invoke(d); }
  void visit(Enum_decl const& d)      { this->invoke(d); }
  void visit(Namespace_decl const& d) { this->invoke(d); }
  void visit(Template_decl const& d)  { this->invoke(d); }
  void visit(Concept_decl const& d)   { this->invoke(d); }
  void visit(Axiom_decl const& d)     { this->invoke(d); }
  void visit(Object_parm const& d)    { this->invoke(d); }
  void visit(Value_parm const& d)     { this->invoke(d); }
  void visit(Type_parm const& d)      { this->invoke(d); }
  void visit(Template_parm const& d)  { this->invoke(d); }
  void visit(Variadic_parm const& d)  { this->invoke(d); }
};


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl const&)>::type>
inline decltype(auto)
apply(Decl const& d, F fn)
{
  Generic_decl_visitor<F, T> vis(fn);
  return accept(d, vis);
}


// A generic visitor for names.
template<typename F, typename T>
struct Generic_decl_mutator : Decl::Mutator, Generic_mutator<F, T>
{
  Generic_decl_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Variable_decl& d)  { this->invoke(d); }
  void visit(Constant_decl& d)  { this->invoke(d); }
  void visit(Function_decl& d)  { this->invoke(d); }
  void visit(Class_decl& d)     { this->invoke(d); }
  void visit(Union_decl& d)     { this->invoke(d); }
  void visit(Enum_decl& d)      { this->invoke(d); }
  void visit(Namespace_decl& d) { this->invoke(d); }
  void visit(Template_decl& d)  { this->invoke(d); }
  void visit(Concept_decl& d)   { this->invoke(d); }
  void visit(Axiom_decl& d)     { this->invoke(d); }
  void visit(Object_parm& d)    { this->invoke(d); }
  void visit(Value_parm& d)     { this->invoke(d); }
  void visit(Type_parm& d)      { this->invoke(d); }
  void visit(Template_parm& d)  { this->invoke(d); }
  void visit(Variadic_parm& d)  { this->invoke(d); }
};


// Apply a function to the given declaration.
template<typename F, typename T = typename std::result_of<F(Variable_decl&)>::type>
inline decltype(auto)
apply(Decl& d, F fn)
{
  Generic_decl_mutator<F, T> vis(fn);
  return accept(d, vis);
}


} // namesapce banjo

#endif
