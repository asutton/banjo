// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_DECLARATION_HPP
#define BANJO_DECLARATION_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

struct Context;
struct Decl;
struct Scope;


void declare(Context&, Decl&);
void declare(Context&, Scope&, Decl&);

void declare_required_expression(Context&, Expr&);

} // namespace banjo


#endif
