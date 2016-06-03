// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILD_TYPE_HPP
#define BANJO_BUILD_TYPE_HPP

#include "build-core.hpp"


namespace banjo
{

// The type builder is responsible for the allocation of types.
struct Type_builder : Builder_base
{
  using Builder_base::Builder_base;

  // Fundamental types
  Type get_void_type(Qualifier_set = {});
  Type get_bool_type(Qualifier_set = {}, Reference_kind = {});
  Type get_byte_type(Qualifier_set = {}, Reference_kind = {});
  Type get_integer_type(bool, int, Qualifier_set = {}, Reference_kind = {});
  Type get_int_type(Qualifier_set = {}, Reference_kind = {});
  Type get_uint_type(Qualifier_set = {}, Reference_kind = {});
  Type get_float_type(int, Qualifier_set = {}, Reference_kind = {});

  // Composite types
  Type get_function_type(Decl_list const&, Type, Reference_kind = {});
  Type get_function_type(Type_list const&, Type, Reference_kind = {});
  Type get_array_type(Type, Expr&, Reference_kind = {});
  Type get_tuple_type(Type_list&&, Reference_kind = {});
  Type get_tuple_type(Type_list const&, Reference_kind = {});
  Type get_pointer_type(Type, Qualifier_set = {}, Reference_kind = {});

  // User-defined types
  Type get_class_type(Type_decl&, Qualifier_set = {}, Reference_kind = {});
  Type get_typename_type(Type_decl&, Qualifier_set = {}, Reference_kind = {});
  
  // Placeholder types
  Type get_auto_type(Type_decl&, Qualifier_set = {}, Reference_kind = {});
  Type get_decltype_type(Expr&, Qualifier_set = {}, Reference_kind = {});

  // Meta-types
  Type get_type_type();

  // Qualified types
  Type get_qualified_type(Type&, Qualifier_set);
  Type get_unqualified_type(Type&);
  Type get_const_type(Type&);
  Type get_volatile_type(Type&);
  Type get_cv_type(Type&);

  // Fresh types
  Type make_auto_type();
  Type make_synthetic_type(Decl&);

  Single_factory<Void_type> void_type;
  Single_factory<Boolean_type> bool_type;
  Single_factory<Byte_type> byte_type;
  Unique_factory<Integer_type> integer_type;
  Unique_factory<Float_type> float_type;
  Unique_factory<Function_type> function_type;
  Unique_factory<Array_type> array_type;
  Unique_factory<Tuple_type> tuple_type;
  Unique_factory<Pointer_type> pointer_type;
  Unique_factory<Class_type> class_type;
  Unique_factory<Typename_type> typename_type;
  Unique_factory<Auto_type> auto_type;
  Unique_factory<Decltype_type> decltype_type;
  Single_factory<Type_type> type_type;
};


} // namespace banjo


#endif
