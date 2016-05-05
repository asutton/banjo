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
    void operator()(Class_decl& d)    { p.elaborate_class_definition(d); }
    void operator()(Coroutine_decl& d){ p.elaborate_coroutine_definition(d); }
    void operator()(Super_decl& d)    { /* Nothing to do. */ }
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
Parser::elaborate_coroutine_definition(Coroutine_decl &decl){
  Enter_scope scope(cxt);
  for(Decl& d: decl.parameters())
    declare(cxt, d);
  Function_def* def = dynamic_cast<Function_def*>(&decl.definition());
  if (def == nullptr){
    std::cout << "ERROR\n";
  }


  Stmt& stmt = elaborate_compound_statement(def->statement());
  def->stmt_ = &stmt;
  decl.def_ = def;
}

void
Parser::elaborate_function_definition(Function_decl& decl, Expression_def& def)
{
  // Declare parameters as local variables prior to elaborating
  // the definition.
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
{
  // Declare parameters as local variables prior to elaborating
  // the definition.
  //
  // TODO: Should we be using a specifici kind of scope here?
  Enter_scope scope(cxt);
  for (Decl& d : decl.parameters())
    declare(cxt, d);

  // Elaborate the definition's statement, possibly parsing it.
  Stmt& stmt = elaborate_compound_statement(def.statement());

  // Update the definition with the new statement. We don't need
  // to update the declaration.
  def.stmt_ = &stmt;
}


// Elaborate the definition of a type.
void
Parser::elaborate_class_definition(Class_decl& d)
{
  struct fn
  {
    Parser& p;
    Class_decl& decl;
    void operator()(Def& d)      { lingo_unhandled(d); }
    void operator()(Class_def& d) { p.elaborate_class_definition(decl, d); }
  };
  apply(d.definition(), fn{*this, d});
}



// The type is defined by its body.
void
Parser::elaborate_class_definition(Class_decl& decl, Class_def& def)
{
  // Enter the scope associated with the declaration.
  Enter_scope scope(cxt, cxt.saved_scope(decl));

  // Elaborate the definition's statement, possibly parsing it.
  Stmt& stmt = elaborate_member_statement(def.body());

  // Update the definition with the new statement. We don't need
  // to update the declaration.
  def.body_ = &stmt;
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


Stmt&
Parser::elaborate_compound_statement(Stmt& s)
{
  if (Unparsed_stmt* soup = as<Unparsed_stmt>(&s)) {
    Save_input_location loc(cxt);
    Token_stream ts(soup->tokens());
    Parser parse(cxt, ts);
    return parse.compound_statement();
  }
  return s;
}


Stmt&
Parser::elaborate_member_statement(Stmt& s)
{
  if (Unparsed_stmt* soup = as<Unparsed_stmt>(&s)) {
    Save_input_location loc(cxt);
    Token_stream ts(soup->tokens());
    Parser parse(cxt, ts);
    return parse.member_statement();
  }
  return s;
}


} // namespace banjo
