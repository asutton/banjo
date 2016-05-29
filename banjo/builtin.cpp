// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builtin.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "intrinsic.hpp"
#include "declaration.hpp"

// #include "printer.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Translation unit
//
static Translation_unit&
make_translation_unit(Context& cxt)
{
  Translation_unit& tu = cxt.make_translation_unit();
  cxt.tu_ = &tu;
  return tu;
}


// -------------------------------------------------------------------------- //
// class Compiler { ... }
//
// The compiler class provides compile-time information about the
// compiler and its flags, and also operations that expose language
// specific operations (e.g., diagnostics, queries, and reflection).


// Generate the constant
//
//    static const version = ...;
//
// TODO: The compiler version is hard-coded as 1. This should be pulled
// from the build environment.
static Declaration_stmt&
make_compiler_version(Context& cxt)
{
  Name& name = cxt.get_id("version");
  Type& type = cxt.get_int_type();
  Expr& init = cxt.get_integer(type, 1);
  Decl& decl = cxt.make_constant_declaration(name, type, init);
  decl.spec_ |= internal_spec;
  decl.spec_ |= static_spec;

  declare(cxt, decl);

  return cxt.make_declaration_statement(decl);
}


// Generate the following (static) macro.
//
//    static macro show(const n : int) -> void
static Declaration_stmt&
make_compiler_show_int(Context& cxt)
{
  Name& name = cxt.get_id("show");
  Decl_list parms {
    &cxt.make_value_parm(cxt.get_id("n"), cxt.get_int_type())
  };
  Type& ret = cxt.get_void_type();
  Def& def = cxt.make_function_definition(&intrinsic::show_value);
  Decl& decl = cxt.make_macro_declaration(name, std::move(parms), ret, def);
  decl.spec_ |= internal_spec;
  decl.spec_ |= static_spec;

  declare(cxt, decl);

  return cxt.make_declaration_statement(decl);
}



static Class_decl& 
make_compiler_class(Context& cxt)
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
  
  cxt.comp_class_ = &cls;
  return cls;
}


// -------------------------------------------------------------------------- //
// const compiler : Compiler
//
// The compiler object provides access to the compiler class.

static Constant_decl&
make_compiler(Context& cxt)
{
  Name& name = cxt.get_id("compiler");
  Type& type = cxt.get_class_type(cxt.builtin_compiler_class());
  Constant_decl& decl = cxt.make_constant_declaration(name, type);
  decl.spec_ |= internal_spec;

  declare(cxt, decl);

  cxt.comp_ = &decl;
  return decl;
}


// -------------------------------------------------------------------------- //
// Builtin initialization

void 
init_builtins(Context& cxt)
{
  // Create the translation unit.
  Translation_unit& tu = make_translation_unit(cxt);
  
  // Create builtin declarations.
  Enter_scope scope(cxt, tu);
  make_compiler_class(cxt);
  make_compiler(cxt);
}


} // namespace banjo

