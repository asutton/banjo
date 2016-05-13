// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_ELAB_OVERLOADS_HPP
#define BANJO_ELAB_OVERLOADS_HPP

#include "language.hpp"


namespace banjo
{

struct Parser;


// Recursively parse and analyze expressions in the translation unit.
// At the time of elaboration, all types of expressions shall be complete
// or completable. In other words, there shall be no cycles of type 
// dependence.
struct Elaborate_overloads
{
  using Self = Elaborate_overloads;

  Elaborate_overloads(Parser&);

  void operator()(Translation_stmt& s) { translation_unit(s); }

  void translation_unit(Translation_stmt&);

  void statement(Stmt&);
  void statement_seq(Stmt_list&);
  void compound_statement(Compound_stmt&);
  void return_statement(Return_stmt&);
  void yield_statement(Yield_stmt&);
  void if_statement(If_then_stmt&);
  void if_statement(If_else_stmt&);
  void while_statement(While_stmt&);
  void declaration_statement(Declaration_stmt&);
  void expression_statement(Expression_stmt&);

  void declaration(Decl&);
  void variable_declaration(Variable_decl&);
  void function_declaration(Function_decl&);
  void class_declaration(Class_decl&);

  Expr& expression(Expr&);

  Parser&  parser;
  Context& cxt;
};


} // nammespace banjo


#endif
