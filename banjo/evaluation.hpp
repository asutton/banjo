// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_EVALUATOR_HPP
#define BANJO_EVALUATOR_HPP

#include "prelude.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "value.hpp"

#include <lingo/environment.hpp>


namespace banjo
{

// TODO: Specialize the store and call stack to use load and store
// instead of lookup and bind. That's a bit more conventional for
// the kinds of operations here.


// Dynamic binding of declarations to their values.
using Store = Environment<Decl const*, Value>;


// The store stack maintains the dynamic binding between symbols
// and their values. Each time a new binding environment is entered
// (e.g., a block scope), a new frame is created to store bindings.
//
// This is also the call stack.
using Call_stack = Stack<Store>;


// Represents the evaluation of a statement. This determines the
// next action to be taken.
enum Control
{
  next_ctl,
  return_ctl,
  break_ctl,
  continue_ctl,
};


// The evaluator is responsible for the interpretation  of a program as a 
// value.
//
// FIXME: 
struct Evaluator
{
public:
  Value operator()(Expr const& e) { return evaluate(e); }

  Value evaluate(Expr const&);

  Value boolean(Boolean_expr const&);
  Value integer(Integer_expr const&);
  Value tuple(Tuple_expr const&);
  Value ref(Decl_expr const&);
  Value call(Call_expr const&);
  Value logical_and(And_expr const&);
  Value logical_or(Or_expr const&);
  Value logical_not(Not_expr const&);

  Value add(Add_expr const&);
  Value sub(Sub_expr const&);
  Value mul(Mul_expr const&);
  Value div(Div_expr const&);
  Value rem(Rem_expr const&);
  Value pos(Pos_expr const&);
  Value neg(Neg_expr const&);

  Value eq(Eq_expr const&);
  Value ne(Ne_expr const&);
  Value lt(Lt_expr const&);
  Value gt(Gt_expr const&);
  Value le(Le_expr const&);
  Value ge(Ge_expr const&);
  Value cmp(Cmp_expr const&);

  Control evaluate(Stmt const&, Value&);
  Control evaluate_block(Compound_stmt const&, Value&);
  Control evaluate_declaration(Declaration_stmt const&, Value&);
  Control evaluate_expression(Expression_stmt const&, Value&);
  Control evaluate_return(Return_stmt const&, Value&);

  void elaborate(Decl const&);
  void elaborate_object(Object_decl const&);

  // Memory management
  Value  alias(Decl const&);
  Value  load(Decl const&);
  Value& store(Decl const&, Value const&);
  Value& alloca(Decl const&);

  struct Enter_frame;

  Call_stack stack;
};


// A helper class for managing stack frames.
struct Evaluator::Enter_frame
{
  Enter_frame(Evaluator& e)
    : eval(e)
  {
    eval.stack.push();
  }

  ~Enter_frame()
  {
    eval.stack.pop();
  }

  Evaluator& eval;
};


// -------------------------------------------------------------------------- //
// Expression evaluation

// Evaluate the given expression.
inline Value
evaluate(Expr const& e)
{
  Evaluator eval;
  return eval(e);
}


Expr const& reduce(Context&, Expr const&);
Expr&       reduce(Context&, Expr&);


} // namespace banjo


#endif
