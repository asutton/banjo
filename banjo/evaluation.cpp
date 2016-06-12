// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "evaluation.hpp"
#include "ast.hpp"
#include "builder.hpp"

#include <iostream>


namespace banjo
{

// Returns a reference to the object or function corresponding
// do the declaration `d`.
//
// FIXME: If d refers to a global variable, then we may not
// have a binding for for it. We most definitely should.
Value
Evaluator::alias(Decl const& d)
{
  return Reference_value(&d);
}


// Load the value of an object corresponding to the given declaration.
// This produces a value associated with the name.
Value
Evaluator::load(Decl const& d)
{
  return stack.lookup(&d)->second;
}


// Allocate storage for the declaration. This effectively reserves
// storage for the object's associated value. The initial value is 
// an error value.
//
// TODO: The initial value should be an indeterminate value.
Value&
Evaluator::alloc(Decl const& d)
{
  return stack.top().bind(&d, Value{}).second;
}


// Stores a value in the object corresponding to the given
// declaration. This copies the value into the object, and
// returns a reference to that value.
//
// FIXME: We should not be able to store values in objects that
// have not been allocated.
Value&
Evaluator::store(Decl const& d, Value const& v)
{
  return stack.top().rebind(&d, v).second;
}


// -------------------------------------------------------------------------- //
// Evaluation of expressions

Value
Evaluator::evaluate(Expr const& e)
{
  struct fn
  {
    Evaluator& self;
    
    // FIXME: This should probably throw an evaluation error and not
    // result in an ICE. The idea is that only the expressions in this
    // switch can be CT-evaluated.
    Value operator()(Expr const& e) { lingo_unhandled(e); }
    
    Value operator()(Boolean_expr const& e) { return self.boolean(e); }
    Value operator()(Integer_expr const& e) { return self.integer(e); }
    Value operator()(Tuple_expr const& e)   { return self.tuple(e); }
    Value operator()(Object_expr const& e)  { return self.object(e); }
    Value operator()(Call_expr const& e)    { return self.call(e); }
    Value operator()(And_expr const& e)     { return self.logical_and(e); }
    Value operator()(Or_expr const& e)      { return self.logical_or(e); }
    Value operator()(Not_expr const& e)     { return self.logical_not(e); }

    Value operator()(Add_expr const& e)     { return self.add(e); }
    Value operator()(Sub_expr const& e)     { return self.sub(e); }
    Value operator()(Mul_expr const& e)     { return self.mul(e); }
    Value operator()(Div_expr const& e)     { return self.div(e); }
    Value operator()(Rem_expr const& e)     { return self.rem(e); }
    Value operator()(Pos_expr const& e)     { return self.pos(e); }
    Value operator()(Neg_expr const& e)     { return self.neg(e); }

    Value operator()(Eq_expr const& e)      { return self.eq(e); }
    Value operator()(Ne_expr const& e)      { return self.ne(e); }
    Value operator()(Lt_expr const& e)      { return self.lt(e); }
    Value operator()(Gt_expr const& e)      { return self.gt(e); }
    Value operator()(Le_expr const& e)      { return self.le(e); }
    Value operator()(Ge_expr const& e)      { return self.ge(e); }
    Value operator()(Cmp_expr const& e)     { return self.cmp(e); }
  };
  return apply(e, fn{*this});
}


Value
Evaluator::boolean(Boolean_expr const& e)
{
  return e.value();
}


Value
Evaluator::integer(Integer_expr const& e)
{
  // FIXME: Eh? Is this right? Does it matter? It wouldn't if the
  // value contained an actual integer.
  return Integer_value(e.value().getu());
}


Value
Evaluator::tuple(Tuple_expr const& e)
{
  Expr_list const& elems = e.elements();
  Aggregate_value ret(elems.size());
  for (std::size_t i = 0; i < elems.size(); ++i)
    ret[i] = evaluate(*elems[i]);
  return ret;
}


// Returns a reference to the object referred to be e.
Value
Evaluator::object(Object_expr const& e)
{
  return alias(e.declaration());
}


Value
Evaluator::call(Call_expr const& e)
{
  lingo_unreachable();
#if 0
  // FIXME: I believe that the function operand must be a reference to
  // a function.
  Value v = evaluate(e.function());
  Function_decl const& f = cast<Function_decl>(*v.get_reference());

  // There should probably be a body for the function.
  //
  // FIXME: What if the function is = default. How do we determine
  // what that behavior should be? Synthesize a new kind of definition
  // that explicitly performs that behavior?
  //
  // TODO: It would be more elegant to simply dispatch on the
  // definition rather than filter it here.
  Function_def const* def = as<Function_def>(&f.definition());
  if (!def)
    lingo_unreachable();

  // Each parameter is declared as a local variable within the
  // function.
  Enter_frame frame(*this);
  Expr_list const& args = e.arguments();
  Decl_list const& parms = f.parameters();
  auto ai = args.begin();
  auto pi = parms.begin();
  while (ai != args.end() && pi != parms.end()) {
    Expr const& arg = *ai;
    Decl const& parm = *pi;

    // TODO: Parameters are copy-initialized. 
    store(parm, evaluate(arg));
  }

  // Evaluate the function definition.
  //
  // TODO: Check result in case we've thrown an exception.
  //
  // FIXME: Failure to evaluate is a translation error, not
  // an internal error.
  Value result;
  Control ctl = evaluate(def->statement(), result);
  if (ctl != return_ctl)
    throw Evaluation_error("function evaluation failed");
  return result;
#endif
}


Value
Evaluator::logical_and(And_expr const& e)
{
  Value v = evaluate(e.left());
  if (!v.get_integer())
    return v;
  else
    return evaluate(e.right());
}


Value
Evaluator::logical_or(Or_expr const& e)
{
  Value v = evaluate(e.left());
  if (v.get_integer())
    return v;
  else
    return evaluate(e.right());
}


Value
Evaluator::logical_not(Not_expr const& e)
{
  Value v = evaluate(e.operand());
  return !v.get_integer();
}


// -------------------------------------------------------------------------- //
// Evaluation of arithmetic expressions
//
// TODO: This implementation assumes that all arithmetic operands have 
// integer values. However, we'll need to dispatch based on the type.
//
// TODO: Check for various forms of undefined behavior and throw an
// appropriate exception.

Value
Evaluator::add(Add_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() + v2.get_integer();
}


Value
Evaluator::sub(Sub_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() - v2.get_integer();
}


Value
Evaluator::mul(Mul_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() * v2.get_integer();
}


Value
Evaluator::div(Div_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() / v2.get_integer();
}


Value
Evaluator::rem(Rem_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() % v2.get_integer();
}


Value
Evaluator::pos(Pos_expr const& e)
{
  return evaluate(e.operand());
}


Value
Evaluator::neg(Neg_expr const& e)
{
  Value v = evaluate(e.operand());
  return -v.get_integer();
}


// -------------------------------------------------------------------------- //
// Evaluation of relational expressions
//
// TODO: This implementation assumes that all relational operands have 
// integer values. However, we'll need to dispatch based on the type.

Value
Evaluator::eq(Eq_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() == v2.get_integer();
}


Value
Evaluator::ne(Ne_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() != v2.get_integer();
}


Value
Evaluator::lt(Lt_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() < v2.get_integer();
}


Value
Evaluator::gt(Gt_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() > v2.get_integer();
}


Value
Evaluator::le(Le_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() <= v2.get_integer();
}


Value
Evaluator::ge(Ge_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  return v1.get_integer() >= v2.get_integer();
}


Value
Evaluator::cmp(Cmp_expr const& e)
{
  Value v1 = evaluate(e.left());
  Value v2 = evaluate(e.right());
  if (v1.get_integer() < v2.get_integer())
    return -1;
  if (v1.get_integer() > v2.get_integer())
    return 1;
  return 0;
}


// -------------------------------------------------------------------------- //
// Evaluation of conversions


// Given an object, convert it to a value. This loads the value stored
// previously by the object.
Value
Evaluator::to_value(Value_conv const& e)
{
  lingo_unreachable();
  // Value v = evaluate(e.source());
  // return load(*v.get_reference());
}


// -------------------------------------------------------------------------- //
// Evaluation of statements

// Evaluate the given statement, returning a
// control instruction, which determines how
// the evaluation proceeds. Storage is provided
// for a return value as an output argument.
Control
Evaluator::evaluate(Stmt const& s, Value& r)
{
  struct fn
  {
    Evaluator& self;
    Value&     r;

    Control operator()(Stmt const& s)             { lingo_unhandled(s); }
    Control operator()(Compound_stmt const& s)    { return self.evaluate_block(s, r); }
    Control operator()(Declaration_stmt const& s) { return self.evaluate_declaration(s, r); }
    Control operator()(Expression_stmt const& s)  { return self.evaluate_expression(s, r); }
    Control operator()(Return_stmt const& s)      { return self.evaluate_return(s, r); }
  };
  return apply(s, fn{*this, r});
}


Control
Evaluator::evaluate_block(Compound_stmt const& s, Value& r)
{
  Enter_frame frame(*this);
  for (Stmt const& s1 : s.statements()) {
    Control ctl = evaluate(s1, r);
    switch (ctl) {
      case return_ctl:
      case break_ctl:
      case continue_ctl:
        return ctl;
      default:
        break;
    }
  }
  return next_ctl;
}


Control
Evaluator::evaluate_declaration(Declaration_stmt const& s, Value& r)
{
  elaborate(s.declaration());
  return next_ctl;
}


Control
Evaluator::evaluate_expression(Expression_stmt const& s, Value& r)
{
  evaluate(s.expression());
  return next_ctl;
}


Control
Evaluator::evaluate_return(Return_stmt const& s, Value& r)
{
  r = Value(Void_value{});
  return return_ctl;
}


// -------------------------------------------------------------------------- //
// Evaluation of declarations.

void
Evaluator::elaborate(Decl const& d)
{
  struct fn
  {
    Evaluator& self;
    void operator()(Decl const& d)          { lingo_unhandled(d); }
    void operator()(Variable_decl const& d) { self.variable(d); }
  };
  return apply(d, fn{*this});
}


void
Evaluator::variable(Variable_decl const& decl)
{
  struct fn
  {
    Evaluator&  self;
    Decl const& decl;
    void operator()(Def const& d)            { lingo_unhandled(d); }
    void operator()(Expression_def const& d) { self.initialize(decl, d.expression()); }
  };
  apply(decl.initializer(), fn{*this, decl});
}


// Allocate storage for the declared object and then initialize it.
//
// TODO: Should allocation lay out the shape of the object in memory?
// Right now, we just build values based on initialization pattern. It
// works, but it's not quite true to the abstract machine.
void
Evaluator::initialize(Decl const& decl, Expr const& e)
{
  Value& obj = alloc(decl);
  initialize(obj, e);
}


void
Evaluator::initialize(Value& obj, Expr const& init)
{
  struct fn
  {
    Evaluator&  self;
    Value&      v;
    void operator()(Expr const& e)           { lingo_unhandled(e); }
    void operator()(Copy_init const& e)      { self.initialize(v, e); }
    void operator()(Aggregate_init const& e) { self.initialize(v, e); }
  };
  apply(init, fn{*this, obj});
}


// Copy the initial value into the stored object.
void
Evaluator::initialize(Value& obj, Copy_init const& init)
{
  obj = evaluate(init.expression());
}


// Build an aggregate value, initializing each element in turn.
//
// TODO: See the comments above on allocation. Instead of assigning
// to obj, we could be casting to an already create aggregate.
void
Evaluator::initialize(Value& obj, Aggregate_init const& init)
{
  Expr_list const& inits = init.initializers();
  Aggregate_value agg(inits.size());
  for (std::size_t i = 0; i < inits.size(); ++i) {
    agg[i] = Value{};
    initialize(agg[i], *inits[i]);
  }
  obj = agg;
}


// -------------------------------------------------------------------------- //
// Reduction

static Expr& lift_value(Context&, Type&, Value const&);


// FIXME: What is the location of this error?
static Expr& 
lift_error(Context& cxt, Type&, Error_value const& v) 
{
  error(cxt, "non-constant expression");
  throw Evaluation_error(); 
};


static Expr&
lift_integer(Context& cxt, Type& t, Integer_value const& v)
{
  if (is_integer_type(t))
    return cxt.get_integer(t, v); 
  if (is_boolean_type(t))
    return cxt.get_boolean(t, v);

  // TODO: What other kinds of integer representation do we have?
  lingo_unreachable();
}


// Construct a tuple expression from the values. 
//
// TODO: Note that T could be a tuple or class type. Not just a tuple
// type.
static Expr&
lift_aggregate(Context& cxt, Type& t, Aggregate_value const& v)
{
  // Only tuple types evaluate to tuples. We need the element
  // types to reconstruct the elements.
  Type_list& types = cast<Tuple_type>(t).element_types();
  
  // Populate the element list for the tuple.
  Expr_list elems;
  elems.resize(v.size());
  for (std::size_t i = 0; i < v.size(); ++i) {
    Type& t = *types[i];
    elems[i] = &lift_value(cxt, t, v[i]);
  }
  return cxt.make_tuple(t, std::move(elems));
}


static Expr&
lift_value(Context& cxt, Type& t, Value const& v)
{
  struct fn
  {
    fn(Context& c, Type& t)
      : cxt(c), type(t)
    { }

    Context& cxt;
    Type&    type;

    Expr& operator()(Error_value const& v)     { return lift_error(cxt, type, v); }
    Expr& operator()(Void_value const& v)      { lingo_unreachable(); }
    Expr& operator()(Integer_value const& v)   { return lift_integer(cxt, type, v); }
    Expr& operator()(Float_value const& v)     { lingo_unreachable(); }
    Expr& operator()(Reference_value const& v) { lingo_unreachable(); }
    Expr& operator()(Aggregate_value const& v) { return lift_aggregate(cxt, type, v); }
  };
  return apply(v, fn{cxt, t});
}


Expr&
reduce(Context& cxt, Expr& e)
{
  return lift_value(cxt, e.type(), evaluate(cxt, e));
}


Expr const&
reduce(Context& cxt, Expr const& e)
{
  return reduce(cxt, const_cast<Expr&>(e));
}


} // namespace banjo
