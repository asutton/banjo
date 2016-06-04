// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Builder definition

Symbol_table&
Builder::symbols() { return cxt.symbols(); }


// -------------------------------------------------------------------------- //
// Names

// Returns a simple identifier with the given spelling.
//
// TODO: Unique this?
Simple_id&
Builder::get_id(char const* s)
{
  Symbol const* sym = symbols().put_identifier(Token_kind::identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a simple identifier with the given spelling.
Simple_id&
Builder::get_id(std::string const& s)
{
  Symbol const* sym = symbols().put_identifier(Token_kind::identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a simple identifier for the given symbol.
Simple_id&
Builder::get_id(Symbol const& sym)
{
  lingo_assert(is<Identifier_sym>(&sym));
  return make<Simple_id>(sym);
}


// Returns a simple identifier for the symbol.
Simple_id&
Builder::get_id(Symbol const* sym)
{
  return get_id(*sym);
}


// Returns a simple identifier for the symbol.
Simple_id&
Builder::get_id(Token const& tok)
{
  return get_id(tok.symbol());
}


// Returns a placeholder for a name.
//
// TODO: Make placeholders unique. Globally?
Placeholder_id&
Builder::get_id()
{
  return make<Placeholder_id>(cxt.get_unique_id());
}


Operator_id&
Builder::get_id(Operator_kind k)
{
  return make<Operator_id>(k);
}


// Returns a destructor-id for the given type.
Destructor_id&
Builder::get_destructor_id(Type const& t)
{
  lingo_unimplemented("destructor-id");
}


Template_id&
Builder::get_template_id(Template_decl& d, Term_list const& t)
{
  return make<Template_id>(d, t);
}


Concept_id&
Builder::get_concept_id(Concept_decl& d, Term_list const& t)
{
  return make<Concept_id>(d, t);
}


// Returns a qualified-id.
Qualified_id&
Builder::get_qualified_id(Decl& d, Name& n)
{
  return make<Qualified_id>(d, n);
}


// Return the global identifier.
Global_id&
Builder::get_global_id()
{
  // TODO: Global or no?
  static Global_id n;
  return n;
}




// -------------------------------------------------------------------------- //
// Expressions



// -------------------------------------------------------------------------- //
// Statements

Empty_stmt&
Builder::make_empty_statement()
{
  return make<Empty_stmt>();
}


Compound_stmt&
Builder::make_compound_statement()
{
  return make<Compound_stmt>();
}


Compound_stmt&
Builder::make_compound_statement(Stmt_list&& ss)
{
  return make<Compound_stmt>(std::move(ss));
}


Return_stmt&
Builder::make_return_statement(Expr& e)
{
  return make<Return_stmt>(e);
}

Yield_stmt&
Builder::make_yield_statement(Expr& e)
{
  return make<Yield_stmt>(e);
}

If_then_stmt&
Builder::make_if_statement(Expr& e, Stmt& s)
{
  return make<If_then_stmt>(e, s);
}


If_else_stmt&
Builder::make_if_statement(Expr& e, Stmt& s1, Stmt& s2)
{
  return make<If_else_stmt>(e, s1, s2);
}


While_stmt&
Builder::make_while_statement(Expr& e, Stmt& s)
{
  return make<While_stmt>(e, s);
}


Break_stmt&
Builder::make_break_statement()
{
  return make<Break_stmt>();
}


Continue_stmt&
Builder::make_continue_statement()
{
  return make<Continue_stmt>();
}


Expression_stmt&
Builder::make_expression_statement(Expr& e)
{
  return make<Expression_stmt>(e);
}


Declaration_stmt&
Builder::make_declaration_statement(Decl& d)
{
  return make<Declaration_stmt>(d);
}


// -------------------------------------------------------------------------- //
// Initializers

Trivial_init&
Builder::make_trivial_init()
{
  return make<Trivial_init>(get_void_type());
}


Copy_init&
Builder::make_copy_init(Expr& e)
{
  return make<Copy_init>(get_void_type(), e);
}


Bind_init&
Builder::make_bind_init(Expr& e)
{
  return make<Bind_init>(get_void_type(), e);
}


Direct_init&
Builder::make_direct_init(Decl& d, Expr_list const& es)
{
  return make<Direct_init>(get_void_type(), d, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list const& es)
{
  return make<Aggregate_init>(t, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list&& es)
{
  return make<Aggregate_init>(t, std::move(es));
}


// -------------------------------------------------------------------------- //
// Declarations

Super_decl&
Builder::make_super_declaration(Type& t)
{
  Def& d = make_empty_definition();
  return make<Super_decl>(get_id(), t, d);
}

Variable_decl&
Builder::make_variable_declaration(Name& n, Type& t)
{
  Def& d = make_empty_definition();
  return make<Variable_decl>(n, t, d);
}


Variable_decl&
Builder::make_variable_declaration(Name& n, Type& t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return make<Variable_decl>(n, t, d);
}


Variable_decl&
Builder::make_variable_declaration(char const* s, Type& t, Expr& i)
{
  return make_variable_declaration(get_id(s), t, i);
}


// Create a constant declaration with an empty definition.
Constant_decl&
Builder::make_constant_declaration(Name& n, Type& t)
{
  Def& d = make_empty_definition();
  return make<Constant_decl>(n, t, d);
}


// Create a constant declaration.
Constant_decl&
Builder::make_constant_declaration(Name& n, Type& t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return make<Constant_decl>(n, t, d);
}


// Create a constant declaration with the name s.
Constant_decl&
Builder::make_constant_declaration(char const* s, Type& t, Expr& i)
{
  return make_constant_declaration(get_id(s), t, i);
}


// Create a new function with an empty definition. The type is synthesized 
// from the parameter and return types.
Function_decl&
Builder::make_function_declaration(Name& n, Decl_list const& p, Type& t)
{
  Type& r = get_function_type(p, t);
  Def& d = make_empty_definition();
  return make<Function_decl>(n, r, p, d);
}


// Create a new function with an empty definition. The type is synthesized 
// from the parameter and return types.
Function_decl&
Builder::make_function_declaration(Name& n, Decl_list&& p, Type& t)
{
  Type& r = get_function_type(p, t);
  Def& d = make_empty_definition();
  return make<Function_decl>(n, r, std::move(p), d);
}


// Create a new function. The type is synthesized from the parameter 
// and return types.
Function_decl&
Builder::make_function_declaration(Name& n, Decl_list&& p, Type& t, Def& d)
{
  Type& r = get_function_type(p, t);
  return make<Function_decl>(n, r, std::move(p), d);
}


// Create a new macro. The type is synthesized from the parameter 
// and return types.
Macro_decl&
Builder::make_macro_declaration(Name& n, Decl_list&& p, Type& t, Def& d)
{
  Type& r = get_function_type(p, t);
  return make<Macro_decl>(n, r, std::move(p), d);
}


// Create a new method with an empty definition. The type is synthesized 
// from the parameter and return types.
Method_decl&
Builder::make_method_declaration(Name& n, Decl_list const& p, Type& t)
{
  Type& r = get_function_type(p, t);
  Def& d = make_empty_definition();
  return make<Method_decl>(n, r, p, d);
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
Builder::make_class_declaration(Name& n, Type& t)
{
  return make_class_declaration(n, t, make_empty_definition());
}


// Create a class having the given definition.
Class_decl&
Builder::make_class_declaration(Name& n, Type& t, Def& d)
{
  return make<Class_decl>(n, t, d);
}


Class_def&
Builder::make_class_definition(Stmt_list&& s)
{
  return make<Class_def>(std::move(s));
}


Coroutine_decl&
Builder::make_coroutine_declaration(Name&n, Decl_list&p, Type& t, Stmt& s)
{
  lingo_unimplemented("coroutine");
}


Field_decl&
Builder::make_field_declaration(Name& n, Type& t)
{
  Def& d = make_empty_definition();
  return make<Field_decl>(n, t, d);
}


Field_decl&
Builder::make_field_declaration(Name& n, Type& t, Expr& e)
{
  Def& d = make_expression_definition(e);
  return make<Field_decl>(n, t, d);
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
Builder::make_object_parm(Name& n, Type& t)
{
  return make<Object_parm>(n, t);
}


Object_parm&
Builder::make_object_parm(char const* s, Type& t)
{
  return make_object_parm(get_id(s), t);
}


Value_parm&
Builder::make_value_parm(Name& n, Type& t)
{
  return make<Value_parm>(Index {}, n, t);
}


Value_parm&
Builder::make_value_parm(char const* s, Type& t)
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
Builder::make_type_parameter(Name& n, Type& t)
{
  return make<Type_parm>(Index {}, n, t);
}


// Make a type parameter with a default type.
Type_parm&
Builder::make_type_parameter(char const* n, Type& t)
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
// Requirements

Basic_req&
Builder::make_basic_requirement(Expr& e, Type& t)
{
  return make<Basic_req>(e, t);
}


Conversion_req&
Builder::make_conversion_requirement(Expr& e, Type& t)
{
  return make<Conversion_req>(e, t);
}


Syntactic_req&
Builder::make_syntactic_requirement(Expr& e)
{
  return make<Syntactic_req>(e);
}


// -------------------------------------------------------------------------- //
// Statements

Translation_unit&
Builder::make_translation_unit()
{
  return make<Translation_unit>();
}


Translation_unit&
Builder::make_translation_unit(Stmt_list&& ss)
{
  return make<Translation_unit>(std::move(ss));
}


// -------------------------------------------------------------------------- //
// Constraints

// FIXME: Save all uniqued terms in the context, not as global variables.

Concept_cons&
Builder::get_concept_constraint(Decl& d, Term_list const& ts)
{
  static Factory<Concept_cons> f;
  return f.make(d, ts);
}


Predicate_cons&
Builder::get_predicate_constraint(Expr& e)
{
  static Factory<Predicate_cons> f;
  return f.make(e);
}


Expression_cons&
Builder::get_expression_constraint(Expr& e, Type& t)
{
  static Factory<Expression_cons> f;
  return f.make(e, t);
}


Conversion_cons&
Builder::get_conversion_constraint(Expr& e, Type& t)
{
  static Factory<Conversion_cons> f;
  return f.make(e, t);

}


Parameterized_cons&
Builder::get_parameterized_constraint(Decl_list const& ds, Cons& c)
{
  static Factory<Parameterized_cons> f;
  return f.make(ds, c);
}


Conjunction_cons&
Builder::get_conjunction_constraint(Cons& c1, Cons& c2)
{
  static Factory<Conjunction_cons> f;
  return f.make(c1, c2);
}


Disjunction_cons&
Builder::get_disjunction_constraint(Cons& c1, Cons& c2)
{
  static Factory<Disjunction_cons> f;
  return f.make(c1, c2);
}


}
