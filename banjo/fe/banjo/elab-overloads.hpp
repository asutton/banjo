// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_FE_ELAB_OVERLOADS_HPP
#define BANJO_FE_ELAB_OVERLOADS_HPP

#include <banjo/language.hpp>


namespace banjo
{

namespace fe
{

struct Parser;
struct Context;

// Recursively parse and analyze expressions in the translation unit.
// At the time of elaboration, all types of expressions shall be complete
// or completable. In other words, there shall be no cycles of type 
// dependence.
//
// TODO: Is there a way that we can just apply this directly to the
// scope and overload sets and not the grammer? There's a linear
// search cost for every overloaded declaration in scope.
struct Elaborate_overloads
{
  using Self = Elaborate_overloads;

  Elaborate_overloads(Parser&);

  void operator()(Translation_unit& s) { translation_unit(s); }

  void translation_unit(Translation_unit&);

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


} // namespace fe

} // nammespace banjo


#endif
