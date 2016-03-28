// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_type.hpp"
#include "context.hpp"

#include <iostream>


namespace banjo
{

// Returns a fresh type that represents the result of an expression.
Type&
make_fresh_type(Context& cxt)
{
  return cxt.make_placeholder_type();
}


} // namespace banjo
