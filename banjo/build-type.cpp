// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Cloning

static inline Void_type&
clone_void_type(Builder& b, Void_type& t)
{
  return b.get_void_type(t.qualifiers());
}


static inline Boolean_type&
clone_bool_type(Builder& b, Boolean_type& t)
{
  return b.get_bool_type(t.category(), t.qualifiers());
}


static inline Byte_type&
clone_byte_type(Builder& b, Byte_type& t)
{
  return b.get_byte_type(t.category(), t.qualifiers());
}


static inline Integer_type&
clone_int_type(Builder& b, Integer_type& t)
{
  return b.get_integer_type(t.category(), t.precision(), t.sign(), t.qualifiers());
}


static inline Float_type&
clone_float_type(Builder& b, Float_type& t)
{
  return b.get_float_type(t.category(), t.precision(), t.qualifiers());
}


static inline Function_type&
clone_function_type(Builder& b, Function_type& t)
{
  Type_list parms = b.clone(t.parameter_types());
  Type& ret = b.clone(t.return_type());
  return b.get_function_type(t.category(), std::move(parms), ret);
}


static inline Array_type&
clone_array_type(Builder& b, Array_type& t)
{
  Type& elem = b.clone(t.element_type());
  Expr& ext = b.clone(t.extent());
  return b.get_array_type(t.category(), elem, ext);
}


static inline Tuple_type&
clone_tuple_type(Builder& b, Tuple_type& t)
{
  Type_list elems = b.clone(t.element_types());
  return b.get_tuple_type(t.category(), elems);
}


static inline Pointer_type&
clone_pointer_type(Builder& b, Pointer_type& t)
{
  Type& elem = b.clone(t.type());
  return b.get_pointer_type(t.category(), elem, t.qualifiers());
}


static inline Class_type&
clone_class_type(Builder& b, Class_type& t)
{
  Class_decl& decl = t.declaration();
  return b.get_class_type(t.category(), decl, t.qualifiers());
}


Type&
Builder::clone(Type& t)
{
  struct fn
  {
    Builder& b;
    Type operator()(Type& t)          { lingo_unhandled(t); }
    Type operator()(Void_type& t)     { return b.clone_void_type(t);
    Type operator()(Boolean_type& t)  { return b.clone_bool_type(t); }
    Type operator()(Byte_type& t)     { return b.clone_byte_type(t); }
    Type operator()(Integer_type& t)  { return b.clone_int_type(t); }
    Type operator()(Float_type& t)    { return b.clone_float_type(t); }
    Type operator()(Function_type& t) { return b.clone_function_type(t); }
    Type operator()(Array_type& t)    { return b.clone_array_type(t); }
    Type operator()(Tuple_type& t)    { return b.clone_tuple_type(t); }
    Type operator()(Pointer_type& t)  { return b.clone_pointer_type(t); }
    Type operator()(Class_type& t)    { return b.clone_class_type(t); }
  };
  return apply(t, fn{*this});
}


Type_list
Builder::clone(Type_list& ts)
{
  Type_list ret;
  for (Type& t : ts)
    ret.push_back(clone(t));
  return ret;
}


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


// Return a copy of t that qualified by q. Note that this will not
// validate that the qualifiers can actually be applied to t.
Type&
Builder::get_qualified_type(Type& t, Qualifier_set q)
{

  return apply(t, fn{*this, t.category(), q});
}



#if 0

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
