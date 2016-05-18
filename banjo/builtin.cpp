// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builtin.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "declaration.hpp"
#include "printer.hpp"

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
  tu.spec_ |= internal_spec;
  cxt.tu_ = &tu;
  return tu;
}


// -------------------------------------------------------------------------- //
// class Compiler { ... }
//
// The compiler class provides compile-time information about the
// compiler and its flags, and also operations that expose language
// specific operations (e.g., diagnostics, queries, and reflection).

static Declaration_stmt&
make_compiler_version(Context& cxt, Class_def& def)
{
  Name& name = cxt.get_id("version");
  Type& type = cxt.get_int_type();
  Expr& init = cxt.get_integer(type, 1);
  Decl& decl = cxt.make_constant_declaration(name, type, init);
  decl.spec_ |= internal_spec;

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
    &make_compiler_version(cxt, def)
    // ...
  };
  for (Declaration_stmt* s : decls)
    def.statements().push_back(*s);

  std::cout << cls << '\n';

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

  std::cout << decl << '\n';

  // TODO: Declare this and evaluate it.
  //
  // TODO: Presumably, the compile has some constant fields associated
  // with it (vendor, version, etc.). In order to initialize these, we
  // need to create a tuple containing the relevant fields.

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
  Enter_scope(cxt, tu);
  make_compiler_class(cxt);
  make_compiler(cxt);
}


} // namespace banjo

