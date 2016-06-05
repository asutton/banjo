// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{


} // namespace banjo


// Just include these instead of compiling them as separate translation 
// units. Otherwise, we have to re-translate the AST for each module.
#include "build-name.cpp"
#include "build-type.cpp"
#include "build-expr.cpp"
#include "build-stmt.cpp"
#include "build-decl.cpp"
