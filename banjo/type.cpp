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
resolve_type(Context& cxt, Name& n)
{
  if (Simple_id* id = as<Simple_id>(&n))
    return resolve_simple_id_type(cxt, *id);
  lingo_unhandled(n);
}


// -------------------------------------------------------------------------- //
// Qualified types
//
// TODO: Be more clever about qualifications.

namespace
{

// Any qualifiers on an array type apply to the element type.
Type&
make_qualified_array_type(Context& cxt, Array_type& t, Qualifier_set q)
{
  Type& qual = make_qualified_type(cxt, t.element_type(), q);
  Expr& ext = t.extent();
  return cxt.get_array_type(qual, ext);
}


// Any qualifiers on a tuple type apply to the element types.
Type&
make_qualified_tuple_type(Context& cxt, Tuple_type& t, Qualifier_set q)
{
  Type_list ts;
  for (Type& elem : t.element_types()) {
    Type& qual = make_qualified_type(cxt, elem, q);
    ts.push_back(qual);
  }
  return cxt.get_tuple_type(std::move(ts));
}


// Set the object-qualifiers of the object t to q. If q is empty, then
// this will return an unqualified object type.
//
// Note that qualifiers for arrays and tuples are transitive. See the
// functions above for their rules.
//
// The qualifiers shall not include function or reference qualifiers.
Type&
make_qualified_object_type(Context& cxt, Type& t, Qualifier_set q)
{
  lingo_assert(get_obejct_qualifiers() == q);
  if (Array_type* arr = as<Array_type>(&t))
    return make_qualified_array_type(cxt, *arr, q);
  if (Tuple_type* tup = as<Tuple_type>(&t))
    return make_qualified_tuple_type(cxt, *tup, q);
  return cxt.get_qualified_type(t, q);
}


Type&
make_qualified_object_type(Context& cxt, Type& t, Qualifier_set q)
{

}


Type&
make_qualified_object_type(Context& cxt, Type& t, Qualifier_set q)
{

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
