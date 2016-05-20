// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "elab-declarations.hpp"
#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{

 Elaborate_declarations::Elaborate_declarations(Parser& p)
  : parser(p), cxt(p.cxt)
{ }


// -------------------------------------------------------------------------- //
// Statements

void
Elaborate_declarations::translation_unit(Translation_unit& tu)
{
  Enter_scope scope(cxt, tu);
  statement_seq(tu.statements());
}


void
Elaborate_declarations::statement(Stmt& s)
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
Elaborate_declarations::statement_seq(Stmt_list& ss)
{
  for (Stmt& s : ss)
    statement(s);
}


void
Elaborate_declarations::compound_statement(Compound_stmt& s)
{
  Enter_scope scope(cxt, cxt.saved_scope(s));
  statement_seq(s.statements());
}


void
Elaborate_declarations::declaration_statement(Declaration_stmt& s)
{
  declaration(s.declaration());
}


// -------------------------------------------------------------------------- //
// Declarations

void
Elaborate_declarations::declaration(Decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Decl& d)           { lingo_unhandled(d); }
    void operator()(Variable_decl& d)  { elab.variable_declaration(d); }
    void operator()(Constant_decl& d)  { elab.constant_declaration(d); }
    void operator()(Super_decl& d)     { elab.super_declaration(d); }
    void operator()(Function_decl& d)  { elab.function_declaration(d); }
    void operator()(Coroutine_decl& d) { elab.coroutine_declaration(d); }
    void operator()(Class_decl& d)     { elab.class_declaration(d); }
  };
  apply(d, fn{*this});
}


void
Elaborate_declarations::variable_declaration(Variable_decl& d)
{
  d.type_ = &type(d.type());
}


void
Elaborate_declarations::constant_declaration(Constant_decl& d)
{
  d.type_ = &type(d.type());
}


void
Elaborate_declarations::super_declaration(Super_decl& d)
{
  d.type_ = &type(d.type());
}


void
Elaborate_declarations::function_declaration(Function_decl& decl)
{
  // Create a new scope for the function and elaborate parameter
  // declarations. Note that we're going 
  Decl_list& parms = decl.parameters();
  for (Decl& p : parms)
    parameter(p);

  // Elaborate the return type.
  Type& ret = type(decl.return_type());

  // Rebuild the function type and update the declaration.
  decl.type_ = &cxt.get_function_type(parms, ret);

  // FIXME: Rewrite expression definitions into function definitions
  // to simplify later analysis and code gen.
  
  // Enter the function scope and recurse through the definition.
  Enter_scope scope(cxt, decl);
  if (Function_def* def = as<Function_def>(&decl.definition()))
    statement(def->statement());
}


// TODO: We should probably be doing more checking here.
//
// TODO: Is this the appropriate place to transform a couroutine
// into a class. Perhaps...
void
Elaborate_declarations::coroutine_declaration(Coroutine_decl &decl)
{
  // Elaborate the parameters.
  Decl_list& parms = decl.parameters();
  for (Decl& p : parms)
    parameter(p);
  
  // Elaborate the return type of the coroutine
  decl.ret_ = &type(decl.type());

  // FIXME: Don't we have to set the coroutine type here?

  Enter_scope scope(cxt, decl);
  if (Function_def* def = as<Function_def>(&decl.definition()))
    statement(def->statement());
}


void
Elaborate_declarations::class_declaration(Class_decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Def& d)       { lingo_unhandled(d); }
    void operator()(Class_def& d) { elab.statement_seq(d.statements()); }
  };

  // Update the class kind/metatype
  d.kind_ = &type(d.kind());

  Enter_scope scope(cxt, d);
  apply(d.definition(), fn{*this});
}


void
Elaborate_declarations::parameter(Decl& p)
{
  parameter(cast<Object_parm>(p));
}


void
Elaborate_declarations::parameter(Object_parm& p)
{
  p.type_ = &type(p.type());
}


// -------------------------------------------------------------------------- //
// Types

// Parse the type as needed, returning its fully elaborated form.
Type&
Elaborate_declarations::type(Type& t)
{
  if (Unparsed_type* u = as<Unparsed_type>(&t)) {
    Save_input_location loc(cxt);
    Token_stream ts(u->tokens());
    Parser p(cxt, ts);
    return p.type();    
  }
  return t;
}


} // namespace banjo

