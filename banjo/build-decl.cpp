// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"

#include <unordered_set>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Variables


Object_decl&
Builder::make_object_declaration(Name& n, Type& t, Def& d)
{
  return Object_decl::make(alloc_, n, t, d);
}


Reference_decl&
Builder::make_reference_declaration(Name& n, Type& t, Def& d)
{
  return Reference_decl::make(alloc_, n, t, d);
}


Empty_def&
Builder::make_variable_initializer()
{
  return make_empty_definition();
}


// For e to be valid, it must be an initializer.
Expression_def&
Builder::make_variable_initializer(Expr& e)
{
  return make_expression_definition(e);
}


// -------------------------------------------------------------------------- //
// Functions

// Create a new function. 
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list const& p, Def& d)
{
  return Function_decl::make(alloc_, n, t, p, d);
}


// Create a new function. 
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list&& p, Def& d)
{
  return Function_decl::make(alloc_, n, t, std::move(p), d);
}


// Create a function definition for the given statement.
Function_def&
Builder::make_function_definition(Stmt& s)
{
  return Function_def::make(alloc_, s);
}


// Create an expression definition.
Expression_def&
Builder::make_function_definition(Expr& e)
{
  return Expression_def::make(alloc_, e);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Nullary_fn f)
{
  return Intrinsic_def::make(alloc_, f);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Unary_fn f)
{
  return Intrinsic_def::make(alloc_, f);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Binary_fn f)
{
  return Intrinsic_def::make(alloc_, f);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Ternary_fn f)
{
  return Intrinsic_def::make(alloc_, f);
}


// -------------------------------------------------------------------------- //
// Classes and their members

// Create a class having the given definition.
Class_decl&
Builder::make_class_declaration(Name& n, Def& d)
{
  return Class_decl::make(alloc_, n, d);
}


Class_def&
Builder::make_class_definition(Stmt_list&& s)
{
  return Class_def::make(alloc_, std::move(s));
}


// -------------------------------------------------------------------------- //
// Parameters

Object_parm&
Builder::make_object_parameter(Name& n, Type& t)
{
  return Object_parm::make(alloc_, n, t, make_empty_definition());
}


Object_parm&
Builder::make_object_parameter(char const* s, Type& t)
{
  return make_object_parameter(get_id(s), t);
}


Reference_parm&
Builder::make_reference_parameter(Name& n, Type& t)
{
  return Reference_parm::make(alloc_, n, t, make_empty_definition());
}


Reference_parm&
Builder::make_reference_parameter(char const* s, Type& t)
{
  return make_reference_parameter(get_id(s), t);
}


Type_parm&
Builder::make_type_parameter(Name& n)
{
  return Type_parm::make(alloc_, n, make_empty_definition());
}


Type_parm&
Builder::make_type_parameter(char const* s)
{
  return make_type_parameter(get_id(s));
}


// -------------------------------------------------------------------------- //
// Definitions

Empty_def&
Builder::make_empty_definition()
{
  return Empty_def::make(alloc_);
}

Deleted_def&
Builder::make_deleted_definition()
{
  return Deleted_def::make(alloc_);
}


Defaulted_def&
Builder::make_defaulted_definition()
{
  return Defaulted_def::make(alloc_);
}


Expression_def&
Builder::make_expression_definition(Expr& e)
{
  return Expression_def::make(alloc_, e);
}


// -------------------------------------------------------------------------- //
// Modules

Translation_unit&
Builder::make_translation_unit()
{
  return Translation_unit::make(alloc_);
}


Translation_unit&
Builder::make_translation_unit(Stmt_list&& ss)
{
  return Translation_unit::make(alloc_, std::move(ss));
}


} // namespace banjo
