// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "build-type.hpp"

#include <unordered_set>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Fundamental types

Type
Type_builder::get_void_type(Type_category c, Qualifier_set q)
{
  return {void_type(), c, q};
}


Type
Type_builder::get_bool_type(Type_category c, Qualifier_set q)
{
  return {bool_type(), c, q};
}


Type
Type_builder::get_byte_type(Type_category c, Qualifier_set q)
{
  return {byte_type(), c, q};
}


Type
Type_builder::get_integer_type(Type_category c, bool s, int p, Qualifier_set q)
{
  return {integer_type(s, p), c, q};
}


// TODO: Default precision should depend on the target platform. That
// information needs to be supplied by an external argument or take
// information from the context.
Type
Type_builder::get_int_type(Type_category c, Qualifier_set q)
{
  return get_integer_type(c, true, 32, q);
}


// TODO: See comments above.
Type
Type_builder::get_uint_type(Type_category c, Qualifier_set q)
{
  return get_integer_type(c, false, 32, q);
}


Type
Type_builder::get_float_type(Type_category c, int p, Qualifier_set q)
{
  return {float_type(p), c, q};
}


// -------------------------------------------------------------------------- //
// Compound types

Type
Type_builder::get_function_type(Type_category c, Decl_list const& ps, Type t)
{
  Type_list ts;
  for (Decl& d : modify(ps))
    ts.push_back(d.type());
  return get_function_type(c, ts, t);
}


Type
Type_builder::get_function_type(Type_category c, Type_list const& ts, Type t)
{
  return {function_type(ts, t), c};
}


Type
Type_builder::get_array_type(Type_category c, Type t, Expr& e)
{
  return {array_type(t, e), c, empty_qual};
}


Type
Type_builder::get_tuple_type(Type_category c, Type_list&& t)
{
  return {tuple_type(std::move(t)), c};
}


Type
Type_builder::get_tuple_type(Type_category c, Type_list const& t)
{
  return {tuple_type(t), c};
}


Type
Type_builder::get_pointer_type(Type_category c, Type t, Qualifier_set q)
{
  return {pointer_type(t), c, q};
}


// -------------------------------------------------------------------------- //
// User-defined types

// Returns class type for the given type declaration.
Type
Type_builder::get_class_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return {class_type(d), c, q};
}


// Returns the type corresponding to the declaration of a type parameter.
Type
Type_builder::get_typename_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return {typename_type(d), c, q};
}


// -------------------------------------------------------------------------- //
// Deduced types

// Returns the auto type corresponding to the given declaration.
Type
Type_builder::get_auto_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return {auto_type(d), c, q};
}


Type
Type_builder::get_decltype_type(Type_category c, Expr& e, Qualifier_set q)
{
  lingo_unreachable();
  // return {declared_type(e), c, q};
}


// TODO: I hate this type. It should go away...
Type
Type_builder::get_type_type()
{
  return {type_type()};
}


// -------------------------------------------------------------------------- //
// Qualified types


static Type
qualified_array_type(Type_builder& b, Array_type& t, Type_category c, Qualifier_set q)
{
  Type et = t.element_type();
  et.qual_ |= q;
  return b.get_array_type(c, et, t.extent());
}


static Type
qualified_tuple_type(Type_builder& b, Tuple_type& t, Type_category c, Qualifier_set q)
{
  Type_list ts;
  for (Type et : t.element_types()) {
    et.qual_ |= q;
    ts.push_back(et);
  }
  return b.get_tuple_type(c, std::move(ts));
}


// Return a type qualified by q. 
//
// TODO: Handle reference types correctly.
//
// TODO: This seems more algorithmic that constructive. I should move it
// into the type module.
Type
Type_builder::get_qualified_type(Type t, Qualifier_set q)
{
  struct fn
  {
    Type_builder& self;
    Type_category c;
    Qualifier_set q;
    Type operator()(Basic_type& t) { return {t, c, q}; }
    Type operator()(Array_type& t) { return qualified_array_type(self, t, c, q); }
    Type operator()(Tuple_type& t) { return qualified_tuple_type(self, t, c, q); }
  };
  return apply(t, fn{*this, t.category(), q});
}


Type
Type_builder::get_unqualified_type(Type t)
{
  return get_qualified_type(t, empty_qual);
}


Type
Type_builder::get_const_type(Type t)
{
  return get_qualified_type(t, const_qual);
}


Type
Type_builder::get_volatile_type(Type t)
{
  return get_qualified_type(t, volatile_qual);
}


Type
Type_builder::get_cv_type(Type t)
{
  return get_qualified_type(t, cv_qual);
}


// -------------------------------------------------------------------------- //
// Fresh types


// Create a new auto placeholder type. This creates a new, unique type
// parameter and returns its associated type. Note that the type parameter
// is unassociated with any context at the point of creation (it's an
// unbound, unnamed type variable).
Type
Type_builder::make_auto_type()
{
  lingo_unimplemented(__func__);

  // Name& n = cxt.get_id();
  // Type_parm& d = cxt.make_type_parameter(Index {}, n);
  // return get_auto_type(d);
}


// Synthesize a type from the given parameter. This is used to generate
// fake types corresponding to type parameters.
Type
Type_builder::make_synthetic_type(Decl& d)
{
  lingo_unimplemented(__func__);

  // return make<Synthetic_type>(d);
}


} // namespace banjo
