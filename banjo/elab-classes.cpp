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
Parser::elaborate_classes(Stmt_list& ss)
{
  for (Stmt& s : ss)
    elaborate_classes(s);
}


void
Parser::elaborate_classes(Stmt& s)
{
  if (Declaration_stmt* s1 = as<Declaration_stmt>(&s))
    elaborate_class(s1->declaration());
  
  if (Compound_stmt* s1 = as<Compound_stmt>(&s))
    elaborate_classes(s1->statements());
}


void
Parser::elaborate_class(Decl& d)
{
  struct fn
  {
    Parser& p;
    void operator()(Decl& d)       { }
    void operator()(Class_decl& d) { p.elaborate_class(d); }
  };
  apply(d, fn{*this});
}


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


// Actually, elaborate the definition.
void
Parser::elaborate_class(Class_decl& decl)
{
  // FIXME: Elaborate the class kind.

  // Partition the statements into bases and fields.
  Class_def& def = cast<Class_def>(decl.definition());
  for (Stmt& s : def.statements())
    partition_members(def, s);

  // TODO: By the time this function completes, all compile-time properties
  // of the class must be known (e.g., size, layout, alignment, etc).
}



} // namespace banjo
