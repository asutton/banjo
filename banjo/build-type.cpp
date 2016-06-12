// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved


namespace banjo
{

// -------------------------------------------------------------------------- //
// Fundamental types

Void_type&
Builder::get_void_type(Qualifier_set q)
{
  return Void_type::make(alloc_, object_type, q);
}


Boolean_type&
Builder::get_bool_type(Type_category c, Qualifier_set q)
{
  lingo_assert(c != function_type);
  return Boolean_type::make(alloc_, c, q);
}


Byte_type&
Builder::get_byte_type(Type_category c, Qualifier_set q)
{
  lingo_assert(c != function_type);
  return Byte_type::make(alloc_, c, q);
}


Integer_type&
Builder::get_integer_type(Type_category c, bool s, int p, Qualifier_set q)
{
  lingo_assert(c != function_type);
  return Integer_type::make(alloc_, c, s, p, q);
}


Integer_type&
Builder::get_integer_type(bool s, int p, Qualifier_set q)
{
  return get_integer_type(object_type, s, p, q);
}


// TODO: Default precision should depend on the target platform. That
// information needs to be supplied by an external argument or take
// information from the context.
Integer_type&
Builder::get_int_type(Type_category c, Qualifier_set q)
{
  return get_integer_type(c, true, 32, q);
}


// TODO: See comments above.
Integer_type&
Builder::get_uint_type(Type_category c, Qualifier_set q)
{
  return get_integer_type(c, false, 32, q);
}


Float_type&
Builder::get_float_type(Type_category c, int p, Qualifier_set q)
{
  return Float_type::make(alloc_, c, p, q);
}


Float_type&
Builder::get_float_type(int p, Qualifier_set q)
{
  return get_float_type(object_type, p, q);
}


// -------------------------------------------------------------------------- //
// Compound types

Function_type&
Builder::get_function_type(Type_category c, Type_list const& ts, Type& t, Qualifier_set q)
{
  return Function_type::make(alloc_, c, ts, t, q);
}


Function_type&
Builder::get_function_type(Type_category c, Type_list&& ts, Type& t, Qualifier_set q)
{
  return Function_type::make(alloc_, c, std::move(ts), t, q);
}


Function_type&
Builder::get_function_type(Type_list const& ts, Type& t, Qualifier_set q)
{
  return get_function_type(banjo::function_type, ts, t, q);
}


Function_type&
Builder::get_function_type(Type_list&& ts, Type& t, Qualifier_set q)
{
  return get_function_type(banjo::function_type, std::move(ts), t, q);
}


Array_type&
Builder::get_array_type(Type_category c, Type& t, Expr& e)
{
  return Array_type::make(alloc_, c, t, e);
}


Tuple_type&
Builder::get_tuple_type(Type_category c, Type_list const& ts)
{
  return Tuple_type::make(alloc_, c, ts);
}


Tuple_type&
Builder::get_tuple_type(Type_category c, Type_list&& ts)
{
  return Tuple_type::make(alloc_, c, std::move(ts));
}


Pointer_type&
Builder::get_pointer_type(Type_category c, Type& t, Qualifier_set q)
{
  return Pointer_type::make(alloc_, c, t, q);
}


// -------------------------------------------------------------------------- //
// User-defined types

// Returns class type for the given type declaration.
Class_type&
Builder::get_class_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return Class_type::make(alloc_, c, d, q);
}


// Returns the type corresponding to the declaration of a type parameter.
Typename_type&
Builder::get_typename_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return Typename_type::make(alloc_, c, d, q);
}


// -------------------------------------------------------------------------- //
// Deduced types

// Returns the auto type corresponding to the given declaration.
Auto_type&
Builder::get_auto_type(Type_category c, Type_decl& d, Qualifier_set q)
{
  return Auto_type::make(alloc_, c, d, q);
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
  return {Type_type::make(alloc_)};
}


// -------------------------------------------------------------------------- //
// Reference types


static inline Type&
ref_bool_type(Builder& b, Boolean_type& t, Type_category c)
{
  return b.get_bool_type(c, t.qualifiers());
}


static inline Type&
ref_integer_type(Builder& b, Integer_type& t, Type_category c)
{
  return b.get_integer_type(c, t.sign(), t.precision(), t.qualifiers());
}


static inline Type&
ref_float_type(Builder& b, Float_type& t, Type_category c)
{
  return b.get_float_type(c, t.precision(), t.qualifiers());
}


static inline Type&
ref_function_type(Builder& b, Function_type& t, Type_category c)
{
  Type_list const& parms = t.parameter_types();
  Type& ret = t.return_type();
  return b.get_function_type(c, parms, ret);
}


static inline Type&
ref_array_type(Builder& b, Array_type& t, Type_category c)
{
  return b.get_array_type(c, t.element_type(), t.extent());
}


static inline Type&
ref_tuple_type(Builder& b, Tuple_type& t, Type_category c)
{
  return b.get_tuple_type(c, t.element_types());
}


static inline Type&
ref_pointer_type(Builder& b, Pointer_type& t, Type_category c)
{
  return b.get_pointer_type(c, t.type(), t.qualifiers());
}


static inline Type&
ref_class_type(Builder& b, Class_type& t, Type_category c)
{
  return b.get_class_type(c, t.declaration(), t.qualifiers());
}


// Returns a variant of `t` that is a reference.
Type&
Builder::get_reference_type(Type& t)
{
  if (t.is_reference())
    return t;
  struct fn
  {
    Builder& b;
    Type& operator()(Type& t)          { lingo_unhandled(t); }
    Type& operator()(Void_type& t)     { lingo_unreachable(); }
    Type& operator()(Boolean_type& t)  { return ref_bool_type(b, t, reference_type); }
    Type& operator()(Integer_type& t)  { return ref_integer_type(b, t, reference_type); }
    Type& operator()(Float_type& t)    { return ref_float_type(b, t, reference_type); }
    Type& operator()(Function_type& t) { return ref_function_type(b, t, reference_type); }
    Type& operator()(Array_type& t)    { return ref_array_type(b, t, reference_type); }
    Type& operator()(Tuple_type& t)    { return ref_tuple_type(b, t, reference_type); }
    Type& operator()(Pointer_type& t)  { return ref_pointer_type(b, t, reference_type); }
    Type& operator()(Class_type& t)    { return ref_class_type(b, t, reference_type); }
  };
  return apply(t, fn{*this});
}


// Returns the non-reference version of `t`, as if after a reference-to-object
// or reference-to-function conversion.
Type&
Builder::get_non_reference_type(Type& t)
{
  if (t.is_object() || t.is_function())
    return t;
  struct fn
  {
    Builder& b;
    Type& operator()(Type& t)          { lingo_unhandled(t); }
    Type& operator()(Void_type& t)     { lingo_unreachable(); }
    Type& operator()(Boolean_type& t)  { return ref_bool_type(b, t, object_type); }
    Type& operator()(Integer_type& t)  { return ref_integer_type(b, t, object_type); }
    Type& operator()(Float_type& t)    { return ref_float_type(b, t, object_type); }
    Type& operator()(Function_type& t) { return ref_function_type(b, t, function_type); }
    Type& operator()(Array_type& t)    { return ref_array_type(b, t, object_type); }
    Type& operator()(Tuple_type& t)    { return ref_tuple_type(b, t, object_type); }
    Type& operator()(Pointer_type& t)  { return ref_pointer_type(b, t, object_type); }
    Type& operator()(Class_type& t)    { return ref_class_type(b, t, object_type); }
  };
  return apply(t, fn{*this});
}


// -------------------------------------------------------------------------- //
// Qualified types


static inline Type&
qual_void_type(Builder& b, Void_type& t, Qualifier_set q)
{
  return b.get_void_type(q);
}


static inline Type&
qual_bool_type(Builder& b, Boolean_type& t, Qualifier_set q)
{
  return b.get_bool_type(t.category(), q);
}


static inline Type&
qual_byte_type(Builder& b, Byte_type& t, Qualifier_set q)
{
  return b.get_byte_type(t.category(), q);
}


static inline Type&
qual_integer_type(Builder& b, Integer_type& t, Qualifier_set q)
{
  return b.get_integer_type(t.category(), t.sign(), t.precision(), q);
}


static inline Type&
qual_float_type(Builder& b, Float_type& t, Qualifier_set q)
{
  return b.get_float_type(t.category(), t.precision(), q);
}


// Returns an function type qualified by q. Note that there are qualifiers
// on function types (noexcept, meta).
static inline Type&
qual_function_type(Builder& b, Function_type& t, Qualifier_set q)
{
  Type_list const& parms = t.parameter_types();
  Type& ret = t.return_type();
  return b.get_function_type(t.category(), parms, ret, q);
}


// Returns an array type with its element type qualified by q.
static inline Type&
qual_array_type(Builder& b, Array_type& t, Qualifier_set q)
{
  Type& elem = b.get_qualified_type(t.element_type(), q);
  return b.get_array_type(t.category(), elem, t.extent());
}


// Returns a tuple type with each of it element types qualified by q.
static inline Type&
qual_tuple_type(Builder& b, Tuple_type& t, Qualifier_set q)
{
  Type_list elems;
  for (Type& elem : t.element_types())
    elems.push_back(b.get_qualified_type(elem, q));
  return b.get_tuple_type(t.category(), std::move(elems));
}


static inline Type&
qual_pointer_type(Builder& b, Pointer_type& t, Qualifier_set q)
{
  return b.get_pointer_type(t.category(), t.type(), q);
}


static inline Type&
qual_class_type(Builder& b, Class_type& t, Qualifier_set q)
{
  return b.get_class_type(t.category(), t.declaration(), q);
}


// Return a variant `t` qualified by `q`. Note that this will not
// validate that the qualifiers can actually be applied to t.
//
// TODO: Is there a design that would prevent me from having to use the
// specific constructors for every single type?
Type&
Builder::get_qualified_type(Type& t, Qualifier_set q)
{
  struct fn
  {
    Builder& b;
    Qualifier_set q;
    Type& operator()(Type& t)          { lingo_unhandled(t); }
    Type& operator()(Void_type& t)     { return qual_void_type(b, t, q); }
    Type& operator()(Boolean_type& t)  { return qual_bool_type(b, t, q); }
    Type& operator()(Byte_type& t)     { return qual_byte_type(b, t, q); }
    Type& operator()(Integer_type& t)  { return qual_integer_type(b, t, q); }
    Type& operator()(Float_type& t)    { return qual_float_type(b, t, q); }
    Type& operator()(Function_type& t) { return qual_function_type(b, t, q); }
    Type& operator()(Array_type& t)    { return qual_array_type(b, t, q); }
    Type& operator()(Tuple_type& t)    { return qual_tuple_type(b, t, q); }
    Type& operator()(Pointer_type& t)  { return qual_pointer_type(b, t, q); }
  };
  return apply(t, fn{*this, q});
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
