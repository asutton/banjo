// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

namespace
{

Type&
make_type(Context& cxt, Simple_id& id)
{
  Decl& d = simple_lookup(cxt, cxt.current_scope(), id);
  if (!is<Type_decl>(d)) {
    error(cxt, "'{}' does not name a type", d);
    throw Type_error("not a type");
  }
  Type_decl& decl = cast<Type_decl>(d);
  return cxt.get_type(decl);
}


} // namespace



// Resolve a name that refers to a type.
Type&
make_type(Context& cxt, Name& n)
{
  if (Simple_id* id = as<Simple_id>(&n))
    return make_type(cxt, *id);
  lingo_unhandled(n);
}


// Returns a fresh type that represents the result of an expression.
Type&
make_fresh_type(Context& cxt)
{
  return cxt.make_placeholder_type();
}


} // namespace banjo
