// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_name.hpp"
#include "ast_type.hpp"
#include "ast_decl.hpp"
#include "builder.hpp"

#include <iostream>


namespace banjo
{

// Returns a fresh type that represents the result of an expression.
Type&
make_fresh_type(Context& cxt)
{
  Builder build(cxt);

  // FIXME: Give me this better name?
  Name& n = build.get_id();

  // Generate the new declaration (but don't declare it anywhere)
  // and return its associated type.
  Decl& d = build.make_type_parameter(n);
  return build.get_typename_type(d);
}


} // namespace banjo
