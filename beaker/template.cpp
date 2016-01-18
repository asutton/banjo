// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "template.hpp"
#include "initialization.hpp"
#include "substitution.hpp"
#include "builder.hpp"


namespace beaker
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

Decl& specialize_decl(Context&, Decl&, Term_list&);
Decl& specialize_decl(Context&, Variable_decl&, Term_list&);


// Specialize a templated declaration `d` (`d` is the pattern of
// a template).
//
// This is distinct from substitution. Here, we produce a new
// declaration with a distinct name. We do not, however, substitute
// into its initializer. That is done only when the the definition
// is actually needed for use.
//
// TODO: Finish implementing this.
Decl&
specialize_decl(Context& cxt, Decl& d, Term_list& args)
{
  struct fn
  {
    Context&   cxt;
    Term_list& args;
    Decl& operator()(Decl& d)           { lingo_unimplemented(); }
    Decl& operator()(Variable_decl& d)  { return specialize_decl(cxt, d, args); }
    Decl& operator()(Template_decl& d)  { lingo_unreachable(); }
  };
  return apply(d, fn{cxt, args});
}


// TODO: This is basically what happens for every single declaration.
// Find a way of generalizing it.
Decl&
specialize_decl(Context& cxt, Variable_decl& d, Term_list& orig)
{
  Builder build(cxt);

  // Convert parameter.
  Template_decl& tmp = cast<Template_decl>(*d.context());
  Decl_list& parms = tmp.parameters();
  Term_list args = initialize_template_parameters(cxt, parms, orig);

  // Create the specialization name.
  Name& n = build.get_id(tmp, args);

  // Substitute into the type.
  //
  // TODO: Don't substitute or re-declare if we've already
  // created a specialization for these arguments.
  Substitution sub(parms, args);
  Type& t = substitute(cxt, d.type(), sub);

  // TODO: Issue declaration or later?
  return build.make_variable(n, t);
}


// Produce an implicit specialization of the template declaration
// `d`, given a list of template arguments.
//
// Note that this only builds the declaration. It does not fully
// instantiate the definition.
Decl&
specialize_template(Context& cxt, Template_decl& tmp, Term_list& args)
{
  return specialize_decl(cxt, tmp.pattern(), args);
}


} // namespace beaker
