// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Fundamental types

Void_type&
Builder::get_void_type(Qualifier_set q)
{
  return void_type(object_type, q);
}


Boolean_type&
Builder::get_bool_type(Type_category c, Qualifier_set q)
{
  return bool_type(c, q);
}


Byte_type&
Builder::get_byte_type(Type_category c, Qualifier_set q)
{
  return byte_type(c, q);
}


Integer_type&
Builder::get_integer_type(Type_category c, bool s, int p, Qualifier_set q)
{
  return integer_type(c, s, p, q);
}


Integer_type&
Builder::get_integer_type(bool s, int p, Qualifier_set q)
{
  return integer_type(object_type, s, p, q);
}


// TODO: Default precision should depend on the target platform. That
// information needs to be supplied by an external argument or take
// information from the context.
Integer_type&
Builder::get_int_type(Type_category c, Qualifier_set q)
{
  return get_integer_type(c, true, 32, q);
}


Integer_type&
Builder::get_int_type(Qualifier_set q)
{
  return get_integer_type(object_type, true, 32, q);
}


// TODO: See comments above.
Integer_type&
Builder::get_uint_type(Type_category c, Qualifier_set q)
{
  return get_integer_type(c, false, 32, q);
}


Integer_type&
Builder::get_uint_type(Qualifier_set q)
{
  return get_integer_type(object_type, false, 32, q);
}


Float_type&
Builder::get_float_type(Type_category c, int p, Qualifier_set q)
{
  return float_type(c, p, q);
}


// -------------------------------------------------------------------------- //
// Compound types

Function_type&
Builder::get_function_type(Type_category c, Decl_list const& ps, Type& t)
{
  Type_list ts;
  for (Decl& d : modify(ps))
    ts.push_back(d.type());
  return get_function_type(c, ts, t);
}


Function_type&
Builder::get_function_type(Decl_list const& ps, Type& t)
{
  return get_function_type(banjo::function_type, ps, t);
}


Function_type&
Builder::get_function_type(Type_category c, Type_list const& ts, Type& t)
{
  return function_type(c, ts, t);
}


Function_type&
Builder::get_function_type(Type_list const& ts, Type& t)
{
  return function_type(banjo::function_type, ts, t);
}


Array_type&
Builder::get_array_type(Type_category c, Type& t, Expr& e)
{
  return array_type(c, t, e);
}


Tuple_type&
Builder::get_tuple_type(Type_category c, Type_list const& ts)
{
  return tuple_type(c, ts);
}


Tuple_type&
Builder::get_tuple_type(Type_category c, Type_list&& ts)
{
  return tuple_type(c, std::move(ts));
}


Pointer_type&
Builder::get_pointer_type(Type_category c, Type& t, Qualifier_set q)
{
  return pointer_type(c, t, q);
}


// -------------------------------------------------------------------------- //
// User-defined types

// Returns class type for the given type declaration.
Class_type&
Builder::get_class_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return class_type(c, d, q);
}


// Returns the type corresponding to the declaration of a type parameter.
Typename_type&
Builder::get_typename_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return typename_type(c, d, q);
}


// -------------------------------------------------------------------------- //
// Deduced types

// Returns the auto type corresponding to the given declaration.
Auto_type&
Builder::get_auto_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return auto_type(c, d, q);
}


Decltype_type&
Builder::get_decltype_type(Type_category c, Expr& e, Qualifier_set q)
{
  lingo_unreachable();
  // return {declared_type(e), c, q};
}


// TODO: I hate this type. It should go away...
Type_type&
Builder::get_type_type()
{
  return {type_type()};
}


#if 0
// -------------------------------------------------------------------------- //
// Qualified types

static Type
qualified_array_type(Builder& b, Array_type& t, Type_category c, Qualifier_set q)
{
  Type et = t.element_type();
  et.qual_ |= q;
  return b.get_array_type(c, et, t.extent());
}


static Type
qualified_tuple_type(Builder& b, Tuple_type& t, Type_category c, Qualifier_set q)
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
Builder::get_qualified_type(Type t, Qualifier_set q)
{
  struct fn
  {
    Builder& self;
    Type_category c;
    Qualifier_set q;
    Type operator()(Basic_type& t) { return {t, c, q}; }
    Type operator()(Array_type& t) { return qualified_array_type(self, t, c, q); }
    Type operator()(Tuple_type& t) { return qualified_tuple_type(self, t, c, q); }
  };
  return apply(t, fn{*this, t.category(), q});
}


Type
Builder::get_unqualified_type(Type t)
{
  return get_qualified_type(t, empty_qual);
}


Type
Builder::get_const_type(Type t)
{
  return get_qualified_type(t, const_qual);
}


Type
Builder::get_volatile_type(Type t)
{
  return get_qualified_type(t, volatile_qual);
}


Type
Builder::get_cv_type(Type t)
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
Builder::make_auto_type()
{
  lingo_unimplemented(__func__);

  // Name& n = cxt.get_id();
  // Type_parm& d = cxt.make_type_parameter(Index {}, n);
  // return get_auto_type(d);
}


// Synthesize a type from the given parameter. This is used to generate
// fake types corresponding to type parameters.
Type
Builder::make_synthetic_type(Decl& d)
{
  lingo_unimplemented(__func__);

  // return make<Synthetic_type>(d);
}
#endif

} // namespace banjo
