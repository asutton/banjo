
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "elab-classes.hpp"
#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{

 Elaborate_classes::Elaborate_classes(Parser& p)
  : parser(p), cxt(p.cxt)
{ }


// -------------------------------------------------------------------------- //
// Statements

void
Elaborate_classes::translation_unit(Translation_unit& s)
{
  statement_seq(s.statements());
}


void
Elaborate_classes::statement(Stmt& s)
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
Elaborate_classes::statement_seq(Stmt_list& ss)
{
  for (Stmt& s : ss)
    statement(s);
}


void
Elaborate_classes::compound_statement(Compound_stmt& s)
{
  statement_seq(s.statements());
}


void
Elaborate_classes::declaration_statement(Declaration_stmt& s)
{
  declaration(s.declaration());
}


// -------------------------------------------------------------------------- //
// Declarations

void
Elaborate_classes::declaration(Decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Decl& d)          { /* Do nothing. */ }
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
Elaborate_classes::function_declaration(Function_decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Def& d)            { lingo_unhandled(d); }
    void operator()(Expression_def& d) { /* Do nothing. */ }
    void operator()(Function_def& d)   { elab.statement(d.statement()); }
  };

  // Declare parameters. We do this for decltypes.
  Enter_scope scope(cxt);
  for (Decl& p : d.parameters())
    declare(cxt, p);

  apply(d.definition(), fn{*this});
}


// Helper functions for class analysis.
namespace
{

// Remember the field declaration.
void
save_field(Class_def& def, Field_decl& d)
{
  def.objs_.push_back(d);
}


// Remember the base class declaration.
//
// TODO: Should we save super-decls as fields? That would allow
// very precise control over where the base sub-object is placed
// in the class. If it matters. I don't think it does for now.
void
save_base(Class_def& def, Super_decl& d)
{
  def.bases_.push_back(d);
}


// Put members into their corresponding buckets for subsequent analysis.
//
// TODO: Require complete types for bases?
void 
partition_members(Class_def& def, Decl& d)
{
  struct fn
  {
    Class_def& def;
    void operator()(Decl& d)          { lingo_unhandled(d); }
    void operator()(Field_decl& d)    { save_field(def, d); } 
    void operator()(Super_decl& d)    { save_base(def, d); } 
    void operator()(Variable_decl& d) { /* Do nothing. */ } 
    void operator()(Function_decl& d) { /* Do nothing. */ } 
    void operator()(Class_decl& d)    { /* Do nothing. */ } 
  };
  apply(d, fn{def});
}


// FIXME: Save the metaprogram for later.
void 
partition_members(Class_def& def, Stmt& s)
{
  struct fn
  {
    Class_def& def;
    void operator()(Stmt& s) { }
    void operator()(Declaration_stmt& s) { partition_members(def, s.declaration()); } 
  };
  apply(s, fn{def});
}


} // namespace



// TODO: What if the definition is something other than a typical class 
// definition. If so, we should probably apply that transformation here.
void
Elaborate_classes::class_declaration(Class_decl& decl)
{
  // FIXME: Do something with the metatype.

  // Partition the statements into bases and fields.
  Class_def& def = cast<Class_def>(decl.definition());
  for (Stmt& s : def.statements())
    partition_members(def, s);

  // TODO: By the time this function completes, all compile-time properties
  // of the class must be known (e.g., size, layout, alignment, etc).
}


} // namespace banjo

