// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_DECLARATION_HPP
#define BANJO_DECLARATION_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

struct Context;

Expr& make_logical_and(Context&, Expr&, Expr&);
Expr& make_logical_or(Context&, Expr&, Expr&);
Expr& make_logical_not(Context&, Expr&);

Expr& make_reference(Context& cxt, Name&);

Expr& make_requirements(Context& cxt, Decl_list const&, Stmt&);


} // namespace banjo


#endif
