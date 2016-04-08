// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "declaration.hpp"
#include "ast.hpp"

#include <iostream>


namespace banjo
{

// Elaborate the type of each declaration in turn. Note that elaboration
// and "skip forward" if the type of one declaration depends on the type
// or definition of another defined after it.
void
Parser::elaborate_definitions(Stmt_list& ss)
{
  for (Stmt& s : ss) {
    elaborate_definition(s);
  }
}

// If the statement is a declaration, elaborate its declared type.
void
Parser::elaborate_definition(Stmt& s)
{
  if (Declaration_stmt* s1 = as<Declaration_stmt>(&s))
    elaborate_definition(s1->declaration());
}


void
Parser::elaborate_definition(Decl& d)
{
  struct fn
  {
    Parser& p;
    void operator()(Decl& d)          { lingo_unhandled(d); }
    void operator()(Variable_decl& d) { p.elaborate_variable_initializer(d); }
    void operator()(Function_decl& d) { p.elaborate_function_definition(d); }
    void operator()(Type_decl& d)     { p.elaborate_type_definition(d); }
  };
  apply(d, fn{*this});
}


void
Parser::elaborate_variable_initializer(Variable_decl& d)
{
  struct fn
  {
    Parser& p;
    Variable_decl& var;
    void operator()(Def& d)            { lingo_unhandled(d); }
    void operator()(Empty_def& d)      { p.elaborate_variable_initializer(var, d); }
    void operator()(Expression_def& d) { p.elaborate_variable_initializer(var, d); }
  };
  apply(d.initializer(), fn{*this, d});
}


// TODO: Is there anything that needs to be done here? I suppose that depends
// on the semantics of an empty initializer.
void
Parser::elaborate_variable_initializer(Variable_decl& decl, Empty_def& def)
{ }


void
Parser::elaborate_variable_initializer(Variable_decl& decl, Expression_def& def)
{
  def.expr_ = &elaborate_expression(def.expression());
}


void
Parser::elaborate_function_definition(Function_decl& d)
{
  struct fn
  {
    Parser& p;
    Function_decl& fn;
    void operator()(Def& d)            { lingo_unhandled(d); }
    void operator()(Expression_def& d) { p.elaborate_function_definition(fn, d); }
    void operator()(Function_def& d)   { p.elaborate_function_definition(fn, d); }
  };
  apply(d.definition(), fn{*this, d});
}


void
Parser::elaborate_function_definition(Function_decl& decl, Expression_def& def)
{
  // Declare parameters as local variables prior to elaborating the definition.
  //
  // TODO: Should we be using a specifici kind of scope here?
  Enter_scope scope(cxt);
  for (Decl& d : decl.parameters())
    declare(cxt, d);

  // Elaborate the definition, possibly parsing it.
  Expr& expr = elaborate_expression(def.expression());

  // Transform the returned expression into a normal function
  // definition with a single return statement with that expression.
  Stmt& ret = cxt.make_return_statement(expr);
  Stmt& body = cxt.make_compound_statement({&ret});
  decl.def_ = &cxt.make_function_definition(body);
}


void
Parser::elaborate_function_definition(Function_decl& decl, Function_def& def)
{ }


void
Parser::elaborate_type_definition(Type_decl& d)
{
  std::cout << "TYPE DEF: " << d << '\n';
}


Expr&
Parser::elaborate_expression(Expr& e)
{
  if (Unparsed_expr* soup = as<Unparsed_expr>(&e)) {
    Save_input_location loc(cxt);
    Token_stream ts(soup->tokens());
    Parser parse(cxt, ts);
    return parse.expression();
  }
  return e;
}


} // namespace banjo
