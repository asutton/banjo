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



// The type builder is responsible for the allocation of types.
struct Type_builder : Builder_base
{
  using Builder_base::Builder_base;

  // Fundamental types
  Type get_void_type(Qualifier_set = {});
  Type get_bool_type(Qualifier_set = {}, Reference_kind = {});
  Type get_byte_type(Qualifier_set = {}, Reference_kind = {});
  Type get_integer_type(bool, int, Qualifier_set = {}, Reference_kind = {});
  Type get_int_type(Qualifier_set = {}, Reference_kind = {});
  Type get_uint_type(Qualifier_set = {}, Reference_kind = {});
  Type get_float_type(int, Qualifier_set = {}, Reference_kind = {});

  // Composite types
  Type get_function_type(Decl_list const&, Type, Reference_kind = {});
  Type get_function_type(Type_list const&, Type, Reference_kind = {});
  Type get_array_type(Type, Expr&, Reference_kind = {});
  Type get_tuple_type(Type_list&&, Reference_kind = {});
  Type get_tuple_type(Type_list const&, Reference_kind = {});
  Type get_pointer_type(Type, Qualifier_set = {}, Reference_kind = {});

  // User-defined types
  Type get_class_type(Type_decl&, Qualifier_set = {}, Reference_kind = {});
  Type get_typename_type(Type_decl&, Qualifier_set = {}, Reference_kind = {});
  
  // Placeholder types
  Type get_auto_type(Type_decl&, Qualifier_set = {}, Reference_kind = {});
  Type get_decltype_type(Expr&, Qualifier_set = {}, Reference_kind = {});

  // Meta-types
  Type get_type_type();

  // Qualified types
  Type get_qualified_type(Type&, Qualifier_set);
  Type get_unqualified_type(Type&);
  Type get_const_type(Type&);
  Type get_volatile_type(Type&);
  Type get_cv_type(Type&);

  // Fresh types
  Type make_auto_type();
  Type make_synthetic_type(Decl&);

  Single_factory<Void_type> void_type;
  Single_factory<Boolean_type> bool_type;
  Single_factory<Byte_type> byte_type;
  Unique_factory<Integer_type> integer_type;
  Unique_factory<Float_type> float_type;
  Unique_factory<Function_type> function_type;
  Unique_factory<Array_type> array_type;
  Unique_factory<Tuple_type> tuple_type;
  Unique_factory<Pointer_type> pointer_type;
  Unique_factory<Class_type> class_type;
  Unique_factory<Typename_type> typename_type;
  Unique_factory<Auto_type> auto_type;
  Unique_factory<Decltype_type> decltype_type;
  Single_factory<Type_type> type_type;
};



// -------------------------------------------------------------------------- //
// Fundamental types


Type
Builder::get_void_type(Qualifier_set q)
{
  return {void_type(), q};
}


Type
Builder::get_bool_type(Qualifier_set q, Reference_kind r)
{
  return {bool_type(), q, r};
}


Type
Builder::get_byte_type(Qualifier_set q, Reference_kind r)
{
  return {byte_type(), q, r}
}


Type
Builder::get_integer_type(bool s, int p, Qualifier_set q, Reference_kind r)
{
  return {integer_type(s, p), q, r};
}


// TODO: Default precision should depend on the target platform. That
// information needs to be supplied by an external argument.
Type
Builder::get_int_type(Qualifier_set q, Reference_kind r)
{
  return get_integer_type(true, 32, q, r);
}


// TODO: See comments above.
Type
Builder::get_uint_type(Qualifier_set q, Reference_kind r)
{
  return get_integer_type(false, 32, q, r);
}


Type
Builder::get_float_type(int p, Qualifier_set q, Reference_kind r)
{
  return {float_type(p), q, r};
}


// -------------------------------------------------------------------------- //
// Compound types

Type
Builder::get_function_type(Decl_list const& ps, Type t, Reference_kind r)
{
  Type_list ts;
  for (Decl& d : modify(ps))
    ts.push_back(p.type());
  return get_function_type(ts, t, r);
}


Type
Builder::get_function_type(Type_list const& ts, Type t, Reference_kind r)
{
  return {function_type(ts, t), empty_qual, r};
}


Type
Builder::get_array_type(Type t, Expr& e, Reference_kind r)
{
  return {array_type(t, e), empty_qual, r};
}


Type
Builder::get_tuple_type(Type_list&& t, Reference_kind r)
{
  return {tuple_type(std::move(t)), r};
}


Type
Builder::get_tuple_type(Type_list const& t, Reference_kind r)
{
  return {tuple_type(t), r};
}


Type
Builder::get_pointer_type(Type t, Qualifier_set q, Reference_kind r)
{
  return {pointer_type(t), q, r};
}


// -------------------------------------------------------------------------- //
// User-defined types

// Returns class type for the given type declaration.
Type
Builder::get_class_type(Type_decl& d, Qualifier_set q, Reference_kind r)
{
  return {class_type(d), q, r};
}


// Returns the type corresponding to the declaration of a type parameter.
Type
Builder::get_typename_type(Type_decl& d, Qualifier_set q, Reference_kind r)
{
  return {typename_type(d), q, r};
}


// -------------------------------------------------------------------------- //
// Deduced types

// Returns the auto type corresponding to the given declaration.
Type
Builder::get_auto_type(Type_decl& d, Qualifier_set q, Reference_kind r)
{
  return {auto_type(d), q, r};
}


Type
Builder::get_decltype_type(Expr& e, Qualifier_set q, Reference_kind r)
{
  return {declared_type(e), q, r};
}


Type
Builder::get_type_type()
{
  return {type_type(), q, r};
}


// -------------------------------------------------------------------------- //
// Qualified types


static Type
qualified_array_type(Type_builder& b, Array_type& t, Qualifier_set q, Reference_kind r)
{
  Type et(t.element_type(), q);
  return b.get_array_type(et, empty_qual, r);
}


static Type
qualified_tuple_type(Type_builder& b, Tuple_type& t, Qualifier_set q, Reference_kind r)
{
  Type_list ts;
  for (Type& et : t.element_types())
    ts.push_back({et, q});
  return b.get_tuple_type(std::move(ts), empty_qual, r);
}


// Return a type qualified by q. This preserves the reference kind of the 
// original type. Qualifiers applied to an array or tuple type apply to their 
// element type(s).
Type
Type_builder::get_qualified_type(Type t, Qualifier_set q)
{
  struct fn
  {
    Type_builder&  self;
    Reference_kind r;
    Type operator()(Basic_type& t) { return {t, q, r}; }
    Type operator()(Array_type& t) { return qualified_array_type(self, t, q, r); }
    Type operator()(Tuple_type& t) { return qualified_tuple_type{self, t, q, r}; }
  };
  return apply(t, fn{r});
}


Type
Type_builder::get_unqualified_type(Type t)
{
  return get_qualified_type(t, empty_qual);
}


Type
Type_builder::get_const_type(Type t)
{
  return get_qualified_type(t, const_qual);
}


Type
Type_builder::get_volatile_type(Type t)
{
  return get_qualified_type(t, volatile_qual);
}


Type
Type_builder::get_cv_type(Type t)
{
  return get_qualified_type(t, cv_qual);
}


// -------------------------------------------------------------------------- //
// Fresh types


// Create a new auto placeholder type. This creates a new, unique type
// parameter and returns its associated type. Note that the type parameter
// is unassociated with any context at the point of creation (it's an
// unbound, unnamed type variable).
Auto_type&
Builder::make_auto_type()
{
  Name& n = get_id();
  Type_parm& d = make<Type_parm>(Index {}, n);
  return get_auto_type(d);
}


// Synthesize a type from the given parameter. This is used to generate
// fake types corresponding to type parameters.
Synthetic_type&
Builder::make_synthetic_type(Decl& d)
{
  return make<Synthetic_type>(d);
}



// -------------------------------------------------------------------------- //
// Expressions

Void_expr&
Builder::get_void()
{
  return make<Void_expr>(get_void_type());
}


Boolean_expr&
Builder::get_bool(bool b)
{
  return make<Boolean_expr>(get_bool_type(), b);
}


Boolean_expr&
Builder::get_true()
{
  return get_bool(true);
}


Boolean_expr&
Builder::get_false()
{
  return get_bool(false);
}


// TODO: Verify that T can have an integer value?
// I think that all scalars can have integer values.
Integer_expr&
Builder::get_integer(Type& t, Integer const& n)
{
  return make<Integer_expr>(t, n);
}


// Returns the 0 constant, with scalar type `t`.
//
// TODO: Verify that t is scalar.
//
// TODO: Produce zero interpratations for any T?
Integer_expr&
Builder::get_zero(Type& t)
{
  return get_integer(t, 0);
}


Integer_expr&
Builder::get_int(Integer const& n)
{
  return get_integer(get_int_type(), n);
}


Integer_expr&
Builder::get_uint(Integer const& n)
{
  // lingo_assert(n.is_nonnegative(n));
  return get_integer(get_uint_type(), n);
}


Tuple_expr&
Builder::make_tuple(Type& t, Expr_list&& l)
{
  return make<Tuple_expr>(t, std::move(l));
}


// Create a reference to a declared object (variable or parameter).
Object_expr&
Builder::make_reference(Category c, Type& t, Object_decl& d)
{
  return make<Object_expr>(c, t, d.name(), d);
}


// Create a reference to a declared value (constant).
Value_expr&
Builder::make_reference(Category c, Type& t, Value_decl& d)
{
  return make<Value_expr>(c, t, d.name(), d);
}


// Create a reference to a function.
Function_expr&
Builder::make_reference(Category c, Type& t, Function_decl& d)
{
  Name& n = d.name();
  return make<Function_expr>(c, t, n, d);
}


// Create an expression referring to an overload set. Note that overload
// sets are untyped and have no value category.
Overload_expr&
Builder::make_reference(Name& n, Decl_list&& ds)
{
  return make<Overload_expr>(n, std::move(ds));
}


Field_expr&
Builder::make_reference(Category c, Type& t, Expr& e, Field_decl& d)
{
  Name& n = d.name();
  return make<Field_expr>(c, t, e, n, d);
}


Method_expr&
Builder::make_reference(Category c, Type& t, Expr& e, Method_decl& d)
{
  Name& n = d.name();
  return make<Method_expr>(c, t, e, n, d);
}


Member_expr&
Builder::make_reference(Expr& e, Name& n, Decl_list&& ds)
{
  lingo_unimplemented(__func__);
}


And_expr&
Builder::make_and(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<And_expr>(c, t, e1, e2);
}


Or_expr&
Builder::make_or(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Or_expr>(c, t, e1, e2);
}


Not_expr&
Builder::make_not(Category c, Type& t, Expr& e)
{
  return make<Not_expr>(c, t, e);
}


Eq_expr&
Builder::make_eq(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Eq_expr>(c, t, e1, e2);
}


Ne_expr&
Builder::make_ne(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Ne_expr>(c, t, e1, e2);
}


Lt_expr&
Builder::make_lt(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Lt_expr>(c, t, e1, e2);
}


Gt_expr&
Builder::make_gt(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Gt_expr>(c, t, e1, e2);
}


Le_expr&
Builder::make_le(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Le_expr>(c, t, e1, e2);
}


Ge_expr&
Builder::make_ge(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Ge_expr>(c, t, e1, e2);
}


Add_expr&
Builder::make_add(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Add_expr>(c, t, e1, e2);
}


Sub_expr&
Builder::make_sub(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Sub_expr>(c, t, e1, e2);
}


Mul_expr&
Builder::make_mul(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Mul_expr>(c, t, e1, e2);
}


Div_expr&
Builder::make_div(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Div_expr>(c, t, e1, e2);
}


Rem_expr&
Builder::make_rem(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Rem_expr>(c, t, e1, e2);
}


Neg_expr&
Builder::make_neg(Category c, Type& t, Expr& e)
{
  return make<Neg_expr>(c, t, e);
}


Pos_expr&
Builder::make_pos(Category c, Type& t, Expr& e)
{
  return make<Pos_expr>(c, t, e);
}


Bit_and_expr&
Builder::make_bit_and(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Bit_and_expr>(c, t, e1, e2);
}


Bit_or_expr&
Builder::make_bit_or(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Bit_or_expr>(c, t, e1, e2);
}


Bit_xor_expr&
Builder::make_bit_xor(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Bit_xor_expr>(c, t, e1, e2);
}


Bit_lsh_expr&
Builder::make_bit_lsh(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Bit_lsh_expr>(c, t, e1, e2);
}


Bit_rsh_expr&
Builder::make_bit_rsh(Category c, Type& t, Expr& e1, Expr& e2)
{
  return make<Bit_rsh_expr>(c, t, e1, e2);
}


Bit_not_expr&
Builder::make_bit_not(Category c, Type& t, Expr& e)
{
  return make<Bit_not_expr>(c, t, e);
}


Call_expr&
Builder::make_call(Category c, Type& t, Expr& f, Expr_list const& a)
{
  return make<Call_expr>(c, t, f, a);
}


Requires_expr&
Builder::make_requires(Decl_list const& tps, Decl_list const& ps, Req_list const& rs)
{
  return make<Requires_expr>(get_bool_type(), tps, ps, rs);
}


Synthetic_expr&
Builder::synthesize_expression(Category c, Decl& d)
{
  return make<Synthetic_expr>(c, declared_type(d), d);
}


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
