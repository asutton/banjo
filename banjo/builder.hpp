// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILDER_HPP
#define BANJO_BUILDER_HPP

// The builder module defines an interface for constructing core language
// terms. The builder manages the memory for every object (except symbols)
// constructed through its methods.

#include "token.hpp"
#include "hashing.hpp"

#include <forward_list>
#include <unordered_set>


namespace banjo
{


// An interface to an AST builder. This provides simplified procedures
// for creating AST nodes. Note that the nodes are not checked for correctness
// at the point of creation. Use context facilities for creating semantically
// correct terms.
//
struct Builder
{
  Builder(Allocator& a, Symbol_table& s)
    : alloc_(a), syms_(s)
  { }

  // Names

  Symbol const& get_identifier(char const* s);

  Simple_id&      get_id(char const*);
  Simple_id&      get_id(std::string const&);
  Simple_id&      get_id(Symbol const&);
  Simple_id&      get_id(Token const&);
  Placeholder_id& get_id();
  Operator_id&    get_id(Operator_kind);
  
  Conversion_id&  get_conversion_id(Type&);
  
  // Literal_id&     get_id();
  // Destructor_id&  get_destructor_id(Type const&);
  // Template_id&    get_template_id(Template_decl&, Term_list const&);
  // Concept_id&     get_concept_id(Concept_decl&, Term_list const&);
  // Qualified_id&   get_qualified_id(Decl&, Name&);
  // Global_id&      get_global_id();

  // Types

  // Fundamental types
  Void_type&     get_void_type(Qualifier_set = {});
  Boolean_type&  get_bool_type(Type_category = object_type, Qualifier_set = {});
  Byte_type&     get_byte_type(Type_category = object_type, Qualifier_set = {});
  Integer_type&  get_integer_type(Type_category, bool, int, Qualifier_set = {});
  Integer_type&  get_integer_type(bool, int, Qualifier_set = {});
  Integer_type&  get_int_type(Type_category = object_type, Qualifier_set = {});
  Integer_type&  get_uint_type(Type_category = object_type, Qualifier_set = {});
  Float_type&    get_float_type(Type_category, int, Qualifier_set = {});
  Float_type&    get_float_type(int, Qualifier_set = {});

  // Composite types
  Function_type& get_function_type(Type_category, Type_list const&, Type&, Qualifier_set = {});
  Function_type& get_function_type(Type_category, Type_list&&, Type&, Qualifier_set = {});
  Function_type& get_function_type(Type_list const&, Type&, Qualifier_set = {});
  Function_type& get_function_type(Type_list&&, Type&, Qualifier_set = {});
  Array_type& get_array_type(Type_category, Type&, Expr&);
  Tuple_type& get_tuple_type(Type_category, Type_list const&);
  Tuple_type& get_tuple_type(Type_category, Type_list&&);
  Pointer_type& get_pointer_type(Type_category, Type&, Qualifier_set = {});

  // User-defined types
  Class_type&    get_class_type(Type_category, Type_decl&, Qualifier_set = {});
  Typename_type& get_typename_type(Type_category, Type_decl&, Qualifier_set = {});
  
  // Placeholder types
  Auto_type&     get_auto_type(Type_category, Type_decl&, Qualifier_set = {});
  Decltype_type& get_decltype_type(Type_category, Expr&, Qualifier_set = {});

  // Reference and object types
  Type& get_reference_type(Type& t);
  Type& get_non_reference_type(Type& t);
  
  // Qualified types
  Type& get_qualified_type(Type&, Qualifier_set);
  Integer_type& get_qualified_integer_type(Integer_type&, Qualifier_set);
  Float_type&   get_qualified_float_type(Float_type&, Qualifier_set);
  Array_type&   get_qualified_array_type(Array_type&, Qualifier_set);
  Tuple_expr&   get_qualified_tuple_type(Tuple_type&, Qualifier_set);
  Pointer_type& get_qualified_pointer_type(Pointer_type&, Qualifier_set);
  Class_type&   get_qualified_pointer_type(Class_type&, Qualifier_set);

  // Expressions

  // Literals
  Void_expr&     get_void();
  Void_expr&     get_void(Type&);
  Boolean_expr&  get_boolean(Type&, bool);
  Boolean_expr&  get_true(Type&);
  Boolean_expr&  get_false(Type&);
  Integer_expr&  get_integer(Type&, Integer const&);
  Integer_expr&  get_zero(Type&);
  Integer_expr&  get_int(Type&, Integer const&);
  Integer_expr&  get_uint(Type&, Integer const&);
  
  // Aggregates
  Tuple_expr&    make_tuple(Type&, Expr_list const&);
  Tuple_expr&    make_tuple(Type&, Expr_list&&);

  // Declaration references
  Decl_ref&     make_reference(Type&, Variable_decl&);
  Decl_ref&     make_reference(Type&, Function_decl&);
  Overload_ref& make_reference(Name&, Decl_list const&);
  Overload_ref& make_reference(Name&, Decl_list&&);
  Member_ref&   make_reference(Type&, Expr&, Field_decl&);
  Member_ref&   make_reference(Type&, Expr&, Method_decl&);
  Scoped_ref&   make_reference(Expr&, Name&, Decl_list const&);
  Scoped_ref&   make_reference(Expr&, Name&, Decl_list&&);

  // Logical expressions
  And_expr& make_and(Type&, Expr&, Expr&);
  Or_expr&  make_or(Type&, Expr&, Expr&);
  Not_expr& make_not(Type&, Expr&);

  // Relational expressions
  Eq_expr& make_eq(Type&, Expr&, Expr&);
  Ne_expr& make_ne(Type&, Expr&, Expr&);
  Lt_expr& make_lt(Type&, Expr&, Expr&);
  Gt_expr& make_gt(Type&, Expr&, Expr&);
  Le_expr& make_le(Type&, Expr&, Expr&);
  Ge_expr& make_ge(Type&, Expr&, Expr&);

  // Arithmetic expressions
  Add_expr& make_add(Type&, Expr&, Expr&);
  Sub_expr& make_sub(Type&, Expr&, Expr&);
  Mul_expr& make_mul(Type&, Expr&, Expr&);
  Div_expr& make_div(Type&, Expr&, Expr&);
  Rem_expr& make_rem(Type&, Expr&, Expr&);
  Neg_expr& make_neg(Type&, Expr&);
  Pos_expr& make_pos(Type&, Expr&);

  // Bitwise expressions
  Bit_and_expr& make_bit_and(Type&, Expr&, Expr&);
  Bit_or_expr&  make_bit_or(Type&, Expr&, Expr&);
  Bit_xor_expr& make_bit_xor(Type&, Expr&, Expr&);
  Bit_lsh_expr& make_bit_lsh(Type&, Expr&, Expr&);
  Bit_rsh_expr& make_bit_rsh(Type&, Expr&, Expr&);
  Bit_not_expr& make_bit_not(Type&, Expr&);

  // Function call
  Call_expr& make_call(Type&, Expr&, Expr_list const&);
  Call_expr& make_call(Type&, Expr&, Expr_list&&);

  // Initializers
  Trivial_init&   make_trivial_init();
  Copy_init&      make_copy_init(Expr&);
  Bind_init&      make_bind_init(Expr&);
  Direct_init&    make_direct_init(Decl&, Expr_list const&);
  Aggregate_init& make_aggregate_init(Type&, Expr_list const&);
  Aggregate_init& make_aggregate_init(Type&, Expr_list&&);

  // Statements
  Empty_stmt&        make_empty_statement();
  Compound_stmt&     make_compound_statement();
  Compound_stmt&     make_compound_statement(Stmt_list const&);
  Compound_stmt&     make_compound_statement(Stmt_list&&);
  Return_stmt&       make_return_statement();
  Return_value_stmt& make_return_statement(Expr&);
  Yield_stmt&        make_yield_statement();
  Yield_value_stmt&  make_yield_statement(Expr&);
  If_then_stmt&      make_if_statement(Expr&, Stmt&);
  If_else_stmt&      make_if_statement(Expr&, Stmt&, Stmt&);
  While_stmt&        make_while_statement(Expr&, Stmt&);
  Break_stmt&        make_break_statement();
  Continue_stmt&     make_continue_statement();
  Expression_stmt&   make_expression_statement(Expr&);
  Declaration_stmt&  make_declaration_statement(Decl&);

  // Declarations

  // Variables
  Variable_decl&  make_variable_declaration(Name&, Type&, Def&);
  Variable_decl&  make_variable_declaration(char const*, Type&, Def&);
  Variable_decl&  make_variable_declaration(Name&, Type&);
  Variable_decl&  make_variable_declaration(char const*, Type&);
  Empty_def&      make_variable_initializer();
  Expression_def& make_variable_initializer(Expr& e);

  // Functions
  Function_decl&  make_function_declaration(Name&, Type&, Decl_list const&, Def&);
  Function_decl&  make_function_declaration(Name&, Type&, Decl_list&&, Def&);
  Function_decl&  make_function_declaration(Name&, Type&, Decl_list const&);
  Function_decl&  make_function_declaration(Name&, Type&, Decl_list&&);
  Function_def&   make_function_definition(Stmt&);
  Expression_def& make_function_definition(Expr&);
  Intrinsic_def&  make_function_definition(Nullary_fn);
  Intrinsic_def&  make_function_definition(Unary_fn);
  Intrinsic_def&  make_function_definition(Binary_fn);
  Intrinsic_def&  make_function_definition(Ternary_fn);

  // Classes and their members
  Class_decl&  make_class_declaration(Name&, Def&);
  Class_decl&  make_class_declaration(Name&);
  Class_def&   make_class_definition(Stmt_list&&);
  Field_decl&  make_field_declaration(Name&, Type&, Def&);
  Field_decl&  make_field_declaration(char const*, Type&, Def&);
  Field_decl&  make_field_declaration(Name&, Type&);
  Field_decl&  make_field_declaration(char const*, Type&);
  Method_decl& make_method_declaration(Name&, Type&, Decl_list const&, Def&);
  Method_decl& make_method_declaration(Name&, Type&, Decl_list&&, Def&);
  Method_decl& make_method_declaration(Name&, Type&, Decl_list const&);
  Method_decl& make_method_declaration(Name&, Type&, Decl_list&&);

  // Parameters
  Variable_parm&  make_variable_parameter(Name&, Type&);
  Variable_parm&  make_variable_parameter(char const*, Type&);
  Type_parm&      make_type_parameter(Name&);
  Type_parm&      make_type_parameter(char const*);
  
  // General purpose definitions
  Empty_def&      make_empty_definition();
  Deleted_def&    make_deleted_definition();
  Defaulted_def&  make_defaulted_definition();
  Expression_def& make_expression_definition(Expr&);

  // Modular structure
  Translation_unit& make_translation_unit();
  Translation_unit& make_translation_unit(Stmt_list&&);

  // Resources
  Allocator& allocator() { return alloc_; }
  Symbol_table& symbols() { return syms_; }

  // Facilities
  Allocator&    alloc_;
  Symbol_table& syms_;
};


} // namespace banjo


#endif
