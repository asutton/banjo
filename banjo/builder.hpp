
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILDER_HPP
#define BANJO_BUILDER_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "token.hpp"
#include "ast_base.hpp"
#include "qualifier.hpp"
#include "specifier.hpp"

#include <lingo/token.hpp>


namespace banjo
{

// An interface to an AST builder.
//
// TODO: Factor all the checking into a policy class provided
// as a template parameter?
//
// TODO: Make intelligent decisions about canonicalization. If a
// term (e.g., type/constant) is constructed without a source
// location, then it can be uniqued.
struct Builder
{
  Builder(Context& cxt)
    : cxt(cxt)
  { }

  // Names
  //
  // TODO: Implement more of these.
  Simple_id&      get_id(char const*);
  Simple_id&      get_id(std::string const&);
  Simple_id&      get_id(Symbol const&);
  Simple_id&      get_id(Symbol const*);
  Simple_id&      get_id(Token tok);
  Placeholder_id& get_id();
  // Operator_id&    get_id();
  // Conversion_id&  get_id();
  // Literal_id&     get_id();
  Destructor_id&  get_destructor_id(Type const&);
  Template_id&    get_template_id(Template_decl&, Term_list const&);
  Concept_id&     get_concept_id(Concept_decl&, Term_list const&);
  Qualified_id&   get_qualified_id(Decl&, Name&);
  Global_id&      get_global_id();

  // Types
  Void_type&      get_void_type();
  Boolean_type&   get_bool_type();
  Byte_type&      get_byte_type();
  Integer_type&   get_integer_type(bool, int);
  Integer_type&   get_int_type();
  Integer_type&   get_uint_type();
  Float_type&     get_float_type();
  Auto_type&      get_auto_type();
  Decltype_type&  get_decltype_type(Expr&);
  Declauto_type&  get_declauto_type();
  Function_type&  get_function_type(Decl_list const&, Type&);
  Function_type&  get_function_type(Type_list const&, Type&);
  Qualified_type& get_qualified_type(Type&, Qualifier_set);
  Qualified_type& get_const_type(Type&);
  Qualified_type& get_volatile_type(Type&);
  Pointer_type&   get_pointer_type(Type&);
  Reference_type& get_reference_type(Type&);
  Array_type&     get_array_type(Type&, Expr&);
  Sequence_type&  get_sequence_type(Type&);
  Class_type&     get_class_type(Decl&);
  Union_type&     get_union_type(Decl&);
  Enum_type&      get_enum_type(Decl&);
  Typename_type&  get_typename_type(Decl&);
  Synthetic_type& synthesize_type(Decl&);

  // Expressions
  Boolean_expr&   get_bool(bool);
  Boolean_expr&   get_true();
  Boolean_expr&   get_false();
  Integer_expr&   get_integer(Type&, Integer const&);
  Integer_expr&   get_zero(Type&);
  Integer_expr&   get_int(Integer const&);
  Integer_expr&   get_uint(Integer const&);
  Reference_expr& make_reference(Variable_decl&);
  Reference_expr& make_reference(Constant_decl&);
  Reference_expr& make_reference(Function_decl&);
  Reference_expr& make_reference(Object_parm&);
  Check_expr&     make_check(Concept_decl&, Term_list const&);

  And_expr&       make_and(Type&, Expr&, Expr&);
  Or_expr&        make_or(Type&, Expr&, Expr&);
  Not_expr&       make_not(Type&, Expr&);
  Eq_expr&        make_eq(Type&, Expr&, Expr&);
  Ne_expr&        make_ne(Type&, Expr&, Expr&);
  Lt_expr&        make_lt(Type&, Expr&, Expr&);
  Gt_expr&        make_gt(Type&, Expr&, Expr&);
  Le_expr&        make_le(Type&, Expr&, Expr&);
  Ge_expr&        make_ge(Type&, Expr&, Expr&);
  Call_expr&      make_call(Type&, Expr&, Expr_list const&);
  Call_expr&      make_call(Type&, Function_decl&, Expr_list const&);
  Requires_expr&  make_requires(Decl_list const&, Stmt&);
  Synthetic_expr& synthesize_expression(Decl&);

  // Statements
  Compound_stmt&    make_compound_statement(Stmt_list const&);
  Return_stmt&      make_return_statement(Expr&);
  Expression_stmt&  make_expression_statement(Expr&);
  Declaration_stmt& make_declaration_statement(Decl&);

  // Initializers
  Trivial_init&   make_trivial_init(Type&);
  Copy_init&      make_copy_init(Type&, Expr&);
  Bind_init&      make_bind_init(Type&, Expr&);
  Direct_init&    make_direct_init(Type&, Decl&, Expr_list const&);
  Aggregate_init& make_aggregate_init(Type&, Expr_list const&);

  // Definitions
  Deleted_def&    make_deleted_definition();
  Defaulted_def&  make_defaulted_definition();
  Expression_def& make_expression_definition(Expr&);
  Function_def&   make_function_definition(Stmt&);
  Class_def&      make_class_definition(Decl_list const&);
  Concept_def&    make_concept_definition(Req_list const&);

  Namespace_decl& make_namespace(Name&);
  Namespace_decl& make_namespace(char const*);
  Namespace_decl& get_global_namespace();
  Variable_decl&  make_variable(Name&, Type&);
  Variable_decl&  make_variable(char const*, Type&);
  Variable_decl&  make_variable(Name&, Type&, Expr&);
  Variable_decl&  make_variable(char const*, Type&, Expr&);
  Function_decl&  make_function(Name&, Decl_list const&, Type&);
  Function_decl&  make_function(char const*, Decl_list const&, Type&);
  Class_decl&     make_class(Name&);
  Class_decl&     make_class(char const*);
  Template_decl&  make_template(Decl_list const&, Decl&);
  Concept_decl&   make_concept(Name&, Decl_list const&);
  Concept_decl&   make_concept(Name&, Decl_list const&, Def&);
  Concept_decl&   make_concept(Name&, Decl_list const&, Expr&);
  Concept_decl&   make_concept(char const*, Decl_list const&, Def&);
  Concept_decl&   make_concept(char const*, Decl_list const&, Expr&);

  Object_parm& make_object_parm(Name&, Type&);
  Object_parm& make_object_parm(char const*, Type&);
  Value_parm&  make_value_parm(Name&, Type&);
  Value_parm&  make_value_parm(char const*, Type&);
  Type_parm&   make_type_parameter(Name&);
  Type_parm&   make_type_parameter(char const*);
  Type_parm&   make_type_parameter(Name&, Type&);
  Type_parm&   make_type_parameter(char const*, Type&);

  // Constraints.
  // Note that constraints are canonicalized in order
  // ensure efficient hashingn and equivalence comparison.
  Concept_cons&     get_concept_constraint(Decl&, Term_list&);
  Predicate_cons&   get_predicate_constraint(Expr&);
  Conjunction_cons& get_conjunction_constraint(Cons&, Cons&);
  Disjunction_cons& get_disjunction_constraint(Cons&, Cons&);

  // Resources
  Symbol_table& symbols() { return cxt.symbols(); }

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
