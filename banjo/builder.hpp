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

// The singleton factory maintains a single instance of an object.
template<typename T>
struct Singleton_factory
{
  Singleton_factory()
    : obj_()
  { }

  ~Singleton_factory()
  {
    delete obj_;
  }

  template<typename... Args>
  T& operator()(Args&&... args)
  {
    if (!obj_)
      obj_ = new T(std::forward<Args>(args)...);
    return *obj_;
  }

  T* obj_;
};


// A basic object factory allocates new objects by pushing them to a
// list.
//
// TODO: Use a bump allocator instead of a list since we don't really
// need the underlying memory.
template<typename T>
struct Basic_factory : std::forward_list<T>
{
  template<typename... Args>
  T& operator()(Args&&... args) 
  {
    this->emplace_front(std::forward<Args>(args)...);
    return this->front();
  }
};


// The hashed unique factory maintains an unordered set of objects
// that are unique based on their equality. 
template<typename T, typename Hash, typename Eq>
struct Hashed_unique_factory : std::unordered_set<T, Hash, Eq>
{
  template<typename... Args>
  T& operator()(Args&&... args)
  {
    auto ins = this->emplace(std::forward<Args>(args)...);
    return *const_cast<T*>(&*ins.first); // Yuck.
  }
};



// An interface to an AST builder.
//
// Note that the builder owns a symbol table. This can be used by the
// application context to support typical lexing and parsing tasks or 
// to temporarily store unique representations of symbols.
struct Builder
{
  // Hash function for terms.
  using Hash = hash<fnv1a_hash>;

  // Equivalence relation on terms.
  template<typename T>
  struct Eq
  {
    bool operator()(T const& a, T const& b) const
    {
      return is_equivalent(a, b);
    }
  };

  // Factory types.
  template<typename T>
  using Single_factory = banjo::Singleton_factory<T>;

  template<typename T>
  using Basic_factory = banjo::Basic_factory<T>;

  template<typename T>
  using Unique_factory = Hashed_unique_factory<T, Hash, Eq<T>>;

  // Cloning
  Type&     clone(Type&);
  Type_list clone(Type_list&);
  Expr&     clone(Expr&);
  Expr_list clone(Expr_list&);

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
  // Destructor_id&  get_destructor_id(Type const&);
  // Template_id&    get_template_id(Template_decl&, Term_list const&);
  // Concept_id&     get_concept_id(Concept_decl&, Term_list const&);
  // Qualified_id&   get_qualified_id(Decl&, Name&);
  // Global_id&      get_global_id();

  // Types

  // Fundamental types
  Void_type&     get_void_type(Qualifier_set = {});
  Boolean_type&  get_bool_type(Type_category, Qualifier_set = {});
  Byte_type&     get_byte_type(Type_category, Qualifier_set = {});
  Integer_type&  get_integer_type(Type_category, bool, int, Qualifier_set = {});
  Integer_type&  get_integer_type(bool, int, Qualifier_set = {});
  Integer_type&  get_int_type(Type_category, Qualifier_set = {});
  Integer_type&  get_int_type(Qualifier_set = {});
  Integer_type&  get_uint_type(Type_category, Qualifier_set = {});
  Integer_type&  get_uint_type(Qualifier_set = {});
  Float_type&    get_float_type(Type_category, int, Qualifier_set = {});

  // Composite types
  Function_type& get_function_type(Type_category, Decl_list const&, Type&);
  Function_type& get_function_type(Decl_list const&, Type&);
  Function_type& get_function_type(Type_category, Type_list const&, Type&);
  Function_type& get_function_type(Type_list const&, Type&);
  Array_type& get_array_type(Type_category, Type&, Expr&);
  Tuple_type& get_tuple_type(Type_category, Type_list const&);
  Tuple_type& get_tuple_type(Type_category, Type_list&&);
  Pointer_type& get_pointer_type(Type_category, Type&, Qualifier_set = {});

  // User-defined types
  Class_type& get_class_type(Type_category, Type_decl&, Qualifier_set = {});
  Typename_type& get_typename_type(Type_category, Type_decl&, Qualifier_set = {});
  
  // Placeholder types
  Auto_type& get_auto_type(Type_category, Type_decl&, Qualifier_set = {});
  Decltype_type& get_decltype_type(Type_category, Expr&, Qualifier_set = {});

  // Meta-types
  Type_type& get_type_type();

  // Qualified types
  Type& get_reference_type(Type& t, Type_category);
  Type& get_qualified_type(Type&, Qualifier_set);

#if 0
  // Fresh types
  Type make_auto_type();
  Type make_synthetic_type(Decl&);
#endif
  
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
  Tuple_expr&    make_tuple(Type&, Expr_list&&);

  // Resolved references
  Id_object_expr&     make_reference(Type&, Object_decl&);
  Id_function_expr&   make_reference(Type&, Function_decl&);
  Id_overload_expr&   make_reference(Name&, Decl_list&&);
  Dot_object_expr&    make_reference(Type&, Expr&, Field_decl&);
  Dot_function_expr&  make_reference(Type&, Expr&, Method_decl&);
  Dot_overload_expr&  make_reference(Expr&, Name&, Decl_list&&);

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
  Compound_stmt&     make_compound_statement(Stmt_list&&);
  Return_stmt&       make_return_statement();
  Return_value_stmt& make_return_value_statement(Expr&);
  Yield_stmt&        make_yield_statement();
  Yield_value_stmt&  make_yield_value_statement(Expr&);
  If_then_stmt&      make_if_statement(Expr&, Stmt&);
  If_else_stmt&      make_if_statement(Expr&, Stmt&, Stmt&);
  While_stmt&        make_while_statement(Expr&, Stmt&);
  Break_stmt&        make_break_statement();
  Continue_stmt&     make_continue_statement();
  Expression_stmt&   make_expression_statement(Expr&);
  Declaration_stmt&  make_declaration_statement(Decl&);

  // Declarations

  // Objects and references
  Object_decl&    make_variable_declaration(Name&, Type&);
  Object_decl&    make_variable_declaration(Name&, Type&, Expr&);
  Reference_decl& make_reference_declaration(Name&, Type&);
  Reference_decl& make_reference_declaration(Name&, Type&, Expr&);
  Field_decl&     make_field_declaration(Name&, Type&);
  Field_decl&     make_field_declaration(Name&, Type&, Expr&);
  Super_decl&     make_super_declaration(Type&);

  // Functions and methods
  Function_decl&  make_function_declaration(Name&, Type&, Decl_list const&);
  Function_decl&  make_function_declaration(Name&, Type&, Decl_list&&);
  Function_decl&  make_function_declaration(Name&, Type&, Decl_list&&, Def&);
  Function_def&   make_function_definition(Stmt&);
  Method_decl&    make_method_declaration(Name&, Type&, Decl_list const&);
  Method_decl&    make_method_declaration(Name&, Type&, Decl_list&&);
  Method_decl&    make_method_declaration(Name&, Type&, Decl_list&&, Def&);
  Expression_def& make_function_definition(Expr&);
  Intrinsic_def&  make_function_definition(Nullary_fn);
  Intrinsic_def&  make_function_definition(Unary_fn);
  Intrinsic_def&  make_function_definition(Binary_fn);
  Intrinsic_def&  make_function_definition(Ternary_fn);

  // Classes
  Class_decl&  make_class_declaration(Name&, Type&);
  Class_decl&  make_class_declaration(Name&, Type&, Def&);
  Class_def&   make_class_definition(Stmt_list&&);

  // Parameters
  Object_parm&    make_object_parameter(Name&, Type&);
  Object_parm&    make_object_parameter(char const*, Type&);
  Reference_parm& make_reference_parameter(Name&, Type&);
  Reference_parm& make_reference_parameter(char const*, Type&);
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
  Symbol_table& symbols() { return syms_; }

  // Facilities
  Symbol_table syms_;

  // Names
  Basic_factory<Simple_id> simple_id;
  Basic_factory<Placeholder_id> placeholder_id;
  Basic_factory<Operator_id> operator_id;

  // Types
  Basic_factory<Void_type> void_type;
  Basic_factory<Boolean_type> bool_type;
  Basic_factory<Byte_type> byte_type;
  Basic_factory<Integer_type> integer_type;
  Basic_factory<Float_type> float_type;
  Basic_factory<Function_type> function_type;
  Basic_factory<Array_type> array_type;
  Basic_factory<Tuple_type> tuple_type;
  Basic_factory<Pointer_type> pointer_type;
  Basic_factory<Class_type> class_type;
  Basic_factory<Typename_type> typename_type;
  Basic_factory<Auto_type> auto_type;
  Basic_factory<Decltype_type> decltype_type;
  Basic_factory<Type_type> type_type;

  // Expressions
  Single_factory<Void_expr> void_expr;
  Basic_factory<Boolean_expr> bool_expr;
  Basic_factory<Integer_expr> int_expr;
  Basic_factory<Tuple_expr> tuple_expr;
  Basic_factory<Id_object_expr> id_object_expr;
  Basic_factory<Id_function_expr> id_function_expr;
  Basic_factory<Id_overload_expr> id_overload_expr;
  Basic_factory<Dot_object_expr> dot_object_expr;
  Basic_factory<Dot_function_expr> dot_function_expr;
  Basic_factory<Dot_overload_expr> dot_overload_expr;
  Basic_factory<And_expr> and_expr;
  Basic_factory<Or_expr> or_expr;
  Basic_factory<Not_expr> not_expr;
  Basic_factory<Eq_expr> eq_expr;
  Basic_factory<Ne_expr> ne_expr;
  Basic_factory<Lt_expr> lt_expr;
  Basic_factory<Gt_expr> gt_expr;
  Basic_factory<Le_expr> le_expr;
  Basic_factory<Ge_expr> ge_expr;
  Basic_factory<Add_expr> add_expr;
  Basic_factory<Sub_expr> sub_expr;
  Basic_factory<Mul_expr> mul_expr;
  Basic_factory<Div_expr> div_expr;
  Basic_factory<Rem_expr> rem_expr;
  Basic_factory<Neg_expr> neg_expr;
  Basic_factory<Pos_expr> pos_expr;
  Basic_factory<Bit_and_expr> bit_and_expr;
  Basic_factory<Bit_or_expr> bit_or_expr;
  Basic_factory<Bit_xor_expr> bit_xor_expr;
  Basic_factory<Bit_lsh_expr> bit_lsh_expr;
  Basic_factory<Bit_rsh_expr> bit_rsh_expr;
  Basic_factory<Bit_not_expr> bit_not_expr;
  Basic_factory<Call_expr> call_expr;

  // Conversions

  // Initializers
  Basic_factory<Trivial_init> trivial_init;
  Basic_factory<Copy_init> copy_init;
  Basic_factory<Bind_init> bind_init;
  Basic_factory<Direct_init> direct_init;
  Basic_factory<Aggregate_init> aggregate_init;

  // Statements
  Basic_factory<Empty_stmt> empty_stmt;
  Basic_factory<Compound_stmt> compound_stmt;
  Basic_factory<Return_stmt> return_stmt;
  Basic_factory<Return_value_stmt> return_value_stmt;
  Basic_factory<Yield_stmt> yield_stmt;
  Basic_factory<Yield_value_stmt> yield_value_stmt;
  Basic_factory<If_then_stmt> if_then_stmt;
  Basic_factory<If_else_stmt> if_else_stmt;
  Basic_factory<While_stmt> while_stmt;
  Basic_factory<Break_stmt> break_stmt;
  Basic_factory<Continue_stmt> continue_stmt;
  Basic_factory<Expression_stmt> expression_stmt;
  Basic_factory<Declaration_stmt> declaration_stmt;

  // Declarations
  Basic_factory<Object_decl> object_decl;
  Basic_factory<Reference_decl> reference_decl;
  Basic_factory<Field_decl> field_decl;
  Basic_factory<Super_decl> super_decl;
  Basic_factory<Function_decl> function_decl;
  Basic_factory<Method_decl> method_decl;
  Basic_factory<Class_decl> class_decl;

  // Parameters
  Basic_factory<Object_parm> object_parm;
  Basic_factory<Reference_parm> reference_parm;
  Basic_factory<Type_parm> type_parm;

  // Modules
  Single_factory<Translation_unit> translation_unit;
  
  // Definitions
  Basic_factory<Empty_def> empty_def;
  Basic_factory<Deleted_def> deleted_def;
  Basic_factory<Defaulted_def> defaulted_def;
  Basic_factory<Expression_def> expression_def;
  Basic_factory<Function_def> function_def;
  Basic_factory<Class_def> class_def;
  Basic_factory<Intrinsic_def> intrinsic_def;
};


} // namespace banjo


#endif
