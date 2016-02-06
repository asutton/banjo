// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONSTRAINT_HPP
#define BANJO_CONSTRAINT_HPP

#include "prelude.hpp"

namespace banjo
{

struct Cons;
struct Concept_cons;
struct Context;


Cons&       expand(Context&, Concept_cons&);
Cons const& expand(Context&, Concept_cons const&);

} // namespace banjo


#endif
