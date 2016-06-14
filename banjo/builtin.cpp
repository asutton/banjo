// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builtin.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "intrinsic.hpp"
#include "declaration.hpp"
#include "debugging.hpp"


namespace banjo
{

namespace
{

// Indicates a requested type.
enum Typename {
  void_obj,
  bool_obj,
  byte_obj,
  int_obj,
  float_obj,
};


// TODO: Return tr if t is a reference type specifier.
inline bool
is_reference(Typename t)
{
  return false;
}


// TODO: Return the non-reference typename for t.
inline Typename
get_value_type(Typename t)
{
  return t;
}


struct Binary_op
{
  Operator_kind name;
  Typename      parms[2];
  Typename      ret;
  Binary_fn     def;
};


Type&
make_value_type(Context& cxt, Typename t)
{
  Typename v = get_value_type(t);
  switch (v) {
    case bool_obj:
      return cxt.get_bool_type();
    case byte_obj:
      return cxt.get_byte_type();
    case int_obj:
      return cxt.get_int_type();
    default:
      lingo_unreachable();
  }
}

Type&
make_type(Context& cxt, Typename t)
{
  Type& type = make_value_type(cxt, t);
  if (is_reference(t))
    return cxt.get_reference_type(type);
  else
    return type;
}


Decl&
make_parm(Context& cxt, Typename t)
{
  Name& name = cxt.get_id();
  Type& type = make_type(cxt, t);
  return cxt.make_variable_parameter(name, type);
}


template<int N>
Decl_list
make_parms(Context& cxt, Typename const (&ts)[N])
{
  Decl_list parms;
  for (Typename t : ts)
    parms.push_back(make_parm(cxt, t));
  return parms;
}


// TODO: The return value should be an object.
Type& 
make_ret(Context& cxt, Typename t)
{
  return make_type(cxt, t);
}


Decl&
create(Context& cxt, Binary_op const& op)
{
  Name& name = cxt.get_id(op.name);
  Decl_list parms = make_parms(cxt, op.parms);
  Type& ret = make_ret(cxt, op.ret);

  // Build the function type.
  Type_list ts;
  for (Decl& p : parms)
    ts.push_back(cast<Typed_decl>(p).type());
  Type& type = cxt.get_function_type(std::move(ts), ret);
  
  Def& def = cxt.make_function_definition(op.def);

  return cxt.make_function_declaration(name, type, std::move(parms), def);
}


// Translate a sequence of binary op specifications into definitions.
template<int N>
void
create(Context& cxt, Binary_op const (&ops)[N])
{
  for (Binary_op const& op : ops) {
    Decl& decl = create(cxt, op);
    declare(cxt, decl);
  }
}


} // namespace


// Predefine all builtin operators.
static void
make_builtin_ops(Context& cxt, Builtins& bi)
{
  Binary_op binops[] = {
    {eq_op, {bool_obj, bool_obj}, bool_obj, intrinsic::eq_int },
    {ne_op, {bool_obj, bool_obj}, bool_obj, intrinsic::ne_int },
    
    {eq_op, {int_obj, int_obj}, int_obj, intrinsic::eq_int },
    {ne_op, {int_obj, int_obj}, int_obj, intrinsic::ne_int },
  };
  create(cxt, binops);
}


#if 0

// Returns the integer value representing the compiler version.
static inline Expr&
get_compiler_version(Context& cxt)
{
  return cxt.get_int(cxt.get_int_type(), 0);
}


// -------------------------------------------------------------------------- //
// class Compiler { ... }
//
// The compiler class provides compile-time information about the
// compiler and its flags, and also operations that expose language
// specific operations (e.g., diagnostics, queries, and reflection).


// Generate the constant
//
//    static meta var version : int = ...;
//
// TODO: The compiler version is hard-coded as 1. This should be pulled
// from the build environment.
static Declaration_stmt&
make_compiler_version(Context& cxt)
{
  Type& type = cxt.get_int_type();
  Def& init = cxt.make_variable_initializer(get_compiler_version(cxt));
  Decl& decl = cxt.make_object_declaration("version", type, init);
  decl.spec_ |= internal_spec | static_spec | meta_spec;
  
  declare(cxt, decl);

  return cxt.make_declaration_statement(decl);
}


// Generate the "show" macro.
//
//    static meta def show(const n : int) -> void
static Declaration_stmt&
make_compiler_show_int(Context& cxt)
{
  Name& name = cxt.get_id("show");
  Decl_list parms {
    &cxt.make_object_parameter("n", cxt.get_int_type())
  };
  Type& ret = cxt.get_void_type();
  Type& type = cxt.get_function_type(parms, ret);
  Def& def = cxt.make_function_definition(&intrinsic::show_value);
  Decl& decl = cxt.make_function_declaration(name, type, std::move(parms), def);
  decl.spec_ |= internal_spec | static_spec | meta_spec;

  declare(cxt, decl);

  return cxt.make_declaration_statement(decl);
}



static Class_decl& 
make_compiler_class(Context& cxt, Builtins& bi)
{
  Name& name = cxt.get_id("Compiler");
  Type& type = cxt.get_type_type();
  Class_def& def = cxt.make_class_definition(Stmt_list{});
  Class_decl& cls = cxt.make_class_declaration(name, type, def);
  cls.spec_ |= internal_spec;

  // Build the members of the class.
  Enter_scope scope(cxt, cls);
  std::vector<Declaration_stmt*> decls {
    &make_compiler_version(cxt),
    &make_compiler_show_int(cxt)
    // ...
  };
  for (Declaration_stmt* s : decls)
    def.statements().push_back(*s);
  
  declare(cxt, cls);
  
  bi.comp_class_ = &cls;
  return cls;
}


// -------------------------------------------------------------------------- //
// const compiler : Compiler
//
// The compiler object provides access to the compiler class.

static Object_decl&
make_compiler(Context& cxt, Builtins& bi)
{
  Name& name = cxt.get_id("compiler");
  Type& type = cxt.get_class_type(object_type, bi.builtin_compiler_class());
  Object_decl& decl = cxt.make_variable_declaration(name, type);
  decl.spec_ |= internal_spec;

  declare(cxt, decl);

  bi.comp_ = &decl;
  return decl;
}

#endif

// -------------------------------------------------------------------------- //
// Builtin initialization

// Allocate all of the built-in entity definitions and make them available
// for lookup.
void
init_builtins(Context& cxt)
{
  Builtins& bi = cxt.builtins();
  
  // Create the translation unit.
  bi.tu_ = &cxt.make_translation_unit();

  // Put the TU in scope.
  Enter_scope scope(cxt, *bi.tu_);  

  // Initialize all builtin operators
  make_builtin_ops(cxt, bi);
  
  // Create builtin declarations.
  // make_compiler_class(cxt, bi);
  // make_compiler(cxt, bi);
}


} // namespace banjo

