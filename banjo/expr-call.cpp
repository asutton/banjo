// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "type.hpp"
#include "lookup.hpp"
#include "initialization.hpp"
#include "debugging.hpp"

#include <iostream>


namespace banjo
{


// Initialize one parameter and advance the iterators.
//
// The parameter initial parameter and argument are provided to compute 
// parameter/argument indexes. Note that with packs and variadics the
// argument index may not match the parameter index.
Expr&
initialize_parameter(Context& cxt, 
                     Decl_iter p0, Decl_iter& pi, 
                     Expr_iter a0, Expr_iter& ai)
{
  // TODO: Trap errors and use these to explain failures. If we do get 
  // errors, what should we return? Note that we probably need the candidate 
  // in order to cache these for subsequent diagnosis.
  Expr& e = copy_initialize(cxt, *pi, *ai);

  // TODO: If the type of *p is an ellipsis or pack, do not increment 
  // that iterator.
  ++pi;
  ++ai;

  return e;
}


// Determine initialization patterns for all arguments.
Expr_list
initialize_parameters(Context& cxt, Decl_list& parms, Expr_list& args)
{
  // TODO: Handle default arguments here.
  if (args.size() < parms.size())
    throw std::runtime_error("too few arguments");

  // Build a list of converted arguments by copy-initializing
  // each parameter in turn.
  Expr_list ret;
  Decl_iter p0 = parms.begin(), pi = p0, pn = parms.end();
  Expr_iter a0 = args.begin(), ai = a0, an = args.end();
  while (pi != pn && ai != an) {
    Expr& e = initialize_parameter(cxt, p0, pi, a0, ai);
    ret.push_back(e);
  }

  return ret;
}


// -------------------------------------------------------------------------- //
// Overload resolution

// Build a candidate for the given function. The resulting candidate may
// be non-viable.
//
// TODO: Synthesize default arguments if needed.
//
// TODO: If the function is a template, we need to deduce arguments.
// Presumably, that would be done via an overload of this function that
// takes a Function_tmp.
Function_candidate
make_function_candidate(Context& cxt, Function_decl& f, Expr_list& args)
{
  try {
    std::cout << "-------\n";
    debug(f);
    Decl_list& parms = f.parameters();
    Expr_list conv = initialize_parameters(cxt, parms, args);
    std::cout << "^^^^^^^\n";
    return {f, conv, true};
  } 
  catch (Compiler_error& err) {
    // TOOD: We might want to copy the error into the candidate
    // so we can re-throw it later. For now, just do this.
    std::cout << "^^^^^^^\n";
    return {f, args, false};
  }
}


using Candidate_list = std::vector<Function_candidate>;


// Returns a list of all possible candidates, viable or not.
//
// TODO: It might be worthwhile making the distinction between gather
// and filter a little more clear. In particular, this would simply collect
// all of the needed declarations and filter would try to fit the
// arguments to the parameters.
static Candidate_list
gather_candidates(Context& cxt, Decl_list& ds, Expr_list& args)
{
  // Build the initial candidate list.
  Candidate_list cands;
  cands.reserve(ds.size());
  for (Decl& d : ds) {
    // For now, we work only with functions. Note that the current declaration
    // rules do not allow functions and classes to have the same names.
    if (!is<Function_decl>(d)) {
      error(cxt, "'{}' is not a function", d);
      throw Type_error();
    }

    Function_decl& fn = cast<Function_decl&>(d);

    // Suppress diagnostics when creating function candidates.
    Suppress_diagnostics diags(cxt);
    cands.emplace_back(make_function_candidate(cxt, fn, args));
  }
  return cands;
}


// Returns a list containing only the viable candidates.
static Candidate_list
filter_candidates(Candidate_list& cands)
{
  Candidate_list viable;
  viable.reserve(cands.size());
  for (Function_candidate& c : cands) {
    if (c) 
      viable.push_back(c);
  }
  return viable;
}


// Resolve a call to one of the functions or function templates in ds
// given a sequence of arguments.
//
// The result of resolution is the function candidate containing the 
// called function and its converted arguments.
Resolution
resolve_call(Context& cxt, Decl_list& ds, Expr_list& args)
{
  // Gather candidates
  Candidate_list cands = gather_candidates(cxt, ds, args);

  // Eliminate non-viable candidates.
  Candidate_list viable = filter_candidates(cands);

  // If there are no viable candidates, then resolution fails.
  if (viable.empty()) {
    error(cxt, "no viable candidates for '{}", ds.front().name());
    throw Type_error();
  }

  // If there is only one, that is the function called.
  if (viable.size() == 1)
    return viable.front();

  // TODO: Order candidates
  error(cxt, "ambiguous lookup for '{}'", ds.front().name());
  // for (Function_candidate& c : viable) {
  //   debug(c.function());
  //   for (Expr& e : c.arguments())
  //     debug(e);
  // }
  throw Type_error();
}


Resolution
resolve_call(Context& cxt, Decl_list& ds, Expr& e)
{
  Expr_list args {&e};
  return resolve_call(cxt, ds, args);
}


Resolution
resolve_call(Context& cxt, Decl_list& ds, Expr& e1, Expr& e2)
{
  Expr_list args {&e1, &e2};
  return resolve_call(cxt, ds, args);
}


// Perform overload resolution for the given operator and operands.
//
// TODO: All overloads of this function should be using a canonicalizing
// builder to generate the name for lookup.
Resolution 
resolve_operator(Context& cxt, Operator_kind op, Expr& e)
{
  Name& name = cxt.get_id(op);
  Decl_list decls = unqualified_lookup(cxt, name);
  return resolve_call(cxt, decls, e);
}


// Perform overload resolution for the given operator and operands.
Resolution 
resolve_operator(Context& cxt, Operator_kind op, Expr& e1, Expr& e2)
{
  Name& name = cxt.get_id(op);
  Decl_list decls = unqualified_lookup(cxt, name);
  return resolve_call(cxt, decls, e1, e2);
}


// Perform overload resolution for the given operator and operands.
Resolution 
resolve_operator(Context& cxt, Operator_kind op, Expr_list& args)
{
  Name& name = cxt.get_id(op);
  Decl_list decls = unqualified_lookup(cxt, name);
  return resolve_call(cxt, decls, args);
}


// -------------------------------------------------------------------------- //
// Function call
//
// This is a bit different than every other language (I think). Function
// call is resolved by searching for function call operators, even in cases
// where the called entity is actually a function.

// Build a call expression for the function denoted by f and the sequence
// of arguments in as.
Expr&
make_call(Context& cxt, Expr& f, Expr_list& args)
{
  // Build a new sequence of arguments that includes the call target.
  Expr_list ops {&f};
  for (Expr& a : args) 
    ops.push_back(a);

  // Search for function call operators.
  //
  // TODO: This is expensive, especially when we include argument dependent
  // lookup. We can use an alternative form of lookup that gathers operator
  // declarations directly from the entities or objects referred to by f.
  Name& name = cxt.get_id(call_op);
  Decl_list decls = unqualified_lookup(cxt, name);

  // Resolve the function call.
  Resolution res = resolve_operator(cxt, call_op, ops);
  Function_decl& decl = res.function();
  Expr_list& conv = res.arguments();

  // Unpack the function target and arguments from the converted arguments.
  //
  // TODO: Why am I bothering to unpack these?
  Expr& fn = conv[0];
  Expr_list cargs;
  for (auto iter = ++conv.begin(); iter != conv.end(); ++iter)
    cargs.push_back(*iter);

  // Build the call and set its resolved definition.
  Call_expr& expr = cxt.make_call(decl.return_type(), fn, std::move(cargs));
  expr.res_ = &decl;
  return expr;
}


} // namespace banjo
