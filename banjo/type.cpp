// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "context.hpp"
#include "lookup.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Type name resolution

namespace
{

// Resolve a simple id as a type. This performs simple lookup (type names
// are not overloaded) and determines if the found declaration is actually
// a class.
Type&
resolve_simple_id_type(Context& cxt, Simple_id& id)
{
  Decl& d = simple_lookup(cxt, id);
  if (!is<Class_decl>(d)) {
    error(cxt, "'{}' does not name a type", d);
    throw Type_error();
  }
  Class_decl& decl = cast<Class_decl>(d);
  return cxt.get_class_type(object_type, decl);
}


} // namespace


// Resolve a name that refers to a type.
Type&
resolve_id_type(Context& cxt, Name& n)
{
  if (Simple_id* id = as<Simple_id>(&n))
    return resolve_simple_id_type(cxt, *id);
  lingo_unhandled(n);
}


// -------------------------------------------------------------------------- //
// Reference types

// Returns a reference variant of `t`. Throws an error if `t` cannot be made
// into a reference type.
//
// TODO: Check that t is a (valid) object or function type. Note the special
// exception about references to void.
Type&
make_reference_type(Context& cxt, Type& t)
{
  return cxt.get_reference_type(t);
}


// -------------------------------------------------------------------------- //
// Qualified types
//
// TODO: Be more clever about qualifications.

namespace
{

// Set the object-qualifiers of the object t to q. If q is empty, then
// this will return an unqualified object type.
//
// Note that qualifiers for arrays and tuples are transitive. See the
// functions above for their rules.
//
// The qualifiers shall not include function or reference qualifiers.
//
// FIXME: Actually check this stuff.
Type&
make_qualified_object_type(Context& cxt, Type& t, Qualifier_set q)
{
  return cxt.get_qualified_type(t, q);
}


Type&
make_qualified_reference_type(Context& cxt, Type& t, Qualifier_set q)
{
  lingo_unreachable();
}


Type&
make_qualified_function_type(Context& cxt, Type& t, Qualifier_set q)
{
  lingo_unreachable();
}


} // namespace


// Return a qualified variant of t, if possible.
Type&
make_qualified_type(Context& cxt, Type& t, Qualifier_set q)
{
  switch (t.category()) {
    case object_type:
      return make_qualified_object_type(cxt, t, q);
    case reference_type:
      return make_qualified_reference_type(cxt, t, q);
    case function_type:
      return make_qualified_function_type(cxt, t, q);
    default:
      lingo_unreachable();
  }
}


// Returns a pointer-to-T type.
//
// FIXME: Verify that we can actually form a pointer-to-T.
Type&
make_pointer_type(Context& cxt, Type& t)
{
  return cxt.get_pointer_type(object_type, t);
}


// Returns an array-of-T type.
//
// FIXME: Verify that we can can actually form an array-of-T.  For example, 
// is this valid? T[][N]. Perhaps.
//
Type&
make_array_type(Context& cxt, Type& t, Expr& e)
{
  return cxt.get_array_type(object_type, t, e);
}


// Returns a tuple of the given element types.
Type&
make_tuple_type(Context& cxt, Type_list& t)
{
  return cxt.get_tuple_type(object_type, t);
}


// Returns a fresh type that represents the result of an expression.
Type&
make_fresh_type(Context& cxt)
{
  lingo_unreachable();
}

} // namespace banjo
