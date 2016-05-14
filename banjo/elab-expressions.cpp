// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "elab-expressions.hpp"
#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{

 Elaborate_expressions::Elaborate_expressions(Parser& p)
  : parser(p), cxt(p.cxt)
{ }


// -------------------------------------------------------------------------- //
// Statements

void
Elaborate_expressions::translation_unit(Translation_unit& s)
{
  statement_seq(s.statements());
}


void
Elaborate_expressions::statement(Stmt& s)
{
  struct fn
  {
    Self& elab;
    void operator()(Stmt& s)             { lingo_unhandled(s); }
    void operator()(Compound_stmt& s)    { elab.compound_statement(s); }
    void operator()(Return_stmt& s)      { elab.return_statement(s); }
    void operator()(Yield_stmt& s)       { elab.yield_statement(s); }
    void operator()(If_then_stmt& s)     { elab.if_statement(s); }
    void operator()(If_else_stmt& s)     { elab.if_statement(s); }
    void operator()(While_stmt& s)       { elab.while_statement(s); }
    void operator()(Break_stmt& s)       { /* Do nothing. */ }
    void operator()(Continue_stmt& s)    { /* Do nothing. */ }
    void operator()(Declaration_stmt& s) { elab.declaration_statement(s); }
    void operator()(Expression_stmt& s)  { elab.expression_statement(s); }
  };
  apply(s, fn{*this});
}


void
Elaborate_expressions::statement_seq(Stmt_list& ss)
{
  for (Stmt& s : ss)
    statement(s);
}


void
Elaborate_expressions::compound_statement(Compound_stmt& s)
{
  statement_seq(s.statements());
}


void
Elaborate_expressions::return_statement(Return_stmt& s)
{
  s.expr_ = &expression(s.expression());
}


void
Elaborate_expressions::yield_statement(Yield_stmt& s)
{
  s.expr_ = &expression(s.expression());
}


void
Elaborate_expressions::if_statement(If_then_stmt& s)
{
  s.cond_ = &expression(s.condition());
  statement(s.true_branch());
}


void
Elaborate_expressions::if_statement(If_else_stmt& s)
{
  s.cond_ = &expression(s.condition());
  statement(s.true_branch());
  statement(s.false_branch());
}


void
Elaborate_expressions::while_statement(While_stmt& s)
{
  s.cond_ = &expression(s.condition());
  statement(s.body());
}


void
Elaborate_expressions::declaration_statement(Declaration_stmt& s)
{
  declaration(s.declaration());
}


void
Elaborate_expressions::expression_statement(Expression_stmt& s)
{
  s.expr_ = &expression(s.expression());
}


// -------------------------------------------------------------------------- //
// Declarations

void
Elaborate_expressions::declaration(Decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Decl& d)          { lingo_unhandled(d); }
    void operator()(Variable_decl& d) { elab.variable_declaration(d); }
    void operator()(Super_decl& d)    { /* Do nothing. */ }
    void operator()(Function_decl& d) { elab.function_declaration(d); }
    void operator()(Class_decl& d)    { elab.class_declaration(d); }
  };
  apply(d, fn{*this});
}


void
Elaborate_expressions::variable_declaration(Variable_decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Def& d)            { lingo_unhandled(d); }
    void operator()(Empty_def& d)      { /* Do nothing. */ }
    void operator()(Expression_def& d) { d.expr_ = &elab.expression(d.expression()); }
  };
  apply(d.initializer(), fn{*this});
}


// TODO: Parse default arguments.
//
// TODO: Should we have transformed expression definitions into legitimate
// function bodies at this point?
void
Elaborate_expressions::function_declaration(Function_decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Def& d)            { lingo_unhandled(d); }
    void operator()(Expression_def& d) { d.expr_ = &elab.expression(d.expression()); }
    void operator()(Function_def& d)   { elab.statement(d.statement()); }
  };

  // Declare parameters.
  //
  // TODO: Do as C++ does and forbid the declaration of locals within
  // the function block having the same name as a parameter. We could
  // do this by declaring them directly in the function block before
  // any locals (this is a better idea than what I'm doing here).
  Enter_scope scope(cxt);
  for (Decl& p : d.parameters())
    declare(cxt, p);

  apply(d.definition(), fn{*this});
}


void
Elaborate_expressions::class_declaration(Class_decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Def& d)       { lingo_unhandled(d); }
    void operator()(Class_def& d) { elab.statement_seq(d.statements()); }
  };
  apply(d.definition(), fn{*this});
}


// -------------------------------------------------------------------------- //
// Expressions

// Parse the expression as needed, returning its fully elaborated form.
Expr&
Elaborate_expressions::expression(Expr& e)
{
  if (Unparsed_expr* u = as<Unparsed_expr>(&e)) {
    Save_input_location loc(cxt);
    Token_stream ts(u->tokens());
    Parser p(cxt, ts);
    return p.expression();    
  }
  return e;
}


} // namespace banjo
