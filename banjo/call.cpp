// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "call.hpp"
#include "initialization.hpp"
#include "conversion.hpp"
#include "builder.hpp"


namespace banjo
{

// Initialize one parameter.
//
// The parameter initial parameter and argument are provided
// to compute parameter/argument offsets.
Expr&
initialize_parameter(Context& cxt,
                     Type_iter p0,
                     Type_iter pi,
                     Expr_iter a0,
                     Expr_iter ai)
{
  // TODO: Trap errors and use these to explain failures. If
  // we do get errors, what should we return?
  Expr& e = copy_initialize(cxt, *pi, *ai);

  // TODO: If the type of *p is the ellipsis parameter, do not
  // increment that iterator.
  ++pi;
  ++ai;

  return e;
}


Expr_list
initialize_parameters(Context& cxt, Type_list& parms, Expr_list& args)
{
  // TODO: Handle default arguments here.
  if (args.size() < parms.size())
    throw std::runtime_error("too few arguments");

  // Build a list of converted arguments by copy-initializing
  // each parameter in turn.
  Expr_list ret;
  Type_iter p0 = parms.begin(), pi = p0, pn = parms.end();
  Expr_iter a0 = args.begin(), ai = a0, an = args.end();
  while (pi != pn && ai != an) {
    Expr& e = initialize_parameter(cxt, p0, pi, a0, ai);
    ret.push_back(e);
  }

  return ret;
}


Function_candidate
build_function_candidate(Context& cxt, Function_decl& f, Expr_list& args)
{
  Type_list& parms = f.type().parameter_types();
  Expr_list conv = initialize_parameters(cxt, parms, args);
  return {f, conv, true};
}


// Build a function call candidate for the `f` given the list of
// function arguments.
//
// TODO: Synthesize default arguments if needed.
Expr&
build_function_call(Context& cxt, Function_decl& f, Expr_list& args)
{
  Builder build(cxt);
  Function_candidate c = build_function_candidate(cxt, f, args);
  return build.make_call(f.return_type(), f, c.arguments());
}


} // namespace banjo
