// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "template.hpp"
#include "initialization.hpp"
#include "substitution.hpp"
#include "builder.hpp"


namespace beaker
{

// TODO: Is there anything else to do here?
Type&
initialize_type_template_parameter(Context& cxt, Type_parm& p, Term& t)
{
  if (!is<Type>(&t))
    throw std::runtime_error("argument is not a type");
  return cast<Type>(&t);
}


// TODO: Is there anything else to do here?
Type&
initialize_value_template_parameter(Context& cxt, Type_parm& p, Term& t)
{
  if (!is<Expr>(&a))
    throw std::runtime_error("argument is not a value");
  Expr& a = cast<Expr&>(t);
  return copy_initialize(cxt, p, a);
}


// TODO: Implement me.
Type&
initialize_template_template_parameter(Context& cxt, Type_parm& p, Term& t)
{
  lingo_unimplemented();
}


// Return a converted template argument.
Term&
initialize_template_parameter(Context& cxt,
                              Decl_iter p0,
                              Decl_iter pi,
                              Term_iter a0,
                              Term_iter ai)
{
  // TODO: Trap kind/type errors and emit good diagnostics.
  Term* c;
  if (Type_parm* p = as<Type_parm>(&*pi))
    c = &initialize_type_template_parameter(*p, *ai);
  else if (Value_parm* p = as<Value_parm>(&*pi))
    c = &initialize_value_template_parameter(*p, *ai);
  else if (Template_parm* p = as<Template_parm>(&*pi))
    c = &initialize_template_template_parameter(*p, *ai);
  else
    lingo_unreachable();

  // TODO: Handle parameter packs. How would these be
  // represented?
  ++pi;
  ++ci;
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
    ret.push_back(e);
  }

  return ret;
}


// Produce an implicit specialization of the template declaration
// `d`, given a list of template arguments.
//
// Note that this only builds the declaration. It does not fully
// instantiate the definition.
Decl&
build_specialization(Context& cxt, Template_decl& tmp, Term_list& args)
{
  Decl_list& parms = tmp.parameters();
  Term_list conv = initialize_template_parameters(cxt, parms, args);

  throw std::runtime_error("not implemented");
}


} // namespace beaker
