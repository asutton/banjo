// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_ELAB_DECLARATIONS_HPP
#define BANJO_ELAB_DECLARATIONS_HPP

#include "language.hpp"


namespace banjo
{

struct Parser;


// Recursively parse and analyze the types of all declared names. After 
// elaboration, every declaration has a type, which may be a placeholder.
struct Elaborate_declarations
{
  using Self = Elaborate_declarations;

  Elaborate_declarations(Parser&);

  void operator()(Translation_unit& s) { translation_unit(s); }

  void translation_unit(Translation_unit&);

  void statement(Stmt&);
  void statement_seq(Stmt_list&);
  void compound_statement(Compound_stmt&);
  void declaration_statement(Declaration_stmt&);

  void declaration(Decl&);
  void variable_declaration(Variable_decl&);
  void constant_declaration(Constant_decl&);
  void super_declaration(Super_decl&);
  void function_declaration(Function_decl&);
  void coroutine_declaration(Coroutine_decl&);
  void class_declaration(Class_decl&);

  void parameter(Decl&);
  void parameter(Object_parm&);

  Type& type(Type&);

  Parser&  parser;
  Context& cxt;
};


} // nammespace banjo


#endif
