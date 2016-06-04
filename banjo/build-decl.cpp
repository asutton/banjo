// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"

#include <unordered_set>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Objects and references

Object_decl&
Builder::make_variable_declaration(Name& n, Type t)
{
  Def& d = make_empty_definition();
  return object_decl(n, t, d);
}


Object_decl&
Builder::make_variable_declaration(Name& n, Type t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return object_decl(n, t, d);
}


// Create a reference declaration that binds to nothing.
Reference_decl&
Builder::make_reference_declaration(Name& n, Type t)
{
  Def& d = make_empty_definition();
  return reference_decl(n, t, d);
}


// Create a reference declaration that binds to an expression.
Reference_decl&
Builder::make_reference_declaration(Name& n, Type t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return reference_decl(n, t, d);
}


// Create a member variable declaration.
Field_decl&
Builder::make_field_declaration(Name& n, Type t)
{
  Def& d = make_empty_definition();
  return field_decl(n, t, d);
}


Field_decl&
Builder::make_field_declaration(Name& n, Type t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return field_decl(n, t, d);
}


Super_decl&
Builder::make_super_declaration(Type t)
{
  Def& d = make_empty_definition();
  return super_decl(t, d);
}


// -------------------------------------------------------------------------- //
// Functions

// Create a new function with an empty definition.
Function_decl&
Builder::make_function_declaration(Name& n, Type t, Decl_list const& p)
{
  return function_decl(n, t, p, make_empty_definition());
}


// Create a new function with an empty definition.
Function_decl&
Builder::make_function_declaration(Name& n, Type t, Decl_list&& p)
{
  return make_function_declaration(n, t, std::move(p), make_empty_definition());
}


// Create a new function. 
Function_decl&
Builder::make_function_declaration(Name& n, Type t, Decl_list&& p, Def& d)
{
  return function_decl(n, t, std::move(p), d);
}


// Create a new method with an empty definition. The type is synthesized 
// from the parameter and return types.
Method_decl&
Builder::make_method_declaration(Name& n, Type t, Decl_list const& p)
{
  Type r = get_function_type(p, t);
  Def& d = make_empty_definition();
  return make<Method_decl>(n, r, p, d)
}



// Create a function definition for the given statement.
Function_def&
Builder::make_function_definition(Stmt& s)
{
  return make<Function_def>(s);
}


// Create an expression definition.
Expression_def&
Builder::make_function_definition(Expr& e)
{
  return make<Expression_def>(e);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Nullary_fn f)
{
  return make<Intrinsic_def>(f);
}


// Returns a function definition that evaluates the given function.
Intrinsic_def&
Builder::make_function_definition(Unary_fn f)
{
  return make<Intrinsic_def>(f);
}


// Make a class declaration with an empty definition. 
Class_decl&
Builder::make_class_declaration(Name& n, Type t)
{
  return make_class_declaration(n, t, make_empty_definition());
}


// Create a class having the given definition.
Class_decl&
Builder::make_class_declaration(Name& n, Type t, Def& d)
{
  return make<Class_decl>(n, t, d);
}


Class_def&
Builder::make_class_definition(Stmt_list&& s)
{
  return make<Class_def>(std::move(s));
}


Coroutine_decl&
Builder::make_coroutine_declaration(Name&n, Decl_list&p, Type t, Stmt& s)
{
  lingo_unimplemented("coroutine");
}



Template_decl&
Builder::make_template(Decl_list const& p, Decl& d)
{
  return make<Template_decl>(p, d);
}


Concept_decl&
Builder::make_concept(Name& n, Decl_list const& ps)
{
  return make<Concept_decl>(n, ps);
}


Concept_decl&
Builder::make_concept(Name& n, Decl_list const& ps, Def& d)
{
  return make<Concept_decl>(n, ps, d);
}


Concept_decl&
Builder::make_concept(Name& n, Decl_list const& ps, Expr& e)
{
  return make<Concept_decl>(n, ps, make_expression_definition(e));
}


Concept_decl&
Builder::make_concept(char const* s, Decl_list const& ps, Def& d)
{
  return make_concept(get_id(s), ps, d);
}


Concept_decl&
Builder::make_concept(char const* s, Decl_list const& ps, Expr& e)
{
  return make_concept(get_id(s), ps, make_expression_definition(e));
}


// TODO: Parameters can't be functions or void. Check this
// property or assert it.
Object_parm&
Builder::make_object_parm(Name& n, Type t)
{
  return make<Object_parm>(n, t);
}


Object_parm&
Builder::make_object_parm(char const* s, Type t)
{
  return make_object_parm(get_id(s), t);
}


Value_parm&
Builder::make_value_parm(Name& n, Type t)
{
  return make<Value_parm>(Index {}, n, t);
}


Value_parm&
Builder::make_value_parm(char const* s, Type t)
{
  return make_value_parm(get_id(s), t);
}


Type_parm&
Builder::make_type_parameter(Name& n)
{
  return make<Type_parm>(Index {}, n);
}


Type_parm&
Builder::make_type_parameter(char const* n)
{
  return make_type_parameter(get_id(n));
}


// Make a type parameter with a default type.
Type_parm&
Builder::make_type_parameter(Name& n, Type t)
{
  return make<Type_parm>(Index {}, n, t);
}


// Make a type parameter with a default type.
Type_parm&
Builder::make_type_parameter(char const* n, Type t)
{
  return make_type_parameter(get_id(n), t);
}


// -------------------------------------------------------------------------- //
// Definitions

Empty_def&
Builder::make_empty_definition()
{
  static Empty_def d;
  return d;
}

Deleted_def&
Builder::make_deleted_definition()
{
  static Deleted_def d;
  return d;
}


Defaulted_def&
Builder::make_defaulted_definition()
{
  static Defaulted_def d;
  return d;
}


Expression_def&
Builder::make_expression_definition(Expr& e)
{
  return make<Expression_def>(e);
}


Function_def&
Builder::make_coroutine_definition(Stmt& s)
{
  return make<Function_def>(s);
}

Concept_def&
Builder::make_concept_definition(Req_list const& ss)
{
  return make<Concept_def>(ss);
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
