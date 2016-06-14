// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "type.hpp"
#include "lookup.hpp"
#include "initialization.hpp"

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
    Decl_list& parms = f.parameters();
    Expr_list conv = initialize_parameters(cxt, parms, args);
    return {f, conv, true};
  } 
  catch (Compiler_error& err) {
    // TOOD: We might want to copy the error into the candidate
    // so we can re-throw it later. For now, just do this.
    return {f, args, false};
  }
}


#if 0
// Attempt to resolve a dependent call to a (single) function template.
Expr&
make_dependent_template_call(Context& cxt, Template_ref& e, Expr_list& args)
{
  Template_decl& temp = e.declaration();

  Decl& pd = temp.parameterized_declaration();

  // FIXME: Factor this into smaller bits. We'll need it when we try
  // to handle overloads.
  if (Function_decl* f = as<Function_decl>(&pd)) {
    // Perform template argument deduction using the parameters of
    // the function template and the given arguments.
    Substitution sub(temp.parameters());
    try {
      deduce_from_call(cxt, f->parameters(), args, sub);
      Decl& tspec = specialize_template(cxt, temp, sub);
      Function_decl& spec = cast<Function_decl>(tspec);
      Type& t = spec.return_type();

      // If the template is constrained, then ensure that the current
      // constraints subsume those of the declaration.
      //
      // Note that an unconstrained template is awlays admissible.
      //
      // TODO: Do this here, or in specialize_template?
      if (temp.is_constrained()) {
        Expr& fcons = temp.constraint();
        Expr& ccons = *cxt.current_template_constraints();

        // The call is admissible iff the current constraints subsume
        // those of the the candidate function.
        if (!subsumes(cxt, ccons, fcons))
          warning(cxt, "call to function template '{}' not covered by constraints", e);
      }
      return cxt.make_call(t, e, args);
    } catch (Translation_error& err) {
      // FIXME: Improve diagnostics.
      throw Type_error("no matching call to 'e'");
    }
  }
  lingo_unimplemented("dependent function call");
}


// Make a dependent function call expression.
Expr&
make_dependent_call(Context& cxt, Expr& e, Expr_list& args)
{
  Type& t = make_fresh_type(cxt);
  Expr& init = cxt.make_call(t, e, args);

  // Unify with previous requirements.
  if (cxt.in_requirements())
    return make_required_expression(cxt, init);

  // Don't check in unconstrained templates.
  if (cxt.in_unconstrained_template())
    return init;

  // Determine if the constraints explicitly admit this declaration.
  Expr& cons = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, cons, init))
    return *ret;

  // Otherwise, e refers to a previous declaration, possibly many.
  //
  // TODO: Use dispatch?
  //
  // TODO: Handle the overload case. For overloaded funtions, we
  // need to find them most general.
  if (Template_ref* r = as<Template_ref>(&e))
    return make_dependent_template_call(cxt, *r, args);
  if (Reference_expr* r = as<Reference_expr>(&e))
    return make_dependent_function_call(cxt, *r, args);

  lingo_unhandled(e);
}
#endif



Expr&
make_regular_call(Context& cxt, Decl_ref& e, Expr_list& args)
{
  // TODO: This could be a function object!.
  Typed_decl& decl = e.declaration();
  if (!is<Function_decl>(decl)) {
    error(cxt, "call to non-function '{}'", decl);
    throw Type_error();
  }

  // TODO: We should be searching for or synthesizing a call operator that 
  // accepts functions of (specifically) this type -- as opposed to generating
  // the call indirectly. Perhaps we should just attach a declaration to
  // each overloadable operation that contains the resolved declaration to
  // call. This would allow us to preserve the surface syntax while also
  // supporting generalized lookup rules.

  // TODO: The call operator takes an argument of function type (as in the
  // category, not the value type). This means we are generally going to be
  // performing a reference-to-function conversion.

  Function_decl& fn = cast<Function_decl>(decl);
  Function_candidate cand = make_function_candidate(cxt, fn, args);
  if (!cand) {
    // TODO: Diagnose the error at the call site. Also explain why.
    error(cxt, "cannot call function '{}", fn.name());
    throw Lookup_error();
  }

  // The type is the declared return type of the function.
  Type& t = fn.return_type();

  return cxt.make_call(t, e, args);
}


// Make a non-dependent call expression.
//
// FIXME: Allow calls to expressions of any function type.
//
// NOTE: The builtin function call operator requires an object-to-value
// conversion on the target. Be sure to apply that.
//
// FIXME: If e has user-defined type then we need to look for an
// overloaded operator.
Expr&
make_regular_call(Context& cxt, Expr& e, Expr_list& args)
{
  struct fn 
  {
    Context&    cxt;
    Expr_list& args;
    Expr& operator()(Expr& e)     { lingo_unhandled(e); }
    Expr& operator()(Decl_ref& e) { return make_regular_call(cxt, e, args); }
  };
  return apply(e, fn{cxt, args});
}


Expr&
make_call(Context& cxt, Expr& e, Expr_list& args)
{
  // if (is_type_dependent(e) || is_type_dependent(args))
  //   return make_dependent_call(cxt, e, args);
  // else
  //   return make_regular_call(cxt, e, args);
  return make_regular_call(cxt, e, args);
}


// -------------------------------------------------------------------------- //
// Overload resolution

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


} // namespace banjo
