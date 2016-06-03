// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILDER_HPP
#define BANJO_BUILDER_HPP

// The builder module defines an interface for constructing core language
// terms. The builder manages the memory for every object (except symbols)
// constructed through its methods.

#error fuck

#include "build-type.hpp"

#include <lingo/token.hpp>


namespace banjo
{

struct Type_builder;


// An interface to an AST builder.
//
// This class uses the PIMPL idiom to define "sub-builders" for each kind
// of term. The purpose is to keep the AST header out of the include path
// for the Context. I wish there were a better way to do this.
struct Builder : Type_builder
{
  Builder(Context& c)
    : Type_builder(c), cxt(c)
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



  // Literals
  Void_expr&     get_void();
  Boolean_expr&  get_bool(bool);
  Boolean_expr&  get_true();
  Boolean_expr&  get_false();
  Integer_expr&  get_integer(Type&, Integer const&);
  Integer_expr&  get_zero(Type&);
  Integer_expr&  get_int(Integer const&);
  Integer_expr&  get_uint(Integer const&);
  
  // Aggregates
  Tuple_expr&    make_tuple(Type&, Expr_list&&);

  // References
  Object_expr&   make_reference(Type&, Object_decl&);
  Value_expr&    make_reference(Type&, Value_decl&);
  Function_expr& make_reference(Type&, Function_decl&);
  Overload_expr& make_reference(Name&, Decl_list&&);
  Field_expr&    make_reference(Type&, Expr&, Field_decl&);
  Method_expr&   make_reference(Type&, Expr&, Method_decl&);
  Member_expr&   make_reference(Expr&, Name&, Decl_list&&);

  // Logical expressions
  And_expr&       make_and(Type&, Expr&, Expr&);
  Or_expr&        make_or(Type&, Expr&, Expr&);
  Not_expr&       make_not(Type&, Expr&);

  // Relational expressions
  Eq_expr&        make_eq(Type&, Expr&, Expr&);
  Ne_expr&        make_ne(Type&, Expr&, Expr&);
  Lt_expr&        make_lt(Type&, Expr&, Expr&);
  Gt_expr&        make_gt(Type&, Expr&, Expr&);
  Le_expr&        make_le(Type&, Expr&, Expr&);
  Ge_expr&        make_ge(Type&, Expr&, Expr&);

  // Arithmetic expressions
  Add_expr&       make_add(Type&, Expr&, Expr&);
  Sub_expr&       make_sub(Type&, Expr&, Expr&);
  Mul_expr&       make_mul(Type&, Expr&, Expr&);
  Div_expr&       make_div(Type&, Expr&, Expr&);
  Rem_expr&       make_rem(Type&, Expr&, Expr&);
  Neg_expr&       make_neg(Type&, Expr&);
  Pos_expr&       make_pos(Type&, Expr&);

  // Bitwise expressions
  Bit_and_expr&   make_bit_and(Type&, Expr&, Expr&);
  Bit_or_expr&    make_bit_or(Type&, Expr&, Expr&);
  Bit_xor_expr&   make_bit_xor(Type&, Expr&, Expr&);
  Bit_lsh_expr&   make_bit_lsh(Type&, Expr&, Expr&);
  Bit_rsh_expr&   make_bit_rsh(Type&, Expr&, Expr&);
  Bit_not_expr&   make_bit_not(Type&, Expr&);

  // Function call
  Call_expr& make_call(Type&, Expr&, Expr_list const&);

  // Misc.
  Requires_expr&  make_requires(Decl_list const&, Decl_list const&, Req_list const&);
  Synthetic_expr& synthesize_expression(Decl&);

  // Statements
  Empty_stmt&       make_empty_statement();
  Compound_stmt&    make_compound_statement();
  Compound_stmt&    make_compound_statement(Stmt_list&&);
  Return_stmt&      make_return_statement(Expr&);
  Yield_stmt&       make_yield_statement(Expr&);
  If_then_stmt&     make_if_statement(Expr&, Stmt&);
  If_else_stmt&     make_if_statement(Expr&, Stmt&, Stmt&);
  While_stmt&       make_while_statement(Expr&, Stmt&);
  Break_stmt&       make_break_statement();
  Continue_stmt&    make_continue_statement();
  Expression_stmt&  make_expression_statement(Expr&);
  Declaration_stmt& make_declaration_statement(Decl&);

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
