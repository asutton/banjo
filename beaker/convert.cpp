// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "convert.hpp"
#include "ast.hpp"

namespace beaker
{


// An expression of type T&, where T is an object type, can
// be converted into an expression of type T.
//
// In C++, this a glvalue to prvalue conversion.
//
// FIXME: Check that e's type is complete before invoking the
// conversion.
Expr*
convert_to_value(Expr const& e, Type const& t)
{
  // if (t.is_object_type() && e.type().is_reference_type()) {
  //   Type const& t0 = e.type().remove_reference();
  //   return new Value_conv(t.mutate(), e.mutate());
  // }
  // return e;
  return nullptr;
}


Expr*
convert_category(Expr const& e, Type const& t)
{
  return nullptr;
}


// Try to find a conversion from a source expression `e` and
// a target type `t`.
Expr*
convert(Expr const& e, Type const& t)
{
  return nullptr;
}


} // namespace beaker
