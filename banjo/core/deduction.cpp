// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "deduction.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "type.hpp"
#include "template.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Unification (sort of)

// When deduction has succeeded, insert a mapping from p to t. A unification
// error occurs when p has been deduced to different types.
void
unify(Context& cxt, Declared_type& p, Type& t, Substitution& sub)
{
  Decl& d = p.declaration();

  // Check for a previous mapping from p to t. 
  if (sub.has_mapping(d)) {
    if (Term* prev = sub.get_mapping(d)) {
      if (!is_equivalent(t, *prev)) {
        error(cxt, "'{}' previously deduced as'{}'", p, t);
        throw Unification_error();
      }
    }
  } 

  // Otherwise, insert a the mapping.
  sub.map_to(d, t);
}


// -------------------------------------------------------------------------- //
// Deducing template arguments from a type

// Deduce &p vs &t.
static bool
deduce_reference_type(Context& cxt, Reference_type& p, Type& t, Substitution& sub)
{
  if (Reference_type* ref = as<Reference_type>(&t))
    return deduce_type(cxt, p.type(), ref->type(), sub);
  return false;
}


// Deduce cv-p vs cv-t.
bool
deduce_qualified_type(Context& cxt, Qualified_type& p, Type& t, Substitution& sub)
{
  if (Qualified_type* qual = as<Qualified_type>(&t)) {
    if (p.qualifier() == qual->qualifier())
      return deduce_type(cxt, p.type(), qual->type(), sub);
  }
  return false;
}


// Deduced *p vs *t.
bool
deduce_pointer_type(Context& cxt, Pointer_type& p, Type& t, Substitution& sub)
{
  if (Pointer_type* ptr = as<Pointer_type>(&t))
    return deduce_type(cxt, p.type(), ptr->type(), sub);
  return false;
}


// Deduce an assignment of a type parameter to a corresponding
// argument type. For example, given
//
//    p: auto
//    t: int
//
// This will deduce the substitution of auto -> int. If there is already
// a mapping from p to t, deduction fails, returning false.
//
// Note that patterns will only match simple (non-compound) types. In most
// cases deduction is used in a context where argument types are adjusted
// to discard or remove references so imprecise matches can be made.
//
// TODO: Lots of types are missing here. 
bool
deduce_placeholder_type(Context& cxt, Declared_type& p, Type& t, Substitution& sub)
{
  struct fn
  {
    bool operator()(Type& t)         { return false; }
    bool operator()(Byte_type& t)    { return true; }
    bool operator()(Boolean_type& t) { return true; }
    bool operator()(Integer_type& t) { return true; }
    bool operator()(Float_type& t)   { return true; }
    bool operator()(Class_type& t)   { return true; }
  };
  fn is_deducible;
  
  // if we can deduce the type, then unify this mapping with the
  // current substitution.
  if (is_deducible(t)) {
    unify(cxt, p, t, sub);
    return true;
  }

  // TODO: Improve diagnostics. In particular, we should diagnose the
  // "cannot deduce" error at the entry point to the algorithm and then
  // use this error for the particular reason deduction failed.
  error(cxt, "cannot deduce '{}' as '{}'", t, p);
  throw Deduction_error();  
}


// Find a substitution from placeholders in a type pattern `p` to arguments
// in a type `t`.
//
// TODO: Lots of stuff missing here...
bool
deduce_type(Context& cxt, Type& p, Type& t, Substitution& sub)
{
  struct fn
  {
    Context&      cxt;
    Type&         t;
    Substitution& sub;

    // For non-compound, non-placeholder types, p and t must be equivalent.
    bool operator()(Type& p)           { return is_equivalent(p, t); }

    // For placeholder types and type parameters, t must be a simple type.
    bool operator()(Auto_type& p)      { return deduce_placeholder_type(cxt, p, t, sub); }

    // For compound, types, match structure.
    bool operator()(Reference_type& p) { return deduce_reference_type(cxt, p, t, sub); }
    bool operator()(Qualified_type& p) { return deduce_qualified_type(cxt, p, t, sub); }
    bool operator()(Pointer_type& p)   { return deduce_pointer_type(cxt, p, t, sub); }
  };
  return apply(p, fn{cxt, t, sub});
}


// Deduce the type of an expression against a given pattern.
bool
deduce_type(Context& cxt, Type& pattern, Expr& expr, Substitution& sub)
{
  return deduce_type(cxt, pattern, expr.type(), sub);
}


// Return the substitution mapping parameters in t2 to arguments in t2.
Substitution
deduce_type(Context& cxt, Type& pattern, Type& type)
{
  Substitution sub;
  deduce_type(cxt, pattern, type, sub);
  return sub;
}


// Deduce the type of an expression against a type pattern. 
Substitution
deduce_type(Context& cxt, Type& pattern, Expr& expr)
{
  return deduce_type(cxt, pattern, expr.type());
}


// -------------------------------------------------------------------------- //
// Deduction from an initializer

// This form of deduction is used to deduce the declared type of a
// variable and the type of function arguments in a call expression.
//
// If the type pattern is `auto` or a type parameter and `e` is a
// tuple, then the deduced type is the type of `e`.
//
// FIXME: We probably want to strip references and qualifiers from e
// prior to deduction so that we can allow reference binding and other
// forms of initialization later.
//
// FIXME: Check for re-substitution.
bool
deduce_from_initializer(Context& cxt, Type& p, Expr& e, Substitution& s)
{
  // If the pattern is auto or a type parameter and e is a tuple, then
  // map p to the type of e.
  if (is<Tuple_expr>(e)) {
    if (Auto_type* t = as<Auto_type>(&p))
      s.map_to(t->declaration(), e.type());
    return true;
  }
  return false;
}


// Deduce the type of a declaration from its initializer.
Substitution
deduce_from_initializer(Context& cxt, Type& pattern, Expr& expr)
{
  Substitution sub;
  deduce_from_initializer(cxt, pattern, expr, sub);
  return sub;
}


// Deduce the type of the variable or constant from its initializer and 
// update the declaration. If the declared type has no placeholders, no
// action is taken.
void
deduce_declared_type(Context& cxt, Decl& decl, Def& def)
{
  Type& type = decl.type();
  
  // If the declared type contains placeholders, deduce its actual
  // type from the initializer.
  Decl_list ds = get_placeholders(type);
  if (!ds.empty()) {
    Substitution sub(ds);

    // Deduction rules depend on the initialization form.
    if (Expression_def* init = as<Expression_def>(&def)) {
      Expr& expr = init->expression();
      deduce_from_initializer(cxt, type, expr, sub);

      // FIXME: This isn't correct. We actually need to substitute
      // the deduced parameters back into the initial type in order
      // to generate the correct type.
      decl.type_ = &expr.type();
    } else {
      // TODO: Deduce from other initializer forms.
      lingo_unhandled(def);
    }
  }
}


#if 0

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
#endif

// -------------------------------------------------------------------------- //
// Placeholders

// TODO: Finish implementing me.
static void 
get_placeholders(Type& t, Decl_list& ds)
{
  struct fn
  {
    Decl_list& ds;
    void operator()(Type& t)           { /* Do nothing. */ }
    void operator()(Reference_type& t) { get_placeholders(t.type(), ds); }
    void operator()(Pointer_type& t)   { get_placeholders(t.type(), ds); }
    void operator()(Array_type& t)     { get_placeholders(t.type(), ds); }
    void operator()(Auto_type& t)      { ds.push_back(t.declaration()); }    

    void operator()(Function_type& t) 
    { 
      for (Type& elem : t.parameter_types())
        get_placeholders(elem, ds); 
      get_placeholders(t.return_type(), ds);
    }

    void operator()(Tuple_type& t) 
    { 
      for (Type& elem : t.element_types())
        get_placeholders(elem, ds); 
    }
  };
  apply(t, fn{ds});
}


// Get the list of the declared placeholders in the type t. 
Decl_list 
get_placeholders(Type& t)
{
  Decl_list ds;
  get_placeholders(t, ds);
  return ds;
}



} // namespace banjo
