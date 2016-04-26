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
  Decl& d = simple_lookup(cxt, id);
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


// Return t as a cv-qualified type. This will merge subsequent qualifiers 
// so that attempts to construct 'const const T' will  simply result in 
// 'const T'.
Type&
make_qualified_type(Context& cxt, Type& t, Qualifier_set q)
{
  if (Qualified_type* qt = as<Qualified_type>(&t)) {
    qt->qual |= q;
    return *qt;
  } else {
    return cxt.get_qualified_type(t, q);
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
// FIXME: Determine if this is an array or dynarray type by evaluating
// the expression.
Type&
make_array_type(Context& cxt, Type& t, Expr& e)
{
  if(is<Integer_type>(e.type())) {
    return cxt.get_array_type(t, e);
  }
  return cxt.get_dynarray_type(t,e);
}


// Returns a slice-of-T type.
//
// FIXME: Verify that we can actually form a slice-of-T. For example,
// is this valid? T[N][]. Perhaps.
Type&
make_slice_type(Context& cxt, Type& t)
{
  return cxt.get_slice_type(t);
}


// Returns a reference-to-T.
Type&
make_reference_type(Context& cxt, Type& t)
{
  return cxt.get_reference_type(t);
}


// Returns an input type for T.
Type&
make_in_type(Context& cxt, Type& t)
{
  return cxt.get_in_type(t);
}


// NOTE: This is really just the same as an lvalue reference to T.
Type&
make_out_type(Context& cxt, Type& t)
{
  return cxt.get_out_type(t);
}


// NOTE: This is really just the same as an lvalue reference to T.
Type&
make_mutable_type(Context& cxt, Type& t)
{
  return cxt.get_mutable_type(t);
}


// NOTE: This isn't quite the same as an rvalue reference because it
// implies a lvalue-to-temporary conversion at the call site.
Type&
make_consume_type(Context& cxt, Type& t)
{
  return cxt.get_consume_type(t);
}


// NOTE: This is a forwarding reference, which means that the actual type
// is deduced. Forwarding references effectively 
Type&
make_forward_type(Context& cxt, Type& t)
{
  return cxt.get_forward_type(t);
}


// Returns a pack type of T.
Type&
make_pack_type(Context& cxt, Type& t)
{
  return cxt.get_pack_type(t);
}


// Returns a fresh type that represents the result of an expression.
Type&
make_fresh_type(Context& cxt)
{
  return cxt.make_placeholder_type();
}


} // namespace banjo
