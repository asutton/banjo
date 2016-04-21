// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "deduction.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "template.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Deducing template arguments from a type

bool
deduce_from_type(Reference_type& p, Type& a, Substitution& sub)
{
  if (Reference_type* t = as<Reference_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
  return false;
}


bool
deduce_from_type(Qualified_type& p, Type& a, Substitution& sub)
{
  if (Qualified_type* t = as<Qualified_type>(&a)) {
    if (p.qualifier() == t->qualifier())
      return deduce_from_type(p.type(), t->type(), sub);
  }
  return false;
}


bool
deduce_from_type(Pointer_type& p, Type& a, Substitution& sub)
{
  if (Pointer_type* t = as<Pointer_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
  return false;
}


// Deduce arguments when both types have the form T[].
//
// Note that this form of deduction is not available in C++ since
// arrays decay to pointers.
bool
deduce_from_type(Sequence_type& p, Type& a, Substitution& sub)
{
  if (Sequence_type* t = as<Sequence_type>(&a))
    return deduce_from_type(p.type(), t->type(), sub);
  return false;
}


// Deduce an assignment of a type parrameter to a corresponding
// argument type. For example, given
//
//    p: T
//    a: int const
//
// This will deduce the substitution of T -> int const. If a prior
// and different assignment was deduced for T, template argument
// deduction fails.
//
// FIXME: This is totally incorrect. We can only match T against T.
bool
deduce_from_type(Typename_type& p, Type& a, Substitution& sub)
{
  Decl& d = p.declaration();
  if (sub.has_mapping(d)) {
    if (Type* t = as<Type>(sub.get_mapping(d))) {
      if (!is_equivalent(a, *t))
        return false;
    } else {
      sub.map_to(d, a);
    }
    return true;
  }
  return false;
}


// Find a substitution from template parameters in `p` to template
// arguments in `a`.
//
// TODO: This is clearly incomplete. Finish implementing template
// argument deduction.
//
// TODO: For any templated type, we need to perform unification
// against their respective ids.
bool
deduce_from_type(Type& p, Type& a, Substitution& sub)
{
  struct fn
  {
    Type& a;
    Substitution& sub;

    // We can't deduce from most type patterns.
    bool operator()(Type& p)           { return true; }

    bool operator()(Auto_type& p)      { lingo_unreachable(); }
    bool operator()(Decltype_type& p)  { lingo_unreachable(); }
    bool operator()(Declauto_type& p)  { lingo_unreachable(); }
    bool operator()(Function_type& p)  { lingo_unreachable(); }
    bool operator()(Reference_type& p) { return deduce_from_type(p, a, sub); }
    bool operator()(Qualified_type& p) { return deduce_from_type(p, a, sub); }
    bool operator()(Pointer_type& p)   { return deduce_from_type(p, a, sub); }
    bool operator()(Array_type& p)     { lingo_unreachable(); }
    bool operator()(Tuple& p)          { lingo_unreachable(); }
    bool operator()(Dynarray_type& p)  { lingo_unreachable(); }
    bool operator()(Sequence_type& p)  { return deduce_from_type(p, a, sub); }
    bool operator()(Typename_type& p)  { return deduce_from_type(p, a, sub); }
  };
  return apply(p, fn{a, sub});
}


// Deduce a template arguments from a list of parameters and arguments.
// This succeeds only when deduction succeds for each parameter and
// argument in the corresponding lists.
//
// Here, ps is considered a function parameter list and as a
// function argument list.
//
// TODO: Correctly handle deductions against packs, etc.
bool
deduce_from_types(Type_list& ps, Type_list& as, Substitution& sub)
{
  std::size_t i = 0;
  while (i < ps.size() && i < as.size()) {
    Type& p = *ps[i];
    Type& a = *as[i];
    if (!deduce_from_type(p, a, sub))
      return false;
    ++i;
  }

  // FIXME: We should probably never get here. Or maybe we should?
  if (i != ps.size() || i != as.size())
    lingo_unreachable();

  return true;
}


// -------------------------------------------------------------------------- //
// Deducing template arguments from a function call


// If t is in the initial set, then it must be deduced locally.
void
select_template_parameter(Typename_type& t, Substitution& init, Substitution& ret)
{
  Decl& d = t.declaration();
  if (init.has_mapping(d))
    ret.seed_with(d);
}


// Returns a substitution containing the subset of the original
// substitution s that appear in t. Note that this may be the empty set.
void
select_template_parameters(Type& t, Substitution& init, Substitution& ret)
{
  struct fn {
    Substitution& init;
    Substitution& ret;
    void operator()(Type& t)           { }
    void operator()(Function_type& t)  { lingo_unreachable(); }
    void operator()(Reference_type& t) { select_template_parameters(t.type(), init, ret); }
    void operator()(Qualified_type& t) { select_template_parameters(t.type(), init, ret); }
    void operator()(Pointer_type& t)   { select_template_parameters(t.type(), init, ret); }
    void operator()(Array_type& t)     { select_template_parameters(t.type(), init, ret); }
    void operator()(Tuple& t)          { select_template_parameters(t.type(), init, ret); }
    void operator()(Dynarray_type& t)  { select_template_parameters(t.type(), init, ret); }
    void operator()(Sequence_type& t)  { select_template_parameters(t.type(), init, ret); }
    void operator()(Typename_type& t)  { select_template_parameter(t, init, ret); }
  };
  apply(t, fn{init, ret});
}


// Returns a substitution containing the subset of the original
// substitution s that appear in the declared type of d. Note
// that this may be the empty set.
Substitution
select_template_parameters(Decl& d, Substitution& init)
{
  Substitution ret;
  select_template_parameters(declared_type(d), init, ret);
  return ret;
}


void
deduce_from_call(Context& cxt, Decl_list& parms, Expr_list& args, Substitution& sub)
{
  auto pi = parms.begin();
  auto ai = args.begin();
  while (pi != parms.end() && ai != args.end()) {
    Decl& p = *pi;
    Expr& a = *ai;

    // Get the set of template parameters used by the function
    // parameter. Function parameters not using template parameters
    // do not participate in deduction.
    Substitution local = select_template_parameters(p, sub);
    if (!local.empty()) {
      // FIXME: Make adjustments on the parameter and argument type
      // as needed to make stuff work. Be sure to handle deduction with
      // parameter packs in the future.

      // Actually attempt deduction.
      //
      // FIXME: Improve the diagnostic.
      if (!deduce_from_type(declared_type(p), a.type(), local))
        throw Deduction_error(cxt, "deduction failed for '{}'", p.name());

      // Unify the deduction with the global state.
      unify(cxt, sub, local);
    }

    ++pi;
    ++ai;
  }

  // If everything matched up, then we need to check that
  // all template arguments have been deduced.
  if (pi == parms.end()) {
    if (ai == args.end()) {
      if (sub.is_incomplete())
        throw Deduction_error(cxt, "failed to deduce all arguments");
      return;
    }
    throw Deduction_error(cxt, "too many arguments");
  }
  throw Deduction_error(cxt, "too few arguments");
}


} // namespace banjo
