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

// FIXME: This could also match a type parameter.
Type&
make_type(Context& cxt, Simple_id& id)
{
  Decl& d = simple_lookup(cxt, id);
  if (is<Coroutine_decl>(d)) {
    Coroutine_decl& decl = cast<Coroutine_decl>(d);
    return cxt.get_coroutine_type(decl);
  }
  if (!is<Class_decl>(d)) {
    error(cxt, "'{}' does not name a type", d);
    throw Type_error("not a type");
  }
  Class_decl& decl = cast<Class_decl>(d);
  return cxt.get_class_type(decl);
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


// Return t as a cv-qualified type. This will merge subsequent qualifiers 
// so that attempts to construct 'const const T' will  simply result in 
// 'const T'.
Type&
make_qualified_type(Context& cxt, Type& t, Qualifier_set q)
{
  return cxt.get_qualified_type(t, q);
}


// Returns a pointer-to-T type.
//
// FIXME: Verify that we can actually form a pointer-to-T.
Type&
make_pointer_type(Context& cxt, Type& t)
{
  return cxt.get_pointer_type(t);
}


// Returns an array-of-T type.
//
// FIXME: Verify that we can can actually form an array-of-T.  For example, 
// is this valid? T[][N]. Perhaps.
//
Type&
make_array_type(Context& cxt, Type& t, Expr& e)
{
  return cxt.get_array_type(t, e);
}


// Returns a tuple of the given element types.
Type&
make_tuple_type(Context& cxt, Type_list& t)
{
  return cxt.get_tuple_type(t);
}


// Returns a fresh type that represents the result of an expression.
Type&
make_fresh_type(Context& cxt)
{
  return cxt.make_auto_type();
}

} // namespace banjo
