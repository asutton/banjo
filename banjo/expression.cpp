// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast-expr.hpp"
#include "ast-type.hpp"
#include "type.hpp"
#include "context.hpp"
#include "lookup.hpp"


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


//Modeled after make_regular_call
Expr&
make_tuple_expr(Context& cxt, Expr_list& l)
{
  Type_list tlist;
  for(auto eli = l.begin(); eli != l.end(); eli++) {
    tlist.push_back(eli->type());
  }
  
  Type &t = make_tuple_type(cxt,tlist);
  
  Tuple_expr& te = cxt.make_tuple_expr(t, l);
  return te;
}


} // namespace banjo
