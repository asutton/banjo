// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "substitution.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "expression.hpp"
#include "declaration.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Substitution class


// Helper debug output.
std::ostream&
operator<<(std::ostream& os, Substitution const& s)
{
  os << "{\n";
  for (auto& x : s) {
    os << "  " << *x.first << " => ";
    if (x.second)
      os << *x.second;
    else
      os << "<nullptr>";
    os << '\n';
  }
  os << "}\n";
  return os;
}


// -------------------------------------------------------------------------- //
// Simple unification

// Update a global (larger) substitution with the results of a
// local deduction. If a parameter is mapped to different values,
// unification fails with an exception.
void
unify(Context& cxt, Substitution& global, Substitution& local)
{
  for (auto& x : local) {
    Decl& parm = *x.first;
    Term& value = *x.second;
    if (Term* prev = global.get_mapping(parm)) {
      if (!is_equivalent(*prev, value))
        throw Unification_error(cxt, "'{}' deduced with different values");
    }
    global.map_to(parm, value);
  }
}


// -------------------------------------------------------------------------- //
// Substitution helpers

template<typename T>
List<T>
substitute(Context& cxt, List<T>& list, Substitution& sub)
{
  List<T> ret;
  ret.reserve(list.size());
  for (T& x : list)
    ret.push_back(substitute(cxt, x, sub));
  return ret;
}


// -------------------------------------------------------------------------- //
// Substitution into terms

Term&
substitute(Context& cxt, Term& x, Substitution& sub)
{
  if (Type* t = as<Type>(&x))
    return substitute(cxt, *t, sub);
  if (Expr* e = as<Expr>(&x))
    return substitute(cxt, *e, sub);
  if (Decl* d = as<Decl>(&x))
    return substitute(cxt, *d, sub);
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Substitution into types

Type& substitute_type(Context&, Function_type&, Substitution&);
Type& substitute_type(Context&, Reference_type&, Substitution&);
Type& substitute_type(Context&, Qualified_type&, Substitution&);
Type& substitute_type(Context&, Pointer_type&, Substitution&);
Type& substitute_type(Context&, Array_type&, Substitution&);
Type& substitute_type(Context&, Tuple_type&, Substitution&);
Type& substitute_type(Context&, Dynarray_type&, Substitution&);
Type& substitute_type(Context&, Sequence_type&, Substitution&);
Type& substitute_type(Context&, Typename_type&, Substitution&);


Type&
substitute(Context& cxt, Type& t, Substitution& sub)
{
  struct fn
  {
    Context&      cxt;
    Substitution& sub;

    // Most types cannot be substituted into.
    Type& operator()(Type& t)           { return t; }

    Type& operator()(Auto_type& t)      { lingo_unreachable(); }
    Type& operator()(Decltype_type& t)  { lingo_unreachable(); }
    Type& operator()(Declauto_type& t)  { lingo_unreachable(); }

    // Recrusively substitute through compound types.
    Type& operator()(Function_type& t)  { return substitute_type(cxt, t, sub); }
    Type& operator()(Reference_type& t) { return substitute_type(cxt, t, sub); }
    Type& operator()(Qualified_type& t) { return substitute_type(cxt, t, sub); }
    Type& operator()(Pointer_type& t)   { return substitute_type(cxt, t, sub); }
    Type& operator()(Array_type& t)     { return substitute_type(cxt, t, sub); }
    Type& operator()(Tuple_type& t)     { return substitute_type(cxt, t, sub); }
    Type& operator()(Dynarray_type& t)  { return substitute_type(cxt, t, sub); }
    Type& operator()(Sequence_type& t)  { return substitute_type(cxt, t, sub); }
    Type& operator()(Typename_type& t)  { return substitute_type(cxt, t, sub); }
  };
  return apply(t, fn{cxt, sub});
}


Type&
substitute_type(Context& cxt, Function_type& t, Substitution& sub)
{
  Type_list ps = substitute(cxt, t.parameter_types(), sub);
  Type& r = substitute(cxt, t.return_type(), sub);
  return cxt.get_function_type(ps, r);
}


Type&
substitute_type(Context& cxt, Reference_type& t, Substitution& sub)
{
  Type& s = substitute(cxt, t.type(), sub);
  return cxt.get_reference_type(s);
}


Type&
substitute_type(Context& cxt, Qualified_type& t, Substitution& sub)
{
  Type& s = substitute(cxt, t.type(), sub);
  return cxt.get_qualified_type(s, t.qualifier());
}


Type&
substitute_type(Context& cxt, Pointer_type& t, Substitution& sub)
{
  Type& s = substitute(cxt, t.type(), sub);
  return cxt.get_pointer_type(s);
}


Type&
substitute_type(Context& cxt, Array_type& t, Substitution& sub)
{
  lingo_unreachable();
}


Type&
substitute_type(Context& cxt, Tuple_type& t, Substitution& sub)
{
  lingo_unreachable();
}


Type&
substitute_type(Context& cxt, Dynarray_type& t, Substitution& sub)
{
  lingo_unreachable();
}


Type&
substitute_type(Context& cxt, Sequence_type& t, Substitution& sub)
{
  Type& s = substitute(cxt, t.type(), sub);
  return cxt.get_sequence_type(s);
}


// Substitute into a typename type. If the type's declaration is
// in the mapping, then reteurn the mapped type. Otherwise,
// return the original type.
Type&
substitute_type(Context& cxt, Typename_type& t, Substitution& sub)
{
  Decl& d = t.declaration();
  if (sub.has_mapping(d))
    return cast<Type>(*sub.get_mapping(d));
  else
    return t;
}


// -------------------------------------------------------------------------- //
// Substitution into expressions
//


// Substitute into a declaration reference. The result of the
// substitution depends on whether the name refers to a member
// of the current instantiation. If so, then we need to rebind
// the identifier to the locally instantiated declaration.
//
// FIXME: This is almost certainly incorrect.
Expr&
subst_ref(Context& cxt, Decl_expr& e, Substitution& sub)
{
  return make_reference(cxt, e.declaration().name());
}


Expr&
subst_check(Context& cxt, Check_expr& e, Substitution& sub)
{
  // FIXME: This needs to check the substituted arguments
  // against the declared concept, possibly performing some
  // kind of resolution. In other words, use make_check, whenever
  // it gets written.
  Builder build(cxt);
  Term_list args = substitute(cxt, e.arguments(), sub);
  return build.make_check(e.declaration(), args);
}


Expr&
subst_call(Context& cxt, Call_expr& e, Substitution& sub)
{
  Expr& fn = substitute(cxt, e.function(), sub);
  Expr_list args = substitute(cxt, e.arguments(), sub);
  return make_call(cxt, fn, args);
}


template<typename T, typename Make>
Expr&
subst_unary(Context& cxt, T& e, Substitution& sub, Make make)
{
  Expr& e0 = substitute(cxt, e.operand(), sub);
  return make(cxt, e0);
}


template<typename T, typename Make>
Expr&
subst_binary(Context& cxt, T& e, Substitution& sub, Make make)
{
  Expr& e1 = substitute(cxt, e.left(), sub);
  Expr& e2 = substitute(cxt, e.right(), sub);
  return make(cxt, e1, e2);
}


// FIXME: Generalize to all conversions.
Expr&
subst_conv(Context& cxt, Boolean_conv& e, Substitution& sub)
{
  Expr& e1 = substitute(cxt, e.source(), sub);
  Type& t1 = substitute(cxt, e.destination(), sub);
  return *new Boolean_conv(t1, e1);
}


Expr&
substitute(Context& cxt, Expr& e, Substitution& sub)
{
  struct fn
  {
    Context&      cxt;
    Substitution& sub;

    Expr& operator()(Expr& e) { banjo_unhandled_case(e); }

    Expr& operator()(Boolean_expr& e) { return e; }
    Expr& operator()(Integer_expr& e) { return e; }
    Expr& operator()(Decl_expr& e)    { return subst_ref(cxt, e, sub); }
    Expr& operator()(Check_expr& e)   { return subst_check(cxt, e, sub); }
    Expr& operator()(Call_expr& e)    { return subst_call(cxt, e, sub); }

    Expr& operator()(Eq_expr& e)  { return subst_binary(cxt, e, sub, make_eq); }
    Expr& operator()(Ne_expr& e)  { return subst_binary(cxt, e, sub, make_ne); }
    Expr& operator()(Lt_expr& e)  { return subst_binary(cxt, e, sub, make_lt); }
    Expr& operator()(Gt_expr& e)  { return subst_binary(cxt, e, sub, make_gt); }
    Expr& operator()(Le_expr& e)  { return subst_binary(cxt, e, sub, make_le); }
    Expr& operator()(Ge_expr& e)  { return subst_binary(cxt, e, sub, make_ge); }
    Expr& operator()(And_expr& e) { return subst_binary(cxt, e, sub, make_logical_and); }
    Expr& operator()(Or_expr& e)  { return subst_binary(cxt, e, sub, make_logical_or); }
    Expr& operator()(Not_expr& e) { return subst_unary(cxt, e, sub, make_logical_not); }

    Expr& operator()(Boolean_conv& e) { return subst_conv(cxt, e, sub); }

  };
  return apply(e, fn{cxt, sub});
}


// -------------------------------------------------------------------------- //
// Substitution into declarations
//
// Note that substitution into a declaration does not change
// the name of the declaration. Specialization, however, is a
// special form of substitution where we generate a newly named
// declaration.
//
// FIXME: Substitution is kind of like parsing. We need to interpret
// the resulting constructs as if they were parsed. That means we
// need to maintain binding environments to support lookup and
// resolution.


Decl&
substitute_decl(Context& cxt, Variable_decl& d, Substitution& sub)
{
  Name& n = d.name();
  Type& t = substitute(cxt, d.type(), sub);
  Decl& var = cxt.make_variable_declaration(n, t);
  declare(cxt, var);
  return var;
}


// FIXME: Do we substitute into a default argument? It probably
// depends on context, but I assume that the general answer is yes.
Decl&
substitute_decl(Context& cxt, Object_parm& d, Substitution& sub)
{
  Name& n = d.name();
  Type& t = substitute(cxt, d.type(), sub);
  Decl& parm = cxt.make_object_parm(n, t);
  declare(cxt, parm);
  return parm;
}


Decl&
substitute(Context& cxt, Decl& d, Substitution& sub)
{
  struct fn
  {
    Context&      cxt;
    Substitution& sub;
    Decl& operator()(Decl& d)           { lingo_unreachable(); }
    Decl& operator()(Variable_decl& d)  { return substitute_decl(cxt, d, sub); }
    Decl& operator()(Object_parm& d)    { return substitute_decl(cxt, d, sub); }
  };
  return apply(d, fn{cxt, sub});
}


// -------------------------------------------------------------------------- //
// Substitution into constraints
//
// TODO: Substitution through constraints should almost never
// result in outright failure.

Cons&
subst_predicate(Context& cxt, Predicate_cons& c, Substitution& sub)
{
  Expr& e = substitute(cxt, c.expression(), sub);
  return cxt.get_predicate_constraint(e);
}


Cons&
subst_usage(Context& cxt, Expression_cons& c, Substitution& sub)
{
  Expr& e = substitute(cxt, c.expression(), sub);
  Type& t = substitute(cxt, c.type(), sub);

  // Ensure that e has type t: that's what's been assumed.
  e.type_ = &t;

  return cxt.get_expression_constraint(e, t);
}


Cons&
subst_usage(Context& cxt, Conversion_cons& c, Substitution& sub)
{
  Expr& e = substitute(cxt, c.expression(), sub);
  Type& t = substitute(cxt, c.type(), sub);

  // FIXME: We should be doing a lookup to determine the correct
  // type of the expression.

  return cxt.get_conversion_constraint(e, t);
}


Cons&
subst_parametric(Context& cxt, Parameterized_cons& c, Substitution& sub)
{
  // Enter_requires_scope rscope(cxt);
  Decl_list parms = substitute(cxt, c.variables(), sub);
  Cons& cons = substitute(cxt, c.constraint(), sub);
  return cxt.get_parameterized_constraint(parms, cons);
}


Cons&
subst_conjunction(Context& cxt, Conjunction_cons& c, Substitution& sub)
{
  Cons& c1 = substitute(cxt, c.left(), sub);
  Cons& c2 = substitute(cxt, c.right(), sub);
  return cxt.get_conjunction_constraint(c1, c2);
}


Cons&
subst_disjunction(Context& cxt, Disjunction_cons& c, Substitution& sub)
{
  Cons& c1 = substitute(cxt, c.left(), sub);
  Cons& c2 = substitute(cxt, c.right(), sub);
  return cxt.get_disjunction_constraint(c1, c2);
}


Cons&
substitute(Context& cxt, Cons& c, Substitution& sub)
{
  struct fn
  {
    Context&      cxt;
    Substitution& sub;
    Cons& operator()(Cons& c)               { banjo_unhandled_case(c); }
    Cons& operator()(Predicate_cons& c)     { return subst_predicate(cxt, c, sub); }
    Cons& operator()(Expression_cons& c)    { return subst_usage(cxt, c, sub); }
    Cons& operator()(Conversion_cons& c)    { return subst_usage(cxt, c, sub); }
    Cons& operator()(Parameterized_cons& c) { return subst_parametric(cxt, c, sub); }
    Cons& operator()(Conjunction_cons& c)   { return subst_conjunction(cxt, c, sub); }
    Cons& operator()(Disjunction_cons& c)   { return subst_disjunction(cxt, c, sub); }
  };
  return apply(c, fn{cxt, sub});
}


} // namespace banjo
