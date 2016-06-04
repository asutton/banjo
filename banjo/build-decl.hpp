// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILD_DECL_HPP
#define BANJO_BUILD_DECL_HPP

#include "build-core.hpp"


namespace banjo
{

// The statement builder creates statement nodes.
struct Decl_builder : Builder_base
{
  using Builder_base::Builder_base;

  // Objects and references
  Object_decl&    make_variable_declaration(Name&, Type);
  Object_decl&    make_variable_declaration(Name&, Type, Expr&);
  Reference_decl& make_reference_declaration(Name&, Type);
  Reference_decl& make_reference_declaration(Name&, Type, Expr&);
  Field_decl&     make_field_declaration(Name&, Type);
  Field_decl&     make_field_declaration(Name&, Type, Expr&);
  Super_decl&     make_super_declaration(Type);

  // Functions and methods
  Function_decl&  make_function_declaration(Name&, Type, Decl_list const&);
  Function_decl&  make_function_declaration(Name&, Type, Decl_list&&);
  Function_decl&  make_function_declaration(Name&, Type, Decl_list&&, Def&);
  Function_def&   make_function_definition(Stmt&);
  Method_decl&    make_method_declaration(Name&, Decl_list const&, Type);
  Expression_def& make_function_definition(Expr&);
  Intrinsic_def&  make_function_definition(Nullary_fn);
  Intrinsic_def&  make_function_definition(Unary_fn);

  // Classes
  Class_decl&  make_class_declaration(Name&, Type);
  Class_decl&  make_class_declaration(Name&, Type, Def&);
  Class_def&   make_class_definition(Stmt_list&&);

  // Parameters
  Object_parm&    make_object_parameter(Name&, Type);
  Object_parm&    make_object_parameter(char const*, Type);
  Reference_parm& make_reference_parameter(Name&, Type);
  Reference_parm& make_reference_parameter(char const*, Type);
  Type_parm&      make_type_parameter(Name&);
  Type_parm&      make_type_parameter(char const*);
  
  // General purpose definitions
  Empty_def&      make_empty_definition();
  Deleted_def&    make_deleted_definition();
  Defaulted_def&  make_defaulted_definition();
  Expression_def& make_expression_definition(Expr&);

  Basic_factory<Object_decl> object_decl;
  Basic_factory<Reference_decl> reference_decl;
  Basic_factory<Field_decl> field_decl;
  Basic_factory<Super_decl> super_decl;
  Basic_factory<Function_decl> function_decl;
  Basic_factory<Method_decl> method_decl;
  Basic_factory<Class_decl> class_decl;
  
  Basic_factory<Object_parm> object_parm;
  Basic_factory<Reference_parm> reference_parm;
  Basic_factory<Type_parm> type_parm;
  
  Basic_factory<Empty_def> empty_def;
  Basic_factory<Deleted_def> deleted_def;
  Basic_factory<Defaulted_def> defaulted_def;
  Basic_factory<Expression_def> expression_def;
  Basic_factory<Class_def> class_def;
  Basic_factory<Intrinsic_def> intrinsic_def;
};


} // namespace banjo


#endif
