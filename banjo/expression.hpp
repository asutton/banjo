// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_DECLARATION_HPP
#define BANJO_DECLARATION_HPP

#include "prelude.hpp"


namespace banjo
{

struct Context;
struct Type;
struct Expr;
struct Decl;


Expr& make_logical_and(Context&, Expr&, Expr&);
Expr& make_logical_or(Context&, Expr&, Expr&);
Expr& make_logical_not(Context&, Expr&);


} // namespace banjo


#endif
