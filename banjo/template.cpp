// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "template.hpp"
#include "initialization.hpp"
#include "substitution.hpp"
#include "deduction.hpp"
#include "print.hpp"
#include "builder.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Template argument matching

// TODO: Is there anything else to do here?
Type&
initialize_type_template_parameter(Context& cxt, Type_parm& p, Term& a)
{
  if (!is<Type>(&a))
    throw std::runtime_error("argument is not a type");
  return cast<Type>(a);
}


// TODO: Is there anything else to do here? Perhaps verify that
// the argument is also a constant expressions!
Expr&
initialize_value_template_parameter(Context& cxt, Value_parm& p, Term& a)
{
  if (!is<Expr>(&a))
    throw std::runtime_error("argument is not a value");
  return copy_initialize(cxt, p.type(), cast<Expr>(a));
}


// TODO: Implement me.
Decl&
initialize_template_template_parameter(Context& cxt, Template_parm& p, Term& t)
{
  lingo_unimplemented();
}


// Return a converted template argument.
Term&
initialize_template_parameter(Context& cxt,
                              Decl_iter p0,
                              Decl_iter& pi,
                              Term_iter a0,
                              Term_iter& ai)
{
  // TODO: Trap kind/type errors and emit good diagnostics.
  Term* c;
  if (Type_parm* p = as<Type_parm>(&*pi))
    c = &initialize_type_template_parameter(cxt, *p, *ai);
  else if (Value_parm* p = as<Value_parm>(&*pi))
    c = &initialize_value_template_parameter(cxt, *p, *ai);
  else if (Template_parm* p = as<Template_parm>(&*pi))
    c = &initialize_template_template_parameter(cxt, *p, *ai);
  else
    lingo_unreachable();

  // TODO: Handle parameter packs. How would these be
  // represented? Note that this isn't just the non-advancement
  // of the parameter, but the packing of subsequent arguments
  // into a new pack argument.
  ++pi;
  ++ai;

  return *c;
}


// Return a list of converted template arguments.
Term_list
initialize_template_parameters(Context& cxt, Decl_list& parms, Term_list& args)
{
  // TODO: Handle default arguments here.
  if (args.size() < parms.size())
    throw std::runtime_error("too few template arguments");

  // Build a list of converted template arguments by initializing
  // each parameter in turn.
  Term_list ret;
  Decl_iter p0 = parms.begin(), pi = p0, pn = parms.end();
  Term_iter a0 = args.begin(), ai = a0, an = args.end();
  while (pi != pn && ai != an) {
    Term& e = initialize_template_parameter(cxt, p0, pi, a0, ai);

    // TODO: If pi is a pack, then we want to merge e into
    // a single pack argument so that so that the number of
    // parameters and arguments conform.
    ret.push_back(e);
  }

  return ret;
}

// -------------------------------------------------------------------------- //
// Template specialization

// TODO: This is basically what happens for every single declaration.
// Find a way of generalizing it.
Decl&
specialize_variable(Context& cxt, Template_decl& tmp, Variable_decl& d, Term_list& orig)
{
  Builder build(cxt);

  // Convert parameters.
  Decl_list& parms = tmp.parameters();
  Term_list args = initialize_template_parameters(cxt, parms, orig);

  // Create the specialization name.
  Name& n = build.get_template_id(tmp, args);

  // Substitute into the type.
  //
  // TODO: Don't substitute or re-declare if we've already
  // created a specialization for these arguments.
  Substitution sub(parms, args);
  Type& t = substitute(cxt, d.type(), sub);

  return build.make_variable(n, t);
}


Decl&
specialize_function(Context& cxt, Template_decl& tmp, Function_decl& d, Term_list& orig)
{
  Builder build(cxt);

  // Convert parameters.
  Decl_list& tparms = tmp.parameters();
  Term_list targs = initialize_template_parameters(cxt, tparms, orig);

  // Create the specialization name.
  Name& n = build.get_template_id(tmp, targs);

  // Substitute into the type.
  //
  // TODO: Don't substitute or re-declare if we've already
  // created a specialization for these arguments.
  Substitution sub(tparms, targs);

  // Substitute through parameters.
  //
  // TODO: I think I need to re-establish name bindings during substitution
  // because we are going to be resolving types at the same time. This
  // means that I am going to have to move scoping facilities from the
  // parser to the context (which makes some sense).
  Decl_list parms;
  for (Decl& p1 : d.parameters()) {
    Decl& p2 = substitute(cxt, p1, sub);
    parms.push_back(p2);
  }

  // Substitute through the return type.
  Type& ret = substitute(cxt, d.return_type(), sub);

  return build.make_function(n, parms, ret);
}


Decl&
specialize_class(Context& cxt, Template_decl& tmp, Class_decl& d, Term_list& orig)
{
  Builder build(cxt);

  // Convert parameters.
  Decl_list& parms = tmp.parameters();
  Term_list args = initialize_template_parameters(cxt, parms, orig);

  Name& n = build.get_template_id(tmp, args);

  return build.make_class(n);
}


// Specialize a templated declaration `decl` (`decl` is parameterized
// by the template `tmp`).
//
// This is distinct from substitution. Here, we produce a new declaration
// with a distinct name. We do not, however, substitute into its
// initializer. That is done only when the the definition is actually
// needed for use.
//
// TODO: Finish implementing this.
Decl&
specialize_declaration(Context& cxt, Template_decl& tmp, Decl& decl, Term_list& args)
{
  struct fn
  {
    Context&       cxt;
    Template_decl& tmp;
    Term_list&     args;
    Decl& operator()(Decl& d)           { lingo_unimplemented(); }
    Decl& operator()(Variable_decl& d)  { return specialize_variable(cxt, tmp, d, args); }
    Decl& operator()(Function_decl& d)  { return specialize_function(cxt, tmp, d, args); }
    Decl& operator()(Class_decl& d)     { return specialize_class(cxt, tmp, d, args); }
    Decl& operator()(Template_decl& d)  { lingo_unreachable(); }
  };
  return apply(decl, fn{cxt, tmp, args});
}


// Produce an implicit specialization of the template declaration
// `d`, given a list of template arguments.
//
// Note that this only builds the declaration. It does not fully
// instantiate the definition.
Decl&
specialize_template(Context& cxt, Template_decl& tmp, Term_list& args)
{
  Decl& decl = tmp.parameterized_declaration();
  return specialize_declaration(cxt, tmp, decl, args);
}


// -------------------------------------------------------------------------- //
// Synthesis of template arguments from parameters

Type&
synthesize_template_argument(Context& cxt, Type_parm& parm)
{
  Builder build(cxt);
  return build.synthesize_type(parm);
}


Expr&
synthesize_template_argument(Context& cxt, Value_parm& parm)
{
  Builder build(cxt);
  return build.synthesize_expression(parm);
}


// TODO: Synthesize a template declaration with exactly the
// the parameters of the template parameter.
Decl&
synthesize_template_argument(Context& cxt, Template_parm& parm)
{
  lingo_unimplemented();
}


// Synthesize a unique type, value, or template from a corresponding
// template parameter.
//
// TODO: Handle template parameter packs.
Term&
synthesize_template_argument(Context& cxt, Decl& parm)
{
  if (Type_parm* t = as<Type_parm>(&parm))
    return synthesize_template_argument(cxt, *t);
  if (Value_parm* e = as<Value_parm>(&parm))
    return synthesize_template_argument(cxt, *e);
  if (Template_parm* x = as<Template_parm>(&parm))
    return synthesize_template_argument(cxt, *x);
  lingo_unreachable();
}


// Synthesize a list of template arguments from a list of
// template parameter list.
Term_list
synthesize_template_arguments(Context& cxt, Decl_list& parms)
{
  Term_list args;
  args.reserve(parms.size());
  for (Decl& p : parms) {
    Term& arg = synthesize_template_argument(cxt, p);
    args.push_back(arg);
  }
  return args;
}


// -------------------------------------------------------------------------- //
// Partial ordering of function templates


// Produce a transformed function template type for a function
// template `tmp`.
Function_type&
transform_template_type(Context& cxt, Template_decl& tmp)
{
  Function_decl& fn = cast<Function_decl>(tmp.parameterized_declaration());
  Decl_list& parms = tmp.parameters();

  // Synthesize a unique type, value, or template for each parameter.
  Term_list args = synthesize_template_arguments(cxt, parms);

  // TODO: For member functions, we also need to substitute the
  // implic this object. That gets synthesized from the enclosing
  // class (or class template specialization) of the member.

  // Replace each parameter with a synthesized argument in the
  // fucntion type.
  Substitution sub(parms, args);
  Type& t = substitute(cxt, fn.type(), sub);
  return cast<Function_type>(t);
}


// FIXME. Make derived classes Function_temp that saves me from
// typing all this crap.
inline Function_type&
get_function_type(Template_decl& t)
{
  Function_decl& f = cast<Function_decl>(t.parameterized_declaration());
  return f.type();
}


// FIXME: This is a bad name.
using Deductions = std::pair<Type_list, Type_list>;


// Select the set of types from which deduction is performed.
//
// FIXME: From different contexts, we will return different lists.
// For function calls, we use only parameters for which arguments
// have been specified. For conversions, we use the return types.
// For everything else, we just use t.
//
// For now, I am simply assuming that all parameters are used.
//
// FIXME: What do I do with non-dependent parameters?
Deductions
nominate_types_for_ordering(Function_type& p, Function_type& a)
{
  return {p.parameter_types(), a.parameter_types()};
}


// Certain transformations are applied before ordering.
// In particular, remove references and qualfiiers. We'll appeal
// to the original types as tie-breakers later.
Type&
transform_type_for_ordering(Type& t)
{
  return t.non_reference_type().unqualified_type();
}


// Determine whether tmp1 is more specialized than tmp2, or vice
// versa.
//
// FIXME: We also want to return if tmp1 and tmp2 are equivalent or
// specialized or unordered.
//
// FIXME: When ordering from a function call, we only perform deductions
// for parameters that have explicit function call arguments. No packs
// or default arguments apply. I wonder if we can truncate the transformed
// function type by "trimming" those un-deduced parameters.
Partial_ordering
more_specialized_call(Context& cxt, Template_decl& tmp1, Template_decl& tmp2)
{
  Function_type& p1 = get_function_type(tmp1);
  Function_type& p2 = get_function_type(tmp2);

  // Transform the type of each template.
  Function_type& a1 = transform_template_type(cxt, tmp1);
  Function_type& a2 = transform_template_type(cxt, tmp2);

  // Nominate types to be used inthe partial ordering depending
  // on context.
  Deductions ts1 = nominate_types_for_ordering(p1, a1);
  Deductions ts2 = nominate_types_for_ordering(p2, a2);

  // Determine a partial ordering of the nominated types.
  for (std::size_t i = 0; i < ts1.first.size(); ++i) {
    // Set up the first deduction.
    Type_list& ps1 = ts1.first;
    Type_list& as1 = ts2.second;
    Type& p1 = transform_type_for_ordering(*ps1[i]);
    Type& a1 = transform_type_for_ordering(*as1[i]);

    // Set up the second deduction.
    Type_list& ps2 = ts2.first;
    Type_list& as2 = ts1.second;
    Type& p2 = transform_type_for_ordering(*ps2[i]);
    Type& a2 = transform_type_for_ordering(*as2[i]);

    // FIXME: Fail if px is a pack and ax is not.

    // Perform each deduction in turn.
    Substitution s1 = deduce_from_type(p1, a1);
    Substitution s2 = deduce_from_type(p2, a2);

    // FIXME: Check that the ordering is correct. It's probably
    // wrong at the moment.
    if (s1 && !s2)
      return lt_ord;
    if (s2 && !s1)
      return gt_ord;
    if (!s1 && !s2)
      return un_ord;

    // These
    // Tie-brekers

    std::cout << s1 << s2 << '\n';
  }

  return {};
}

} // namespace banjo
