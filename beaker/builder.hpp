// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_BUILDER_HPP
#define BEAKER_BUILDER_HPP

#include "context.hpp"
#include "token.hpp"
#include "ast.hpp"


namespace beaker
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
  Placeholder_id& get_id();
  // Operator_id&    get_id();
  // Conversion_id&  get_id();
  // Literal_id&     get_id();
  Destructor_id&  get_id(Type const&);
  // Template_id&    get_id(Decl& d);
  Qualified_id&   get_id(Decl&, Name&);
  Global_id&      get_global_id();

  // Types
  Void_type&      get_void_type();
  Boolean_type&   get_bool_type();
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

  Boolean_expr&   get_bool(bool);
  Boolean_expr&   get_true();
  Boolean_expr&   get_false();
  Integer_expr&   get_integer(Type&, int);
  Integer_expr&   get_zero(Type&);
  Integer_expr&   get_int(int);
  Integer_expr&   get_uint(unsigned);
  Reference_expr& make_reference(Variable_decl& d);
  Reference_expr& make_reference(Constant_decl& d);
  Reference_expr& make_reference(Function_decl& d);

  And_expr&       make_and(Type&, Expr&, Expr&);
  Not_expr&       make_not(Type&, Expr&);
  Call_expr&      make_call(Type&, Function_decl&, Expr_list const&);

  Equal_init&        make_equal_init(Expr&);
  Paren_init&        make_paren_init(Expr_list const&);
  Brace_init&        make_brace_init(Expr_list const&);
  Structural_init&   make_structural_init(Type&, Expr_list const&);
  Trivial_init&      make_trivial_init(Type&);
  Zero_init&         make_zero_init(Type&, Expr&);
  Constructor_init&  make_constructor_init(Type&, Decl&, Expr_list const&);
  Object_init&       make_object_init(Type&, Expr&);
  Reference_init&    make_reference_init(Type&, Expr&);
  Aggregate_init&    make_aggregate_init(Type&);

  Namespace_decl& make_namespace(Name&);
  Namespace_decl& make_namespace(char const*);
  Namespace_decl& get_global_namespace();

  Variable_decl& make_variable(Name&, Type&);
  Variable_decl& make_variable(char const*, Type&);
  Variable_decl& make_variable(Name&, Type&, Init&);

  Function_decl& make_function(Name&, Decl_list const&, Type&);
  Function_decl& make_function(char const*, Decl_list const&, Type&);

  Template_decl& make_template(Decl_list const&, Decl&);

  Object_parm& make_object_parm(Name&, Type&);
  Object_parm& make_object_parm(char const*, Type&);
  Value_parm&  make_value_parm(Name&, Type&);
  Value_parm&  make_value_parm(char const*, Type&);
  Type_parm&   make_type_parm(Name&);
  Type_parm&   make_type_parm(char const*);

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


// -------------------------------------------------------------------------- //
// Names

// Returns a simple identifier with the given spelling.
//
// TODO: Unique this?
inline Simple_id&
Builder::get_id(char const* s)
{
  Symbol const* sym = symbols().put_identifier(identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a simple identifier with the given spelling.
inline Simple_id&
Builder::get_id(std::string const& s)
{
  Symbol const* sym = symbols().put_identifier(identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a placeholder for a name.
inline Placeholder_id&
Builder::get_id()
{
  return make<Placeholder_id>();
}


// Returns a destructor-id for the given type.
inline Destructor_id&
Builder::get_id(Type const& t)
{
  lingo_unimplemented();
}


// Returns a qualified-id.
inline Qualified_id&
Builder::get_id(Decl& d, Name& n)
{
  return make<Qualified_id>(d, n);
}


// Return the global identifier.
inline Global_id&
Builder::get_global_id()
{
  // TODO: Global or no?
  static Global_id n;
  return n;
}


// -------------------------------------------------------------------------- //
// Types

inline Void_type&
Builder::get_void_type()
{
  return make<Void_type>();
}


inline Boolean_type&
Builder::get_bool_type()
{
  return make<Boolean_type>();
}


inline Integer_type&
Builder::get_integer_type(bool s, int p)
{
  return make<Integer_type>(s, p);
}


// TODO: Default precision depends on configuration.
inline Integer_type&
Builder::get_int_type()
{
  return get_integer_type(true, 32);
}


// TODO: Default precision depends on configuration.
inline Integer_type&
Builder::get_uint_type()
{
  return get_integer_type(false, 32);
}


inline Float_type&
Builder::get_float_type()
{
  return make<Float_type>();
}


inline Auto_type&
Builder::get_auto_type()
{
  return make<Auto_type>();
}


inline Decltype_type&
Builder::get_decltype_type(Expr&)
{
  lingo_unimplemented();
}


inline Declauto_type&
Builder::get_declauto_type()
{
  return make<Declauto_type>();
}


inline Function_type&
Builder::get_function_type(Decl_list const& ps, Type& r)
{
  Type_list ts;
  for (Decl& d : *modify(&ps)) {
    Object_parm& p = cast<Object_parm>(d);
    ts.push_back(p.type());
  }
  return get_function_type(ts, r);
}


inline Function_type&
Builder::get_function_type(Type_list const& ts, Type& r)
{
  return make<Function_type>(ts, r);
}


// TODO: Do not build qualified types for functions or arrays.
// Is that a hard error, or do we simply fold the const into
// the return type and/or element type?
inline Qualified_type&
Builder::get_qualified_type(Type& t, Qualifier_set qual)
{
  if (Qualified_type* q = as<Qualified_type>(&t)) {
    q->qual |= qual;
    return *q;
  }
  return make<Qualified_type>(t, qual);
}


inline Qualified_type&
Builder::get_const_type(Type& t)
{
  return get_qualified_type(t, const_qual);
}


inline Qualified_type&
Builder::get_volatile_type(Type& t)
{
  return get_qualified_type(t, volatile_qual);
}


inline Pointer_type&
Builder::get_pointer_type(Type& t)
{
  return make<Pointer_type>(t);
}


inline Reference_type&
Builder::get_reference_type(Type& t)
{
  return make<Reference_type>(t);
}


inline Array_type&
Builder::get_array_type(Type&, Expr&)
{
  lingo_unimplemented();
}


inline Sequence_type&
Builder::get_sequence_type(Type& t)
{
  return make<Sequence_type>(t);
}


inline Class_type&
Builder::get_class_type(Decl& d)
{
  lingo_unimplemented();
}


inline Union_type&
Builder::get_union_type(Decl& d)
{
  lingo_unimplemented();
}


inline Enum_type&
Builder::get_enum_type(Decl& d)
{
  lingo_unimplemented();
}


inline Typename_type&
Builder::get_typename_type(Decl& d)
{
  return make<Typename_type>(d);
}


// -------------------------------------------------------------------------- //
// Expressions

inline Boolean_expr&
Builder::get_bool(bool b)
{
  Symbol const* sym = symbols().get(b ? "true" : "false");
  return make<Boolean_expr>(get_bool_type(), *sym);
}


inline Boolean_expr&
Builder::get_true()
{
  return get_bool(true);
}


inline Boolean_expr&
Builder::get_false()
{
  return get_bool(false);
}


// TODO: Verify that T can have an integer value?
// I think that all scalars can have integer values.
inline Integer_expr&
Builder::get_integer(Type& t, int n)
{
  Symbol const* sym = symbols().put_integer(integer_tok, std::to_string(n), n);
  return make<Integer_expr>(t, *sym);
}


// Returns the 0 constant, with scalar type `t`.
//
// TODO: Verify that t is scalar.
inline Integer_expr&
Builder::get_zero(Type& t)
{
  return get_integer(t, 0);
}



inline Integer_expr&
Builder::get_int(int n)
{
  return get_integer(get_int_type(), n);
}


inline Integer_expr&
Builder::get_uint(unsigned n)
{
  return get_integer(get_uint_type(), n);
}


// Get an expression that refers to a variable. The type
// is a reference to the declared type of the variable.
inline Reference_expr&
Builder::make_reference(Variable_decl& d)
{
  return make<Reference_expr>(get_reference_type(d.type()), d);
}


inline And_expr&
Builder::make_and(Type& t, Expr& e1, Expr& e2)
{
  return make<And_expr>(t, e1, e2);
}


inline Not_expr&
Builder::make_not(Type& t, Expr& e)
{
  return make<Not_expr>(t, e);
}


inline Call_expr&
Builder::make_call(Type& t, Function_decl& f, Expr_list const& a)
{
  return make<Call_expr>(t, make_reference(f), a);
}


// -------------------------------------------------------------------------- //
// Initializers


inline Equal_init&
Builder::make_equal_init(Expr& e)
{
  return make<Equal_init>(e);
}


inline Paren_init&
Builder::make_paren_init(Expr_list const& es)
{
  return make<Paren_init>(es);
}


inline Brace_init&
Builder::make_brace_init(Expr_list const& es)
{
  return make<Brace_init>(es);
}


inline Structural_init&
Builder::make_structural_init(Type& t, Expr_list const& es)
{
  return make<Structural_init>(t, es);
}


inline Trivial_init&
Builder::make_trivial_init(Type& t)
{
  return make<Trivial_init>(t);
}


inline Zero_init&
Builder::make_zero_init(Type& t, Expr& e)
{
  return make<Zero_init>(t, e);
}


inline Constructor_init&
Builder::make_constructor_init(Type& t, Decl& d, Expr_list const& es)
{
  return make<Constructor_init>(t, d, es);
}


inline Object_init&
Builder::make_object_init(Type& t, Expr& e)
{
  return make<Object_init>(t, e);
}


inline Reference_init&
Builder::make_reference_init(Type& t, Expr& e)
{
  return make<Reference_init>(t, e);
}


inline Aggregate_init&
Builder::make_aggregate_init(Type&)
{
  lingo_unimplemented();
}


// -------------------------------------------------------------------------- //
// Definitions


// -------------------------------------------------------------------------- //
// Declarations

inline Variable_decl&
Builder::make_variable(Name& n, Type& t)
{
  return make_variable(n, t);
}


inline Variable_decl&
Builder::make_variable(char const* s, Type& t)
{
  return make_variable(get_id(s), t);
}


inline Variable_decl&
Builder::make_variable(Name& n, Type& t, Init& i)
{
  return make<Variable_decl>(n, t, i);
}


// Creates an undefined function with parameters ps and return
// type r.
inline Function_decl&
Builder::make_function(Name& n, Decl_list const& ps, Type& r)
{
  Type& t = get_function_type(ps, r);
  return make<Function_decl>(n, t, ps);
}


inline Function_decl&
Builder::make_function(char const* s, Decl_list const& ps, Type& r)
{
  return make_function(get_id(s), ps, r);
}


inline Namespace_decl&
Builder::make_namespace(Name& n)
{
  return make<Namespace_decl>(n);
}


inline Namespace_decl&
Builder::make_namespace(char const* s)
{
  return make_namespace(get_id(s));
}


// FIXME: This should probably be installed on the context.
inline Namespace_decl&
Builder::get_global_namespace()
{
  static Namespace_decl ns(get_global_id());
  return ns;
}


inline Template_decl&
Builder::make_template(Decl_list const& p, Decl& d)
{
  return make<Template_decl>(p, d);
}


// TODO: Parameters can't be functions or void. Check this
// property or assert it.
inline Object_parm&
Builder::make_object_parm(Name& n, Type& t)
{
  return make<Object_parm>(n, t);
}


inline Object_parm&
Builder::make_object_parm(char const* s, Type& t)
{
  return make_object_parm(get_id(s), t);
}


inline Type_parm&
Builder::make_type_parm(Name& n)
{
  return make<Type_parm>(n);
}


inline Type_parm&
Builder::make_type_parm(char const* n)
{
  return make_type_parm(get_id(n));
}


inline Value_parm&
Builder::make_value_parm(Name& n, Type& t)
{
  return make<Value_parm>(n, t);
}


inline Value_parm&
Builder::make_value_parm(char const* s, Type& t)
{
  return make_value_parm(get_id(s), t);
}


} // namespace beaker


#endif
