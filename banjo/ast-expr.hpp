// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_EXPR_HPP
#define BANJO_AST_EXPR_HPP

#include "ast-base.hpp"


namespace banjo
{

// This type is used to explicitly initialize an expression that does
// not have a computed type.
enum untyped_t
{
  untyped
};


// The base class of all expresions.
struct Expr : Term
{
  struct Visitor;
  struct Mutator;

  Expr(Type& t)
    : type_(&t)
  { }

  Expr(untyped_t)
    : type_(nullptr)
  { }

  virtual void accept(Visitor&) const = 0;
  virtual void accept(Mutator&) = 0;

  // Returns the type of the expression. 
  // This is valid only when  is_typed() returns true.
  Type const& type() const { return *type_; }
  Type&       type()       { return *type_; }

  // Returns true when the expression has a type. This is generally
  // the case.
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


// The base class of all ids that refer to declarations. 
struct Id_expr : Expr
{
  Id_expr(Name& n)
    : Expr(untyped), name_(&n)
  { }

  Id_expr(Type& t, Name& n)
    : Expr(t), name_(&n)
  { }

  // Returns the original id of the expression.
  Name const& id() const { return *name_; }
  Name&       id()       { return *name_; }

  Name* name_;
};


// Represents a tuple value; a sequence of expressions comprising a
// single value.
struct Tuple_expr : Expr
{
  Tuple_expr(Type& t, Expr_list const& l)
    : Expr(t), elems(l)
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Expr_list const& elements() const { return elems; }
  Expr_list&       elements()       { return elems; }

  Expr_list elems;
};



// The base class of all identifiers that resolved to a single declaration.
struct Decl_expr : Id_expr
{
  Decl_expr(Type& t, Name& n, Decl& d)
    : Id_expr(t, n), decl_(&d)
  { }
  
  // Returns the referenced declaration.
  Decl const& declaration() const { return *decl_; }
  Decl&       declaration()       { return *decl_; }

  Decl* decl_;
};


// A name that refers to a variable or parameter.
struct Object_expr : Decl_expr
{
  using Decl_expr::Decl_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced variable or parameter.
  Object_decl const& declaration() const;
  Object_decl&       declaration();
};


// A name that refers to a function.
struct Function_expr : Decl_expr
{
  using Decl_expr::Decl_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced function.
  Function_decl const& declaration() const;
  Function_decl&       declaration();
};


// A name that refers to a set of declarations.
struct Overload_expr : Id_expr
{
  Overload_expr(Name& n, Overload_set& o)
    : Id_expr(n), ovl_(&o)
  { }
  
  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  // Returns the referenced declaration.
  Overload_set const& declarations() const { return *ovl_; }
  Overload_set&       declarations()       { return *ovl_; }

  Overload_set* ovl_;
};


// The base class of all dot expressions. 
struct Dot_expr : Expr
{
  Dot_expr(Expr& e, Name& n)
    : Expr(untyped), obj_(&e), mem_(&n)
  { }

  Dot_expr(Type& t, Expr& e, Name& n)
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
struct Nested_decl_expr : Dot_expr
{
  Nested_decl_expr(Type& t, Expr& e, Name& n, Decl& d)
    : Dot_expr(t, e, n), decl_(&d)
  { }

  Decl const& declaration() const { return *decl_; }
  Decl&       declaration()       { return *decl_; }

  Decl* decl_;
};


// A dot-expression that has been resolved to a member variable 
// (or field) of a record type.
struct Field_expr : Nested_decl_expr
{
  using Nested_decl_expr::Nested_decl_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// A dot-expression that has been resolved to a member function
// (or method) of a record type.
struct Method_expr : Nested_decl_expr
{
  using Nested_decl_expr::Nested_decl_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
};


// An unresolved dot-expression that refers to a overload set.
struct Member_expr : Dot_expr
{
  using Dot_expr::Dot_expr;

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }
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


// Represents a bitwise ad expression.
struct Bit_and_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwise inclusive-or expression.
struct Bit_or_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwsise exclusive-or expression.
struct Bit_xor_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwise left-shift expression.
struct Bit_lsh_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents a bitwise-right shift expression.
struct Bit_rsh_expr : Binary_expr
{
  using Binary_expr::Binary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
};


// Represents bit-not (one's complement) expression.
struct Bit_not_expr : Unary_expr
{
  using Unary_expr::Unary_expr;

  void accept(Visitor& v) const { return v.visit(*this); }
  void accept(Mutator& v)       { return v.visit(*this); }
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


// A 3-way comparison expression.
struct Cmp_expr : Binary_expr
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


// Represents an unparsed expression.
struct Unparsed_expr : Expr
{
  Unparsed_expr(Token_seq&& toks)
    : Expr(untyped), toks(std::move(toks))
  { }

  void accept(Visitor& v) const { v.visit(*this); }
  void accept(Mutator& v)       { v.visit(*this); }

  Token_seq const& tokens() const { return toks; }
  Token_seq&       tokens()       { return toks; }

  Token_seq toks;
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


// A conversion from a type-dependent expression to some
// other type. When instantiated, an implicit conversion must
// be applied.
struct Dependent_conv : Conv
{
  using Conv::Conv;

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
bool has_tuple_type(Expr const&);
bool has_class_type(Expr const&);
bool has_union_type(Expr const&);

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
template<typename F, typename T = typename std::result_of<F(Boolean_expr const&)>::type>
inline decltype(auto)
apply(Expr const& e, F fn)
{
  Generic_expr_visitor<F, T> vis(fn);
  return accept(e, vis);
}


// Apply a function to the given type.
template<typename F, typename T = typename std::result_of<F(Boolean_expr&)>::type>
inline decltype(auto)
apply(Expr& e, F fn)
{
  Generic_expr_mutator<F, T> vis(fn);
  return accept(e, vis);
}


} // namesapce banjo


#endif
