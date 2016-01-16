// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_CALL_HPP
#define BEAKER_CALL_HPP

#include "prelude.hpp"
#include "context.hpp"

namespace beaker
{

struct Type;
struct Expr;
struct Init;
struct Decl;
struct Function_decl;


Expr& build_call(Context&, Function_decl&, Expr_list const&);

} // namespace beaker


#endif
