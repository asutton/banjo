// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"

#include <unordered_set>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Objects and references

Object_decl&
Builder::make_variable_declaration(Name& n, Type& t)
{
  Def& d = make_empty_definition();
  return object_decl(n, t, d);
}


Object_decl&
Builder::make_variable_declaration(Name& n, Type& t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return object_decl(n, t, d);
}


// Create a reference declaration that binds to nothing.
Reference_decl&
Builder::make_reference_declaration(Name& n, Type& t)
{
  Def& d = make_empty_definition();
  return reference_decl(n, t, d);
}


// Create a reference declaration that binds to an expression.
Reference_decl&
Builder::make_reference_declaration(Name& n, Type& t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return reference_decl(n, t, d);
}


// Create a member variable declaration.
Field_decl&
Builder::make_field_declaration(Name& n, Type& t)
{
  Def& d = make_empty_definition();
  return field_decl(n, t, d);
}


Field_decl&
Builder::make_field_declaration(Name& n, Type& t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return field_decl(n, t, d);
}


// Make an unnamed base class sub-object declaration.
Super_decl&
Builder::make_super_declaration(Type& t)
{
  return super_decl(get_id(), t, make_empty_definition());
}


// -------------------------------------------------------------------------- //
// Functions

// Create a new function with an empty definition.
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list const& p)
{
  return function_decl(n, t, p, make_empty_definition());
}


// Create a new function with an empty definition.
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list&& p)
{
  return make_function_declaration(n, t, std::move(p), make_empty_definition());
}


// Create a new function. 
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list&& p, Def& d)
{
  return function_decl(n, t, std::move(p), d);
}


// Create a new method with an empty definition.
Method_decl&
Builder::make_method_declaration(Name& n, Type& t, Decl_list const& p)
{
  return method_decl(n, t, p, make_empty_definition());
}


Method_decl&
Builder::make_method_declaration(Name& n, Type& t, Decl_list&& p)
{
  return method_decl(n, t, std::move(p), make_empty_definition());
}


Method_decl&
Builder::make_method_declaration(Name& n, Type& t, Decl_list&& p, Def& d)
{
  return method_decl(n, t, std::move(p), d);
}


// Create a function definition for the given statement.
Function_def&
Builder::make_function_definition(Stmt& s)
{
  return function_def(s);
}


// Create an expression definition.
Expression_def&
Builder::make_function_definition(Expr& e)
{
  return expression_def(e);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Nullary_fn f)
{
  return intrinsic_def(f);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Unary_fn f)
{
  return intrinsic_def(f);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Binary_fn f)
{
  return intrinsic_def(f);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Ternary_fn f)
{
  return intrinsic_def(f);
}


// Make a class declaration with an empty definition. 
Class_decl&
Builder::make_class_declaration(Name& n, Type& t)
{
  return class_decl(n, t, make_empty_definition());
}


// Create a class having the given definition.
Class_decl&
Builder::make_class_declaration(Name& n, Type& t, Def& d)
{
  return class_decl(n, t, d);
}


Class_def&
Builder::make_class_definition(Stmt_list&& s)
{
  return class_def(std::move(s));
}


Object_parm&
Builder::make_object_parameter(Name& n, Type& t)
{
  return object_parm(n, t);
}


Object_parm&
Builder::make_object_parameter(char const* s, Type& t)
{
  return object_parm(get_id(s), t);
}


Reference_parm&
Builder::make_reference_parameter(Name& n, Type& t)
{
  return reference_parm(n, t);
}


Reference_parm&
Builder::make_reference_parameter(char const* s, Type& t)
{
  return reference_parm(get_id(s), t);
}


Type_parm&
Builder::make_type_parameter(Name& n)
{
  return type_parm(n);
}


Type_parm&
Builder::make_type_parameter(char const* n)
{
  return type_parm(get_id(n));
}


// -------------------------------------------------------------------------- //
// Definitions

Empty_def&
Builder::make_empty_definition()
{
  return empty_def();
}

Deleted_def&
Builder::make_deleted_definition()
{
  return deleted_def();
}


Defaulted_def&
Builder::make_defaulted_definition()
{
  return defaulted_def();
}


Expression_def&
Builder::make_expression_definition(Expr& e)
{
  return expression_def(e);
}


// -------------------------------------------------------------------------- //
// Modules

Translation_unit&
Builder::make_translation_unit()
{
  return translation_unit();
}


Translation_unit&
Builder::make_translation_unit(Stmt_list&& ss)
{
  return translation_unit(std::move(ss));
}
} // namespace banjo
