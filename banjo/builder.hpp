// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILDER_HPP
#define BANJO_BUILDER_HPP

// The builder module defines an interface for constructing core language
// terms. The builder manages the memory for every object (except symbols)
// constructed through its methods.

#include "build-type.hpp"
#include "build-expr.hpp"

#include <lingo/token.hpp>


namespace banjo
{


// An interface to an AST builder.
//
// This class uses the PIMPL idiom to define "sub-builders" for each kind
// of term. The purpose is to keep the AST header out of the include path
// for the Context. I wish there were a better way to do this.
struct Builder : Type_builder, Expr_builder
{
  Builder(Context& c)
    : Type_builder(c)
    , Expr_builder(c)
    , cxt(c)
  { }

  // Names
  Simple_id&      get_id(char const*);
  Simple_id&      get_id(std::string const&);
  Simple_id&      get_id(Symbol const&);
  Simple_id&      get_id(Symbol const*);
  Simple_id&      get_id(Token const&);
  Placeholder_id& get_id();
  Operator_id&    get_id(Operator_kind);
  // Conversion_id&  get_id();
  // Literal_id&     get_id();
  Destructor_id&  get_destructor_id(Type const&);
  Template_id&    get_template_id(Template_decl&, Term_list const&);
  Concept_id&     get_concept_id(Concept_decl&, Term_list const&);
  Qualified_id&   get_qualified_id(Decl&, Name&);
  Global_id&      get_global_id();


  // Statements

  // Variables and constants
  Variable_decl&  make_variable_declaration(Name&, Type&);
  Variable_decl&  make_variable_declaration(Name&, Type&, Expr&);
  Variable_decl&  make_variable_declaration(char const*, Type&, Expr&);
  Constant_decl&  make_constant_declaration(Name&, Type&);
  Constant_decl&  make_constant_declaration(Name&, Type&, Expr&);
  Constant_decl&  make_constant_declaration(char const*, Type&, Expr&);

  // Functions, macros, methods and coroutines
  Function_decl&  make_function_declaration(Name&, Decl_list const&, Type&);
  Function_decl&  make_function_declaration(Name&, Decl_list&&, Type&);
  Function_decl&  make_function_declaration(Name&, Decl_list&&, Type&, Def&);
  
  Macro_decl&     make_macro_declaration(Name&, Decl_list&&, Type&, Def&);
  
  Method_decl&    make_method_declaration(Name&, Decl_list const&, Type&);
  
  Coroutine_decl& make_coroutine_declaration(Name&, Decl_list&, Type&, Stmt&);
  
  Function_def&   make_function_definition(Stmt&);
  Expression_def& make_function_definition(Expr&);
  Intrinsic_def&  make_function_definition(Nullary_fn);
  Intrinsic_def&  make_function_definition(Unary_fn);

  // Classes
  Class_decl&  make_class_declaration(Name&, Type&);
  Class_decl&  make_class_declaration(Name&, Type&, Def&);
  Class_def&   make_class_definition(Stmt_list&&);
  Field_decl&  make_field_declaration(Name&, Type&);
  Field_decl&  make_field_declaration(Name&, Type&, Expr&);
  Super_decl&  make_super_declaration(Type&);

  // Templates
  Template_decl&  make_template(Decl_list const&, Decl&);

  // Concepts
  Concept_decl&   make_concept(Name&, Decl_list const&);
  Concept_decl&   make_concept(Name&, Decl_list const&, Def&);
  Concept_decl&   make_concept(Name&, Decl_list const&, Expr&);
  Concept_decl&   make_concept(char const*, Decl_list const&, Def&);
  Concept_decl&   make_concept(char const*, Decl_list const&, Expr&);

  // Initializers
  Trivial_init&   make_trivial_init();
  Copy_init&      make_copy_init(Expr&);
  Bind_init&      make_bind_init(Expr&);
  Direct_init&    make_direct_init(Decl&, Expr_list const&);
  Aggregate_init& make_aggregate_init(Type&, Expr_list const&);
  Aggregate_init& make_aggregate_init(Type&, Expr_list&&);

  // Definitions
  Empty_def&      make_empty_definition();
  Deleted_def&    make_deleted_definition();
  Defaulted_def&  make_defaulted_definition();
  Expression_def& make_expression_definition(Expr&);
  Function_def&   make_coroutine_definition(Stmt&);

  // Concepts
  Concept_def&    make_concept_definition(Req_list const&);

  // Parameters
  Object_parm& make_object_parm(Name&, Type&);
  Object_parm& make_object_parm(char const*, Type&);
  Value_parm&  make_value_parm(Name&, Type&);
  Value_parm&  make_value_parm(char const*, Type&);
  Type_parm&   make_type_parameter(Name&);
  Type_parm&   make_type_parameter(char const*);
  Type_parm&   make_type_parameter(Name&, Type&);
  Type_parm&   make_type_parameter(char const*, Type&);

  // Requirements
  Basic_req&      make_basic_requirement(Expr&, Type&);
  Conversion_req& make_conversion_requirement(Expr&, Type&);
  Syntactic_req&  make_syntactic_requirement(Expr&);

  // Toplevel structures
  Translation_unit& make_translation_unit();
  Translation_unit& make_translation_unit(Stmt_list&&);


  // Constraints
  // Note that constraints are canonicalized in order
  // ensure efficient hashing and equivalence comparison.
  Concept_cons&       get_concept_constraint(Decl&, Term_list const&);
  Predicate_cons&     get_predicate_constraint(Expr&);
  Expression_cons&    get_expression_constraint(Expr&, Type&);
  Conversion_cons&    get_conversion_constraint(Expr&, Type&);
  Parameterized_cons& get_parameterized_constraint(Decl_list const&, Cons&);
  Conjunction_cons&   get_conjunction_constraint(Cons&, Cons&);
  Disjunction_cons&   get_disjunction_constraint(Cons&, Cons&);

  // Resources
  Symbol_table& symbols();

  // Allocate an objet of the given type.
  //
  // TODO: This is a placeholder for using a legitimate object
  // pool in the context (or somewhere else).
  template<typename T, typename... Args>
  T& make(Args&&... args)
  {
    return *new T(std::forward<Args>(args)...);
  }

  Context& cxt;
};


} // namespace banjo


#endif
