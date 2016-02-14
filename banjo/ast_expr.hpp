// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_EXPR_HPP
#define BANJO_AST_EXPR_HPP

#include "ast_base.hpp"


namespace banjo
{

// The base class of all expresions.
struct Expr : Term
{
  struct Visitor;
  struct Mutator;

  Expr()
    : ty(nullptr)
  { }

  Expr(Type& t)
    : ty(&t)
  { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  Type const& type() const { return *ty; }
  Type&       type()       { return *ty; }

  Type* ty;
};


// The visitor for expressions.
struct Expr::Visitor
{
  virtual void visit(Boolean_expr const&) { }
  virtual void visit(Integer_expr const&) { }
  virtual void visit(Real_expr const&) { }
  virtual void visit(Reference_expr const&) { }
  virtual void visit(Check_expr const&) { }
  virtual void visit(Add_expr const&) { }
  virtual void visit(Sub_expr const&) { }
  virtual void visit(Mul_expr const&) { }
  virtual void visit(Div_expr const&) { }
  virtual void visit(Rem_expr const&) { }
  virtual void visit(Neg_expr const&) { }
  virtual void visit(Pos_expr const&) { }
  virtual void visit(Eq_expr const&) { }
  virtual void visit(Ne_expr const&) { }
  virtual void visit(Lt_expr const&) { }
  virtual void visit(Gt_expr const&) { }
  virtual void visit(Le_expr const&) { }
  virtual void visit(Ge_expr const&) { }
  virtual void visit(And_expr const&) { }
  virtual void visit(Or_expr const&) { }
  virtual void visit(Not_expr const&) { }
  virtual void visit(Call_expr const&) { }
  virtual void visit(Assign_expr const&) { }
  virtual void visit(Requires_expr const&) { }
  virtual void visit(Synthetic_expr const&) { }
  virtual void visit(Value_conv const&) { }
  virtual void visit(Qualification_conv const&) { }
  virtual void visit(Boolean_conv const&) { }
  virtual void visit(Integer_conv const&) { }
  virtual void visit(Float_conv const&) { }
  virtual void visit(Numeric_conv const&) { }
  virtual void visit(Ellipsis_conv const&) { }
  virtual void visit(Trivial_init const&) { }
  virtual void visit(Copy_init const&) { }
  virtual void visit(Bind_init const&) { }
  virtual void visit(Direct_init const&) { }
  virtual void visit(Aggregate_init const&) { }
};

struct Expr::Mutator
{
  virtual void visit(Boolean_expr&) { }
  virtual void visit(Integer_expr&) { }
  virtual void visit(Real_expr&) { }
  virtual void visit(Reference_expr&) { }
  virtual void visit(Check_expr&) { }
  virtual void visit(Add_expr&) { }
  virtual void visit(Sub_expr&) { }
  virtual void visit(Mul_expr&) { }
  virtual void visit(Div_expr&) { }
  virtual void visit(Rem_expr&) { }
  virtual void visit(Neg_expr&) { }
  virtual void visit(Pos_expr&) { }
  virtual void visit(Eq_expr&) { }
  virtual void visit(Ne_expr&) { }
  virtual void visit(Lt_expr&) { }
  virtual void visit(Gt_expr&) { }
  virtual void visit(Le_expr&) { }
  virtual void visit(Ge_expr&) { }
  virtual void visit(And_expr&) { }
  virtual void visit(Or_expr&) { }
  virtual void visit(Not_expr&) { }
  virtual void visit(Call_expr&) { }
  virtual void visit(Assign_expr&) { }
  virtual void visit(Requires_expr&) { }
  virtual void visit(Synthetic_expr&) { }
  virtual void visit(Value_conv&) { }
  virtual void visit(Qualification_conv&) { }
  virtual void visit(Boolean_conv&) { }
  virtual void visit(Integer_conv&) { }
  virtual void visit(Float_conv&) { }
  virtual void visit(Numeric_conv&) { }
  virtual void visit(Ellipsis_conv&) { }
  virtual void visit(Trivial_init&) { }
  virtual void visit(Copy_init&) { }
  virtual void visit(Bind_init&) { }
  virtual void visit(Direct_init&) { }
  virtual void visit(Aggregate_init&) { }
};


// The family of base classes for literal. This holds a value
// of the parameterized type.
//
// TODO: Integrate this with a value system?
template<typename T>
struct Literal_expr : Expr
{
  Literal_expr(Type& t, T const& x)
    : Expr(t), val(x)
  { }

  // Returns the interpreted value of the literal.
  T const& value() const { return val; }
  T&       value()       { return val; }

  T val;
};


// The base class of all unary expressions.
struct Unary_expr : Expr
{
  Unary_expr(Type& t, Expr& e)
    : Expr(t), first(&e)
  { }

  // Returns the operand of the unary expression.
  Expr const& operand() const { return *first; }
  Expr&       operand()       { return *first; }

  Expr* first;
};


// The base class of all binary expressions.
struct Binary_expr : Expr
{
  Binary_expr(Type& t, Expr& e1, Expr& e2)
    : Expr(t), first(&e1), second(&e2)
  { }

  Expr const& left() const { return *first; }
  Expr&       left()       { return *first; }

  Expr const& right() const { return *second; }
  Expr&       right()       { return *second; }

  Expr* first;
  Expr* second;
};


// A boolean literal.
struct Boolean_expr : Literal_expr<bool>
{
  using Literal_expr<bool>::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An integer-valued literal.
struct Integer_expr : Literal_expr<Integer>
{
  using Literal_expr<Integer>::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A real-valued literal.
struct Real_expr : Literal_expr<lingo::Real>
{
  using Literal_expr<Real>::Literal_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A reference to a single declaration.
//
// TODO: Subclass for variables, constants, and functions.
// Unresolved identifiers are also interesting. These should be
// called Variable_expr, Constant_expr, and Function_expr,
// respectively.
struct Reference_expr : Expr
{
  Reference_expr(Type& t, Decl& d)
    : Expr(t), decl(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced declaration.
  Decl const& declaration() const { return *decl; }
  Decl&       declaration()       { return *decl; }

  Decl* decl;
};


// Represents the satisfaction of a concept by a sequence
// of template arguments. Unlike a concept-id, this is
// resolved to a single concept.
struct Check_expr : Expr
{
  Check_expr(Type& t, Decl& d, Term_list const& a)
    : Expr(t), con(&d), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Concept_decl const& declaration() const;
  Concept_decl&       declaration();

  Term_list const& arguments() const { return args; }
  Term_list&       arguments()       { return args; }

  Decl*     con;
  Term_list args;
};


// An addition express.
struct Add_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A subtraction expression.
struct Sub_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A multiplication expression.
struct Mul_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A division expression.
struct Div_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A remainder expression.
struct Rem_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A negation expression.
struct Neg_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A identity expression.
struct Pos_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An equality expression.
struct Eq_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An inequality expression.
struct Ne_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A less-than expression.
struct Lt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A greater-than expression.
struct Gt_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A less-equal expression.
struct Le_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A greater-equal expression.
struct Ge_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A logical and expression.
struct And_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A logical or expression.
struct Or_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Logical negation.
struct Not_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents a function call expression of the form `f(args...)`.
// The function arguments are initializers for the declared parameters.
//
// If the `f` and `args` are non-dependent, then `f` must refer
// to a function declaration, `args...` must be the converted
// and instantiated default arguments, and the type is the return
// type of the function.
//
// If the operands are dependent, then the retun type is a fresh
// placeholder type.
//
// TODO: Consider subtyping for [virtual|open| method calls and
// unresolved calls. It would simplify code generation.
struct Call_expr : Expr
{
  Call_expr(Type& t, Expr& e, Expr_list const& a)
    : Expr(t), fn(&e), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr const& function() const { return *fn; }
  Expr&       function()       { return *fn; }

  Expr_list const& arguments() const { return args; }
  Expr_list&       arguments()       { return args; }

  Expr*     fn;
  Expr_list args;
};


// An assignment expresion.
struct Assign_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An expression denoting a requirement for a valid syntax.
// Note that the body can (and generally is) a compound
// statement.
struct Requires_expr : Expr
{
  Requires_expr(Type& t, Decl_list const& ds, Stmt& s)
    : Expr(t), parms(ds), reqs(&s)
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


// A synthesized value of a specified value. This refers to the
// declaration from which the expression was synthesized.
//
// TODO: Do we always need a declaration, or can we just synthesize
// values from thin air?
struct Synthetic_expr : Expr
{
  Synthetic_expr(Type& t, Decl& d)
    : Expr(t), decl(&d)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the declaration from which this expression was
  // synthesized.
  Decl const& declaration() const { return *decl; }
  Decl&       declaration()       { return *decl; }

  Decl* decl;
};


// -------------------------------------------------------------------------- //
// Conversions

// Represents the set of standard conversions. A conversion
// has a source expression and target type. Each derived
// conversion contains the logic needed to transform the
// value computed by the source expression into the target
// type. Note that the target type is also the type of the
// expression.
//
// FIXME: Embed as much information in type conversions as possible.
// Essentially we want to duplicate the set of conversions that
// would be applied in the IR. That frees us from dealing with
// complex code generation implementations.
struct Conv : Expr
{
  Conv(Type& t, Expr& e)
    : Expr(t), expr(&e)
  { }

  // Returns the destination type of the conversion. This is the
  // same as the expression's type.
  Type const& destination() const { return type(); }
  Type&       destination()       { return type(); }

  // Returns the converted expression (the operand).
  Expr const& source() const { return *expr; }
  Expr&       source()       { return *expr; }

  Expr* expr;
};


// A grouping class. All standard conversions derive frrom
// this type.
struct Standard_conv : Conv
{
  using Conv::Conv;
};


// A conversion from an object to a value.
struct Value_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from a less cv-qualified type to a more
// cv-qualified type.
struct Qualification_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one integer type to another.
struct Boolean_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one integer type to another.
struct Integer_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one floating point type to another.
struct Float_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from integer to floating point type.
//
// TODO: Integrate this with the float conversion?
struct Numeric_conv : Standard_conv
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the conversion of an argument type the ellipsis
// parameter.
struct Ellipsis_conv : Conv
{
  using Conv::Conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// -------------------------------------------------------------------------- //
// Initializers

// An initializer is an expressio that provides a value for an
// object or reference.
//
// TODO: Should all initializers have type void?
//
// FIXME: Merge initializers back into definitions. They work
// better there.
struct Init : Expr
{
  using Expr::Expr;
};


// Represents the absence of initialization for an object. This
// is selected by zero initialization of references and by the
// default constrution of trivially constructible class and union
// types.
struct Trivial_init : Init
{
  using Init::Init;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the initialization of an object by an expression.
// The result of the expression is copied (or moved?) into a
// target object.
struct Copy_init : Init
{
  Copy_init(Type& t, Expr& e)
    : Init(t), expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the source expression.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// Represents the initialization of a reference by an expression.
// The declared reference is bound to the result of the expression.
struct Bind_init : Init
{
  Bind_init(Type& t, Expr& e)
    : Init(t), expr(&e)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the source expression.
  Expr const& expression() const { return *expr; }
  Expr&       expression()       { return *expr; }

  Expr* expr;
};


// Represents the initialization of an class or union object by a
// constructor.
struct Direct_init : Init
{
  // FIXME: The type of this expression is the type of the
  // constructed object. We should be able to compute this
  // instead of passing it directly.
  Direct_init(Type& t, Decl& d, Expr_list const& a)
    : Init(t), ctor(&d), args(a)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the constructor declaration
  Decl const& consructor() const { return *ctor; }
  Decl&       consructor()       { return *ctor; }

  // Returns the argumetns supplied for direct initialization.
  Expr_list const& arguments() const { return args; }
  Expr_list&       arguments()       { return args; }

  Decl*     ctor;
  Expr_list args;
};


// Represents the recursive initialziation of a class, union,
// or array object.
struct Aggregate_init : Init
{
  Aggregate_init(Type& t, Expr_list const& es)
    : Init(t), inits(es)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns a sequence of selected initializers for
  // a compound target type.
  Expr_list const& initializers() const { return inits; }
  Expr_list&       initializers()       { return inits; }

  Expr_list inits;
};


// -------------------------------------------------------------------------- //
// Operations on expressions

bool has_bool_type(Expr const&);
bool has_integer_type(Expr const&);
bool has_floating_point_type(Expr const&);
bool has_reference_type(Expr const&);
bool has_array_type(Expr const&);
bool has_class_type(Expr const&);
bool has_union_type(Expr const&);


// -------------------------------------------------------------------------- //
// Operations on conversions

// Returns true if the expression is a standard conversion.
inline bool
is_standard_conversion(Expr const& e)
{
  return is<Standard_conv>(&e);
}


// Returns true if the expression is an ellipsis conversion.
inline bool
is_ellipsis_conversion(Expr const& e)
{
  return is<Ellipsis_conv>(&e);
}


// -------------------------------------------------------------------------- //
// Visitors

// A generic visitor for expressions.
template<typename F, typename T>
struct Generic_expr_visitor : Expr::Visitor, Generic_visitor<F, T>
{
  Generic_expr_visitor(F f)
    : Generic_visitor<F, T>(f)
  { }

  void visit(Boolean_expr const& e)       { this->invoke(e); }
  void visit(Integer_expr const& e)       { this->invoke(e); }
  void visit(Real_expr const& e)          { this->invoke(e); }
  void visit(Reference_expr const& e)     { this->invoke(e); }
  void visit(Check_expr const& e)         { this->invoke(e); }
  void visit(Add_expr const& e)           { this->invoke(e); }
  void visit(Sub_expr const& e)           { this->invoke(e); }
  void visit(Mul_expr const& e)           { this->invoke(e); }
  void visit(Div_expr const& e)           { this->invoke(e); }
  void visit(Rem_expr const& e)           { this->invoke(e); }
  void visit(Neg_expr const& e)           { this->invoke(e); }
  void visit(Pos_expr const& e)           { this->invoke(e); }
  void visit(Eq_expr const& e)            { this->invoke(e); }
  void visit(Ne_expr const& e)            { this->invoke(e); }
  void visit(Lt_expr const& e)            { this->invoke(e); }
  void visit(Gt_expr const& e)            { this->invoke(e); }
  void visit(Le_expr const& e)            { this->invoke(e); }
  void visit(Ge_expr const& e)            { this->invoke(e); }
  void visit(And_expr const& e)           { this->invoke(e); }
  void visit(Or_expr const& e)            { this->invoke(e); }
  void visit(Not_expr const& e)           { this->invoke(e); }
  void visit(Call_expr const& e)          { this->invoke(e); }
  void visit(Assign_expr const& e)        { this->invoke(e); }
  void visit(Requires_expr const& e)      { this->invoke(e); }
  void visit(Synthetic_expr const& e)     { this->invoke(e); }
  void visit(Value_conv const& e)         { this->invoke(e); }
  void visit(Qualification_conv const& e) { this->invoke(e); }
  void visit(Boolean_conv const& e)       { this->invoke(e); }
  void visit(Integer_conv const& e)       { this->invoke(e); }
  void visit(Float_conv const& e)         { this->invoke(e); }
  void visit(Numeric_conv const& e)       { this->invoke(e); }
  void visit(Ellipsis_conv const& e)      { this->invoke(e); }
  void visit(Trivial_init const& e)       { this->invoke(e); }
  void visit(Copy_init const& e)          { this->invoke(e); }
  void visit(Bind_init const& e)          { this->invoke(e); }
  void visit(Direct_init const& e)        { this->invoke(e); }
  void visit(Aggregate_init const& e)     { this->invoke(e); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Boolean_expr const&)>::type>
inline T
apply(Expr const& e, F fn)
{
  Generic_expr_visitor<F, T> vis(fn);
  return accept(e, vis);
}


// A generic mutator for expressions.
template<typename F, typename T>
struct Generic_expr_mutator : Expr::Mutator, Generic_mutator<F, T>
{
  Generic_expr_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

  void visit(Boolean_expr& e)       { this->invoke(e); }
  void visit(Integer_expr& e)       { this->invoke(e); }
  void visit(Real_expr& e)          { this->invoke(e); }
  void visit(Reference_expr& e)     { this->invoke(e); }
  void visit(Check_expr& e)         { this->invoke(e); }
  void visit(Add_expr& e)           { this->invoke(e); }
  void visit(Sub_expr& e)           { this->invoke(e); }
  void visit(Mul_expr& e)           { this->invoke(e); }
  void visit(Div_expr& e)           { this->invoke(e); }
  void visit(Rem_expr& e)           { this->invoke(e); }
  void visit(Neg_expr& e)           { this->invoke(e); }
  void visit(Pos_expr& e)           { this->invoke(e); }
  void visit(Eq_expr& e)            { this->invoke(e); }
  void visit(Ne_expr& e)            { this->invoke(e); }
  void visit(Lt_expr& e)            { this->invoke(e); }
  void visit(Gt_expr& e)            { this->invoke(e); }
  void visit(Le_expr& e)            { this->invoke(e); }
  void visit(Ge_expr& e)            { this->invoke(e); }
  void visit(And_expr& e)           { this->invoke(e); }
  void visit(Or_expr& e)            { this->invoke(e); }
  void visit(Not_expr& e)           { this->invoke(e); }
  void visit(Call_expr& e)          { this->invoke(e); }
  void visit(Assign_expr& e)        { this->invoke(e); }
  void visit(Requires_expr& e)      { this->invoke(e); }
  void visit(Synthetic_expr& e)     { this->invoke(e); }
  void visit(Value_conv& e)         { this->invoke(e); }
  void visit(Qualification_conv& e) { this->invoke(e); }
  void visit(Boolean_conv& e)       { this->invoke(e); }
  void visit(Integer_conv& e)       { this->invoke(e); }
  void visit(Float_conv& e)         { this->invoke(e); }
  void visit(Numeric_conv& e)       { this->invoke(e); }
  void visit(Ellipsis_conv& e)      { this->invoke(e); }
  void visit(Trivial_init& e)       { this->invoke(e); }
  void visit(Copy_init& e)          { this->invoke(e); }
  void visit(Bind_init& e)          { this->invoke(e); }
  void visit(Direct_init& e)        { this->invoke(e); }
  void visit(Aggregate_init& e)     { this->invoke(e); }
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Boolean_expr&)>::type>
inline T
apply(Expr& e, F fn)
{
  Generic_expr_mutator<F, T> vis(fn);
  return accept(e, vis);
}


} // namesapce banjo


#endif
