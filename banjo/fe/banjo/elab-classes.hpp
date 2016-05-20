// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_FE_ELAB_CLASSES_HPP
#define BANJO_FE_ELAB_CLASSES_HPP

#include <banjo/language.hpp>


namespace banjo
{

namespace fe
{

struct Parser;
struct Context;


// Recursively complete all class definitions in a translation unit.
struct Elaborate_classes
{
  using Self = Elaborate_classes;

  Elaborate_classes(Parser&);

  void operator()(Translation_unit& s) { translation_unit(s); }

  void translation_unit(Translation_unit&);

  void statement(Stmt&);
  void statement_seq(Stmt_list&);
  void compound_statement(Compound_stmt&);
  void declaration_statement(Declaration_stmt&);

  void declaration(Decl&);
  void function_declaration(Function_decl&);
  void class_declaration(Class_decl&);

  Parser&  parser;
  Context& cxt;
};


} // namespace fe

} // nammespace banjo


#endif
