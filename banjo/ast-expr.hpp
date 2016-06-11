// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_EXPR_HPP
#define BANJO_AST_EXPR_HPP

#include "ast-type.hpp"


namespace banjo
{

// This type is used to explicitly initialize an expression that does
// not have a computed type.
enum untyped_t
{
  untyped_expr
};


// The base class of all expressions.
struct Expr : Term
{
  struct Visitor;
  struct Mutator;

  Expr(Type& t)
    : type_(&t)
  { }

  Expr(untyped_t)
    : type_()
  { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  // Returns the type of the expression. This is valid only when 
  // is_typed() returns true.
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  // Returns true when the expression has a type. 
  bool is_typed() const { return type_; }

  Type* type_;
};


// The visitor for expressions.
struct Expr::Visitor
{
#define define_node(Node) virtual void visit(Node const&) = 0;
#include "ast-expr.def"
#undef define_node
};

struct Expr::Mutator
{
#define define_node(Node) virtual void visit(Node&) = 0;
#include "ast-expr.def"
#undef define_node
};


// The family of base classes for literal. This holds a value of the 
// parameterized type. All literal expressions are inherently value 
// expressions.
//
// TODO: Integrate this with a value system?
template<typename T>
struct Literal_expr : Expr
{
  Literal_expr(Type& t, T const& x)
    : Expr(t), val_(x)
  { }

  // Returns the interpreted value of the literal.
  T const& value() const { return val_; }
  T&       value()       { return val_; }

  T val_;
};


// An expression yielding the direct void value.
//
// Note that (currently) it is impossible to grammatically construct
// a void expression. 
struct Void_expr : Expr, Allocatable<Void_expr>
{
  Void_expr(Type& t)
    : Expr(t)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A boolean literal.
struct Boolean_expr : Literal_expr<bool>, Allocatable<Boolean_expr>
{
  Boolean_expr(Type& t, bool b)
    : Literal_expr<bool>(t, b)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An integer-valued literal.
struct Integer_expr : Literal_expr<Integer>, Allocatable<Integer_expr>
{
  Integer_expr(Type& t, Integer const& n)
    : Literal_expr<Integer>(t, n)
  { }

  Integer_expr(Type& t, Integer&& n)
    : Literal_expr<Integer>(t, std::move(n))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A real-valued literal.
//
// TODO: The shape of real-valued literals actually depends on the type.
// We may have support for fixed point types in the future.
struct Real_expr : Literal_expr<lingo::Real>, Allocatable<Real_expr>
{
  Real_expr(Type& t, Real const& n)
    : Literal_expr<Real>(t, n)
  { }

  Real_expr(Type& t, Real&& n)
    : Literal_expr<Real>(t, std::move(n))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents an immediate aggregate of other values. A tuple expression
// is inherently a value expression.
struct Tuple_expr : Expr, Allocatable<Tuple_expr>
{
  Tuple_expr(Type& t, Expr_list const& l)
    : Expr(t), elems_(l)
  { }

  Tuple_expr(Type& t, Expr_list&& l)
    : Expr(t), elems_(std::move(l))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr_list const& elements() const { return elems_; }
  Expr_list&       elements()       { return elems_; }

  Expr_list elems_;
};


// This is the base class of all expressions formed as a result of name
// lookup. Note that the name may or may not be resolved.
struct Id_expr : Expr
{
  Id_expr(Name& n)
    : Expr(untyped_expr), name_(&n)
  { }

  Id_expr(Type& t, Name& n)
    : Expr(t), name_(&n)
  { }

  // Returns the original id of the expression.
  Name const& id() const { return *name_; }
  Name&       id()       { return *name_; }

  Name* name_;
};


// A resolved id expression refers to a single declaration.
struct Resolved_id_expr : Id_expr
{
  Resolved_id_expr(Type& t, Name& n, Decl& d)
    : Id_expr(t, n), decl_(&d)
  { }

  // Returns the referenced declaration.
  Typed_decl const& declaration() const;
  Typed_decl&       declaration();

  Decl* decl_;
};


// A resolved reference to an object.
struct Object_expr : Resolved_id_expr, Allocatable<Object_expr>
{
  using Resolved_id_expr::Resolved_id_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced object.
  Object_decl const& declaration() const;
  Object_decl&       declaration();
};


// A resolved reference to a reference.
struct Reference_expr : Resolved_id_expr, Allocatable<Reference_expr>
{
  using Resolved_id_expr::Resolved_id_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced object.
  Reference_decl const& declaration() const;
  Reference_decl&       declaration();
};


// A resolved reference to a function.
struct Function_expr : Resolved_id_expr, Allocatable<Function_expr>
{
  using Resolved_id_expr::Resolved_id_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced object.
  Function_decl const& declaration() const;
  Function_decl&       declaration();
};


// A name that refers to a set of declarations. Overload expressions
// are inherently untyped.
struct Overload_expr : Id_expr, Allocatable<Overload_expr>
{
  Overload_expr(Name& n, Decl_list const& ds)
    : Id_expr(n), decls_(ds)
  { }

  Overload_expr(Name& n, Decl_list&& ds)
    : Id_expr(n), decls_(std::move(ds))
  { }
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced declaration.
  Decl_list const& declarations() const { return decls_; }
  Decl_list&       declarations()       { return decls_; }

  Decl_list decls_;
};


// The base class of all member access expressions.
struct Mem_expr : Expr
{
  Mem_expr(Expr& e, Name& n)
    : Expr(untyped_expr), obj_(&e), mem_(&n)
  { }

  Mem_expr(Type& t, Expr& e, Name& n)
    : Expr(t), obj_(&e), mem_(&n)
  { }

  // Returns the object enclosing the member name.
  Expr const& object() const { return *obj_; }
  Expr&       object()       { return *obj_; }

  // Returns the requested member name.
  Name const& member() const { return *mem_; }
  Name&       member()       { return *mem_; }
  
  Expr* obj_;
  Name* mem_;
};


// The base class of resolved dot-expressions. This stores the resolved
// declaration of the member name.
struct Resolved_mem_expr : Mem_expr
{
  Resolved_mem_expr(Type& t, Expr& e, Name& n, Decl& d)
    : Mem_expr(t, e, n), decl_(&d)
  { }

  Typed_decl const& declaration() const;
  Typed_decl&       declaration();

  Decl* decl_;
};


// A resolved reference to an object.
struct Mem_object_expr : Resolved_mem_expr, Allocatable<Mem_object_expr>
{
  using Resolved_mem_expr::Resolved_mem_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced object.
  Mem_object_decl const& declaration() const;
  Mem_object_decl&       declaration();
};


// A resolved reference to a reference.
struct Mem_reference_expr : Resolved_mem_expr, Allocatable<Mem_reference_expr>
{
  using Resolved_mem_expr::Resolved_mem_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced object.
  Mem_reference_decl const& declaration() const;
  Mem_reference_decl&       declaration();
};


// A resolved reference to a function.
struct Mem_function_expr : Resolved_mem_expr, Allocatable<Mem_function_expr>
{
  using Resolved_mem_expr::Resolved_mem_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced object.
  Mem_function_decl const& declaration() const;
  Mem_function_decl&       declaration();
};


// An unresolved access expression, referring to a set of declarations.
struct Mem_overload_expr : Mem_expr, Allocatable<Mem_overload_expr>
{
  Mem_overload_expr(Expr& e, Name& n, Decl_list const& ds)
    : Mem_expr(e, n), decls_(ds)
  { }

  Mem_overload_expr(Expr& e, Name& n, Decl_list&& ds)
    : Mem_expr(e, n), decls_(std::move(ds))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced declaration.
  Decl_list const& declarations() const { return decls_; }
  Decl_list&       declarations()       { return decls_; }

  Decl_list decls_;
};


// The base class of all unary expressions. This is parameterized
// by the derived class, providing default some default operations.
struct Unary_expr : Expr
{
  Unary_expr(Type& t, Expr& e)
    : Expr(t), op_(&e)
  { }

  // Returns the operand of the unary expression.
  Expr const& operand() const { return *op_; }
  Expr&       operand()       { return *op_; }

  Expr* op_;
};


// The base class of all binary expressions.
struct Binary_expr : Expr
{
  Binary_expr(Type& t, Expr& e1, Expr& e2)
    : Expr(t), left_(&e1), right_(&e2)
  { }

  Expr const& left() const { return *left_; }
  Expr&       left()       { return *left_; }

  Expr const& right() const { return *right_; }
  Expr&       right()       { return *right_; }

  Expr* left_;
  Expr* right_;
};


// An addition express.
struct Add_expr : Binary_expr, Allocatable<Add_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A subtraction expression.
struct Sub_expr : Binary_expr, Allocatable<Sub_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A multiplication expression.
struct Mul_expr : Binary_expr, Allocatable<Mul_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A division expression.
struct Div_expr : Binary_expr, Allocatable<Div_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A remainder expression.
struct Rem_expr : Binary_expr, Allocatable<Rem_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A negation expression.
struct Neg_expr : Unary_expr, Allocatable<Neg_expr>
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A identity expression.
struct Pos_expr : Unary_expr, Allocatable<Pos_expr>
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents a bitwise ad expression.
struct Bit_and_expr : Binary_expr, Allocatable<Bit_and_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwise inclusive-or expression.
struct Bit_or_expr : Binary_expr, Allocatable<Bit_or_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwsise exclusive-or expression.
struct Bit_xor_expr : Binary_expr, Allocatable<Bit_xor_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwise left-shift expression.
struct Bit_lsh_expr : Binary_expr, Allocatable<Bit_lsh_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwise-right shift expression.
struct Bit_rsh_expr : Binary_expr, Allocatable<Bit_rsh_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents bit-not (one's complement) expression.
struct Bit_not_expr : Unary_expr, Allocatable<Bit_not_expr>
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// An equality expression.
struct Eq_expr : Binary_expr, Allocatable<Eq_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An inequality expression.
struct Ne_expr : Binary_expr, Allocatable<Ne_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A less-than expression.
struct Lt_expr : Binary_expr, Allocatable<Lt_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A greater-than expression.
struct Gt_expr : Binary_expr, Allocatable<Gt_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A less-equal expression.
struct Le_expr : Binary_expr, Allocatable<Le_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A greater-equal expression.
struct Ge_expr : Binary_expr, Allocatable<Ge_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A 3-way comparison expression.
struct Cmp_expr : Binary_expr, Allocatable<Cmp_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A logical and expression.
struct And_expr : Binary_expr, Allocatable<And_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A logical or expression.
struct Or_expr : Binary_expr, Allocatable<Or_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Logical negation.
struct Not_expr : Unary_expr, Allocatable<Not_expr>
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
struct Call_expr : Expr, Allocatable<Call_expr>
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


// An assignment expression.
struct Assign_expr : Binary_expr, Allocatable<Assign_expr>
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An expression denoting a requirement for a valid syntax. Note that 
// the body can (and generally is) a compound statement. A requires
// expression is a value expression.
struct Requires_expr : Expr, Allocatable<Requires_expr>
{
  Requires_expr(Type& t, Decl_list const& tps, Decl_list const& nps, Req_list const& rs)
    : Expr(t), tparms(tps), nparms(nps), reqs(rs)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the list of parameters in terms of which the requirements
  // are written.
  Decl_list const& template_parameters() const { return tparms; }
  Decl_list&       template_parameters()       { return tparms; }

  // Returns the list of parameters in terms of which the requirements
  // are written.
  Decl_list const& normal_parameters() const { return nparms; }
  Decl_list&       normal_parameters()       { return nparms; }

  // Returns the list of requirements.
  Req_list const& requirements() const { return reqs; }
  Req_list&       requirements()       { return reqs; }

  Decl_list tparms;
  Decl_list nparms;
  Req_list  reqs;
};


// A synthesized value of a specified value. This refers to the
// declaration from which the expression was synthesized.
//
// TODO: Do we always need a declaration, or can we just synthesize
// values from thin air?
struct Synthetic_expr : Expr, Allocatable<Synthetic_expr>
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


// Represents an unparsed expression.
struct Unparsed_expr : Expr, Allocatable<Unparsed_expr>
{
  Unparsed_expr(Token_seq&& toks)
    : Expr(untyped_expr), toks(std::move(toks))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Token_seq const& tokens() const { return toks; }
  Token_seq&       tokens()       { return toks; }

  Token_seq toks;
};


// -------------------------------------------------------------------------- //
// Conversions

// Represents the set of standard conversions. A conversion has a source 
// expression and target type. Each derived conversion represents the 
// logic needed to transform the value computed by the source expression 
// into the target type. Note that the target type is also the type of the
// expression.
//
// All conversions are value expressions.
//
// TODO: Are there any conversions that yield references? Maybe in
// reference initialization, but those aren't technically conversions.
// Apparently, both Clang and Cppreference treat derived-to-base
// reference bindings as conversions.
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


// A grouping class. All standard conversions derive from this type.
// This is used to distinguish between standard and user-defined
// conversions when ordering conversions.
struct Standard_conv : Conv
{
  using Conv::Conv;
};


// A conversion from an reference to a value.
struct Value_conv : Standard_conv, Allocatable<Value_conv>
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from a less cv-qualified type to a more
// cv-qualified type.
struct Qualification_conv : Standard_conv, Allocatable<Qualification_conv>
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one integer type to another.
struct Boolean_conv : Standard_conv, Allocatable<Boolean_conv>
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one integer type to another.
struct Integer_conv : Standard_conv, Allocatable<Integer_conv>
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from one floating point type to another.
struct Float_conv : Standard_conv, Allocatable<Float_conv>
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from integer to floating point type.
//
// TODO: Integrate this with the float conversion?
struct Numeric_conv : Standard_conv, Allocatable<Numeric_conv>
{
  using Standard_conv::Standard_conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A conversion from a type-dependent expression to some
// other type. When instantiated, an implicit conversion must
// be applied.
struct Dependent_conv : Conv, Allocatable<Dependent_conv>
{
  using Conv::Conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the conversion of an argument type the ellipsis
// parameter.
struct Ellipsis_conv : Conv, Allocatable<Ellipsis_conv>
{
  using Conv::Conv;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// -------------------------------------------------------------------------- //
// Initializers

// An initializer is an expression that provides a value for a variable. 
// or constant declaration. All initializers have type void and are
// value expressions.
struct Init : Expr
{
  Init(Type& t)
    : Expr(t)
  { }
};


// Represents the absence of initialization for an object. This
// is selected by zero initialization of references and by the
// default construction of trivially constructible class and union
// types.
struct Trivial_init : Init, Allocatable<Trivial_init>
{
  using Init::Init;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// Represents the initialization of an object by an expression.
// The result of the expression is copied (or moved?) into a
// target object.
struct Copy_init : Init, Allocatable<Copy_init>
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
struct Bind_init : Init, Allocatable<Bind_init>
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
struct Direct_init : Init, Allocatable<Direct_init>
{
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


// Represents the recursive initialization of a class, union,
// or array object.
struct Aggregate_init : Init, Allocatable<Aggregate_init>
{
  Aggregate_init(Type& t, Expr_list const& es)
    : Init(t), inits(es)
  { }

  Aggregate_init(Type& t, Expr_list&& es)
    : Init(t), inits(std::move(es))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns a sequence of selected initializers for a compound target 
  // type. Each of these is an initializer.
  Expr_list const& initializers() const { return inits; }
  Expr_list&       initializers()       { return inits; }

  Expr_list inits;
};


// -------------------------------------------------------------------------- //
// Operations on expressions

bool has_bool_type(Expr const&);
bool has_integer_type(Expr const&);
bool has_floating_point_type(Expr const&);
bool has_array_type(Expr const&);
bool has_tuple_type(Expr const&);
bool has_class_type(Expr const&);

bool is_type_dependent(Expr const&);
bool is_type_dependent(Expr_list const&);

Type const& declared_type(Expr const&);
Type&       declared_type(Expr&);


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

#define define_node(Node) void visit(Node const& t) { this->invoke(t); }
#include "ast-expr.def"
#undef define_node
};


// A generic mutator for expressions.
template<typename F, typename T>
struct Generic_expr_mutator : Expr::Mutator, Generic_mutator<F, T>
{
  Generic_expr_mutator(F f)
    : Generic_mutator<F, T>(f)
  { }

#define define_node(Node) void visit(Node& t) { this->invoke(t); }
#include "ast-expr.def"
#undef define_node
};


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_expr const&)>::type>
inline decltype(auto)
apply(Expr const& e, F fn)
{
  Generic_expr_visitor<F, T> vis(fn);
  return accept(e, vis);
}


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Void_expr&)>::type>
inline decltype(auto)
apply(Expr& e, F fn)
{
  Generic_expr_mutator<F, T> vis(fn);
  return accept(e, vis);
}


} // namesapce banjo


#endif
