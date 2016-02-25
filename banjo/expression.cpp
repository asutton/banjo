// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_expr.hpp"
#include "context.hpp"


namespace banjo
{

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

} // namespace banjo
