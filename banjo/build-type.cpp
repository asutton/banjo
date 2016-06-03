// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "build-type.hpp"

#include <unordered_set>


namespace banjo
{


// -------------------------------------------------------------------------- //
// Fundamental types


Type
Type_builder::get_void_type(Qualifier_set q)
{
  return {void_type(), q};
}


Type
Type_builder::get_bool_type(Qualifier_set q, Reference_kind r)
{
  return {bool_type(), q, r};
}


Type
Type_builder::get_byte_type(Qualifier_set q, Reference_kind r)
{
  return {byte_type(), q, r}
}


Type
Type_builder::get_integer_type(bool s, int p, Qualifier_set q, Reference_kind r)
{
  return {integer_type(s, p), q, r};
}


// TODO: Default precision should depend on the target platform. That
// information needs to be supplied by an external argument.
Type
Type_builder::get_int_type(Qualifier_set q, Reference_kind r)
{
  return get_integer_type(true, 32, q, r);
}


// TODO: See comments above.
Type
Type_builder::get_uint_type(Qualifier_set q, Reference_kind r)
{
  return get_integer_type(false, 32, q, r);
}


Type
Type_builder::get_float_type(int p, Qualifier_set q, Reference_kind r)
{
  return {float_type(p), q, r};
}


// -------------------------------------------------------------------------- //
// Compound types

Type
Type_builder::get_function_type(Decl_list const& ps, Type t, Reference_kind r)
{
  Type_list ts;
  for (Decl& d : modify(ps))
    ts.push_back(p.type());
  return get_function_type(ts, t, r);
}


Type
Type_builder::get_function_type(Type_list const& ts, Type t, Reference_kind r)
{
  return {function_type(ts, t), empty_qual, r};
}


Type
Type_builder::get_array_type(Type t, Expr& e, Reference_kind r)
{
  return {array_type(t, e), empty_qual, r};
}


Type
Type_builder::get_tuple_type(Type_list&& t, Reference_kind r)
{
  return {tuple_type(std::move(t)), r};
}


Type
Type_builder::get_tuple_type(Type_list const& t, Reference_kind r)
{
  return {tuple_type(t), r};
}


Type
Type_builder::get_pointer_type(Type t, Qualifier_set q, Reference_kind r)
{
  return {pointer_type(t), q, r};
}


// -------------------------------------------------------------------------- //
// User-defined types

// Returns class type for the given type declaration.
Type
Type_builder::get_class_type(Type_decl& d, Qualifier_set q, Reference_kind r)
{
  return {class_type(d), q, r};
}


// Returns the type corresponding to the declaration of a type parameter.
Type
Type_builder::get_typename_type(Type_decl& d, Qualifier_set q, Reference_kind r)
{
  return {typename_type(d), q, r};
}


// -------------------------------------------------------------------------- //
// Deduced types

// Returns the auto type corresponding to the given declaration.
Type
Type_builder::get_auto_type(Type_decl& d, Qualifier_set q, Reference_kind r)
{
  return {auto_type(d), q, r};
}


Type
Type_builder::get_decltype_type(Expr& e, Qualifier_set q, Reference_kind r)
{
  return {declared_type(e), q, r};
}


Type
Type_builder::get_type_type()
{
  return {type_type(), q, r};
}


// -------------------------------------------------------------------------- //
// Qualified types


static Type
qualified_array_type(Type_builder& b, Array_type& t, Qualifier_set q, Reference_kind r)
{
  Type et(t.element_type(), q);
  return b.get_array_type(et, empty_qual, r);
}


static Type
qualified_tuple_type(Type_builder& b, Tuple_type& t, Qualifier_set q, Reference_kind r)
{
  Type_list ts;
  for (Type& et : t.element_types())
    ts.push_back({et, q});
  return b.get_tuple_type(std::move(ts), empty_qual, r);
}


// Return a type qualified by q. This preserves the reference kind of the 
// original type. Qualifiers applied to an array or tuple type apply to their 
// element type(s).
Type
Type_builder::get_qualified_type(Type t, Qualifier_set q)
{
  struct fn
  {
    Type_builder&  self;
    Reference_kind r;
    Type operator()(Basic_type& t) { return {t, q, r}; }
    Type operator()(Array_type& t) { return qualified_array_type(self, t, q, r); }
    Type operator()(Tuple_type& t) { return qualified_tuple_type{self, t, q, r}; }
  };
  return apply(t, fn{r});
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
