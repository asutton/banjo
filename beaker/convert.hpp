// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_CONVERT_HPP
#define BEAKER_CONVERT_HPP

#include "prelude.hpp"


namespace beaker
{

struct Type;
struct Expr;

Expr* convert(Expr const&, Type const&);


} // namespace beaker


#endif
