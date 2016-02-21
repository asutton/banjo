// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONSTRAINT_HPP
#define BANJO_CONSTRAINT_HPP

#include "prelude.hpp"
#include "language.hpp"
#include "scope.hpp"


namespace banjo
{

Cons&       expand(Context&, Concept_cons&);
Cons const& expand(Context&, Concept_cons const&);

Expr* admit_expression(Context&, Expr&, Expr&);
Expr* admit_expression(Context&, Cons&, Expr&);

} // namespace banjo


#endif
