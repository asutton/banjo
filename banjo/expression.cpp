// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast-expr.hpp"
#include "ast-type.hpp"
#include "type.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "conversion.hpp"


namespace banjo
{

// A helper function for the typing of expressions occurring in a
// requires-expressions. This performs a lookup of an iniitally
// constructed expression, and adjusts its type as needed.
Expr&
make_required_expression(Context& cxt, Expr& e)
{
  if (Expr* prev = requirement_lookup(cxt, e))
    e.type_ = prev->type_;
  return e;
}


// A requires expression has type bool.
//
// TODO: Actually validate information about the requires expression. The
// statement cannot be emtpy. No variadic parameter, etc.
//
// TODO: Eventually move this to a different expr_ module.
Expr&
make_requirements(Context& cxt,
                  Decl_list const& tparms,
                  Decl_list const& parms,
                  Req_list const& reqs)
{
  return cxt.make_requires(tparms, parms, reqs);
}


// Apply an object-to-value conversion.
//
// FIXME: Move this into the conversion module.
static inline Expr&
convert_to_value(Context& cxt, Expr& e)
{
  Type& t = e.type().non_reference_type();
  return standard_conversion(e, t);
}


// Returns a new tuple expression. The operands are converted to values.
// The type of a tuple expression is the tuple type comprising the types
// of the converted operands.
//
// The value of a tuple expression is an aggregate containing the values
// of its operands.
//
// TODO: Are there any other conversions we want to perform prior to
// converting these to values?
Expr&
make_tuple_expr(Context& cxt, Expr_list& es)
{
  // Convert expressions to values.
  Expr_list cs;
  cs.reserve(es.size());
  for (auto i = es.begin(); i != es.end(); i++)
    cs.push_back(convert_to_value(cxt, *i));

  // Generate the tuple type.
  Type_list ts;
  ts.reserve(cs.size());
  for (auto i = cs.begin(); i != cs.end(); i++)
    ts.push_back(i->type());
  Type& t = make_tuple_type(cxt, ts);
  
  return cxt.make_tuple_expr(t, cs);
}


} // namespace banjo
