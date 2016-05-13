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

Elaborate_overloads::Elaborate_overloads(Parser& p)
  : parser(p), cxt(p.cxt)
{ }


// -------------------------------------------------------------------------- //
// Statements

void
Elaborate_overloads::translation_unit(Translation_stmt& s)
{
  statement_seq(s.statements());
}


void
Elaborate_overloads::statement(Stmt& s)
{
  struct fn
  {
    Self& elab;
    void operator()(Stmt& s)             { /* Do nothing. */ }
    void operator()(Compound_stmt& s)    { elab.compound_statement(s); }
    void operator()(Declaration_stmt& s) { elab.declaration_statement(s); }
  };
  apply(s, fn{*this});
}


void
Elaborate_overloads::statement_seq(Stmt_list& ss)
{
  for (Stmt& s : ss)
    statement(s);
}


void
Elaborate_overloads::compound_statement(Compound_stmt& s)
{
  statement_seq(s.statements());
}


void
Elaborate_overloads::declaration_statement(Declaration_stmt& s)
{
  declaration(s.declaration());
}


// -------------------------------------------------------------------------- //
// Declarations

void
Elaborate_overloads::declaration(Decl& d)
{
  Name& name = decl.name();
  Overload_set& ovl = *current_scope().lookup(name);

  // Find the position of the declaration within the overload
  // set. We only need to compare it with declarations "down stream"
  // since we will have validated all preceding declarations.
  auto iter = std::find_if(ovl.begin(), ovl.end(), [&decl](Decl& d) {
    return &decl == &d;
  });

  // Check each downstream declaration in turn, trapping declaration
  // errors so we can diagnose as many as possible.
  bool ok = true;
  for (++iter ; iter != ovl.end(); ++iter) {
    try {
      check_declarations(cxt, decl, *iter);
    } catch (...) {
      ok = false;
    }
  }

  // If we got an error, rethrow it.
  if (!ok)
    throw Declaration_error();

  // Otherwise, recurse.
  struct fn
  {
    Self& elab;
    void operator()(Decl& d)          { lingo_unhandled(d); }
    void operator()(Function_decl& d) { elab.function_declaration(d); }
    void operator()(Class_decl& d)    { elab.class_declaration(d); }
  };
  apply(d, fn{*this});
}



// TODO: Parse default arguments.
//
// TODO: Should we have transformed expression definitions into legitimate
// function bodies at this point?
void
Elaborate_overloads::function_declaration(Function_decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Def& d)            { lingo_unhandled(d); }
    void operator()(Expression_def& d) { d.expr_ = &elab.expression(d.expression()); }
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
Elaborate_overloads::class_declaration(Class_decl& d)
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
Elaborate_overloads::expression(Expr& e)
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


// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{


void
Parser::elaborate_overloads(Stmt_list& ss)
{
  for (Stmt& s : ss)
    elaborate_overloads(s);
}


void
Parser::elaborate_overloads(Stmt& s)
{
  if (Declaration_stmt* s1 = as<Declaration_stmt>(&s))
    elaborate_overloads(s1->declaration());
  
  if (Compound_stmt* s1 = as<Compound_stmt>(&s))
    elaborate_overloads(s1->statements());
}


void
Parser::elaborate_overloads(Decl& decl)
{

}


} // namespace banjo
