// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"

#include <unordered_set>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Variables


Variable_decl&
Builder::make_variable_declaration(Name& n, Type& t, Def& d)
{
  lingo_assert(!is<Function_type>(t));
  return Variable_decl::make(alloc_, n, t, d);
}


Variable_decl&
Builder::make_variable_declaration(char const* s, Type& t, Def& d)
{
  lingo_assert(!is<Function_type>(t));
  return make_variable_declaration(get_id(s), t, d);
}


// Create an object declaration whose definition is empty.
Variable_decl&
Builder::make_variable_declaration(Name& n, Type& t)
{
  return make_variable_declaration(n, t, make_empty_definition());
}


// Create an object declaration whose definition is empty.
Variable_decl&
Builder::make_variable_declaration(char const* s, Type& t)
{
  return make_variable_declaration(s, t, make_empty_definition());
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
Builder::make_function_declaration(Name& n, Type& t, Decl_list const& ps, Def& d)
{
  lingo_assert(is<Function_type>(t));
  return Function_decl::make(alloc_, n, t, ps, d);
}


// Create a new function. 
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list&& ps, Def& d)
{
  lingo_assert(is<Function_type>(t));
  return Function_decl::make(alloc_, n, t, std::move(ps), d);
}


// Create a new function with an empty definition.
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list const& ps)
{
  return make_function_declaration(n, t, ps, make_empty_definition());
}


// Create a new function with an empty definition.
Function_decl&
Builder::make_function_declaration(Name& n, Type& t, Decl_list&& ps)
{
  return make_function_declaration(n, t, std::move(ps), make_empty_definition());
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


// Create a class with an empty definition.
Class_decl&
Builder::make_class_declaration(Name& n)
{
  return make_class_declaration(n, make_empty_definition());
}


Class_def&
Builder::make_class_definition(Stmt_list&& s)
{
  return Class_def::make(alloc_, std::move(s));
}


Field_decl&
Builder::make_field_declaration(Name& n, Type& t, Def& d)
{
  return Field_decl::make(alloc_, n, t, d);
}


Field_decl&
Builder::make_field_declaration(char const* s, Type& t, Def& d)
{
  return make_field_declaration(get_id(s), t, d);
}


Field_decl&
Builder::make_field_declaration(Name& n, Type& t)
{
  return make_field_declaration(n, t, make_empty_definition());
}


Field_decl&
Builder::make_field_declaration(char const* s, Type& t)
{
  return make_field_declaration(s, t, make_empty_definition());
}


Method_decl&
Builder::make_method_declaration(Name& n, Type& t, Decl_list const& ps, Def& d)
{
  return Method_decl::make(alloc_, n, t, ps, d);
}


Method_decl&
Builder::make_method_declaration(Name& n, Type& t, Decl_list&& ps, Def& d)
{
  return Method_decl::make(alloc_, n, t, std::move(ps), d);
}


Method_decl&
Builder::make_method_declaration(Name& n, Type& t, Decl_list const& ps)
{
  return make_method_declaration(n, t, ps, make_empty_definition());
}


Method_decl&
Builder::make_method_declaration(Name& n, Type& t, Decl_list&& ps)
{
  return make_method_declaration(n, t, std::move(ps), make_empty_definition());
}


// -------------------------------------------------------------------------- //
// Parameters

Variable_parm&
Builder::make_variable_parameter(Name& n, Type& t)
{
  return Variable_parm::make(alloc_, n, t, make_empty_definition());
}


Variable_parm&
Builder::make_variable_parameter(char const* s, Type& t)
{
  return make_variable_parameter(get_id(s), t);
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
