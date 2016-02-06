// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "evaluation.hpp"
#include "ast.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Evaluation of expressions

Value
Evaluator::evaluate(Expr const& e)
{
  struct fn
  {
    Evaluator& self;
    Value operator()(Expr const& e) { banjo_unhandled_case(e); }
    Value operator()(Boolean_expr const& e) { return self.evaluate_boolean(e); }
    Value operator()(Integer_expr const& e) { return self.evaluate_integer(e); }
    Value operator()(Reference_expr const& e) { return self.evaluate_reference(e); }
    Value operator()(Call_expr const& e) { return self.evaluate_call(e); }
    Value operator()(And_expr const& e) { return self.evaluate_and(e); }
    Value operator()(Or_expr const& e) { return self.evaluate_or(e); }
    Value operator()(Not_expr const& e) { return self.evaluate_not(e); }
  };
  return apply(e, fn{*this});
}


Value
Evaluator::evaluate_boolean(Boolean_expr const& e)
{
  return e.value();
}


Value
Evaluator::evaluate_integer(Integer_expr const& e)
{
  // FIXME: Eh? Is this right? Does it matter? It wouldn't if the
  // value contained an actual integer.
  return Integer_value(e.value().getu());
}


// Returns the object or function referred to by the
// given declaration.
Value
Evaluator::evaluate_reference(Reference_expr const& e)
{
  Decl const& d = e.declaration();

  // If the expression refers to an object, then produce
  // a reference to its stored value.
  if (Object_decl const* var = as<Object_decl>(&d))
    return &stack.lookup(var)->second;

  // If the expression refers to a function, then produce
  // a reference to that function.
  if (Function_decl const* fn = as<Function_decl>(&d))
    return fn;

  // Is there anything else?
  banjo_unhandled_case(e);
}


Value
Evaluator::evaluate_call(Call_expr const& e)
{
  Value v = evaluate(e.function());
  Function_decl const& f = *v.get_function();

  // Get the function's definition.
  if (!f.is_definition())
    throw Internal_error("function '{}' is not defined", f.name());

  // There should probably be a body for the function.
  //
  // FIXME: What if the function is = default. How do we determine
  // what that behavior should be? Synthesize a new kind of defintion
  // that explicitly performs that behavior?
  //
  // TODO: It would be more elegant to simply dispatch on the
  // definition rather than filter it here.
  Function_def const* def = as<Function_def>(&f.definition());
  if (!def)
    lingo_unimplemented();

  // Evaluate each argument in turn.
  //
  // FIXME: See the comments below.
  Value_list args;
  args.reserve(e.arguments().size());
  for (Expr const& a : e.arguments())
    args.push_back(evaluate(a));

  // Each parameter is declared as a local variable within the
  // function.
  //
  // FIXME: Parameters are copy-initialized. We should be sure
  // that we're actually doing the right thing here. Also,
  // why isn't this merged with the one above?
  Enter_frame frame(*this);
  for (std::size_t i = 0; i < args.size(); ++i)
    stack.top().bind(f.parameters()[i], args[i]);

  // Evaluate the function definition.
  //
  // TODO: Check result in case we've thrown an exception.
  //
  // FIXME: Failure to evaluate is a translation error, not
  // an internal error.
  Value result;
  Control ctl = evaluate(def->statement(), result);
  if (ctl != return_ctl)
    throw Internal_error("function evaluation failed");
  return result;
}


Value
Evaluator::evaluate_and(And_expr const& e)
{
  Value v = evaluate(e.left());
  if (!v.get_integer())
    return v;
  else
    return evaluate(e.right());
}


Value
Evaluator::evaluate_or(Or_expr const& e)
{
  Value v = evaluate(e.left());
  if (v.get_integer())
    return v;
  else
    return evaluate(e.right());
}


Value
Evaluator::evaluate_not(Not_expr const& e)
{
  Value v = evaluate(e.operand());
  return !v.get_integer();
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

    Control operator()(Compound_stmt const& s) { return self.evaluate_block(s, r); }
    Control operator()(Declaration_stmt const& s) { return self.evaluate_declaration(s, r); }
    Control operator()(Expression_stmt const& s) { return self.evaluate_expression(s, r); }
    Control operator()(Return_stmt const& s) { return self.evaluate_return(s, r); }
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
  r = evaluate(s.expression());
  return return_ctl;
}


// -------------------------------------------------------------------------- //
// Evaluation of declarations

namespace
{

// Allocate a value whose shape is determined by the type. No
// guarantees are made about the contents of the resulting value.
Value
make_object(Type const& t)
{
  struct Fn
  {
    Value operator()(Type const& t) { banjo_unhandled_case(t); }

    // Return a scalar object.
    Value operator()(Boolean_type const&) { return 0; }
    Value operator()(Integer_type const&) { return 0; }
    Value operator()(Float_type const&)   { return 0.0; }

    // Return a function object.
    Value operator()(Function_type const&) { return Function_value(nullptr); }

    // Return a reference object.
    Value operator()(Reference_type const&) { return Reference_value(nullptr); }
  };
  return apply(t, Fn{});
}

} // namespace


void
Evaluator::elaborate(Decl const& d)
{
  struct fn
  {
    Evaluator& self;
    void operator()(Decl const& d)        { banjo_unhandled_case(d); }
    void operator()(Object_decl const& d) { self.elaborate_object(d); }
  };
  return apply(d, fn{*this});
}


void
Evaluator::elaborate_object(Object_decl const& d)
{
  // Create an uninitialized object and bind it
  // to the symbol. Keep a reference so we can
  // initialize it directly.
  Value v0 = make_object(d.type());
  Value& v1 = stack.top().bind(&d, v0).second;

  // FIXME: Actually initialize the object. Note that an initializer
  // is conceptually a call to a constructor.
  (void)v1;
  // eval_init(d->init(), v1);
}


} // namespace banjo
