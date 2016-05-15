// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "elab-overloads.hpp"
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
Elaborate_overloads::translation_unit(Translation_unit& tu)
{
  Enter_scope(cxt, tu);
  statement_seq(tu.statements());
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
  Enter_scope scope(cxt, s);
  statement_seq(s.statements());
}


void
Elaborate_overloads::declaration_statement(Declaration_stmt& s)
{
  declaration(s.declaration());
}


// -------------------------------------------------------------------------- //
// Declarations


template<typename Iter>
static void
check_declarations(Context& cxt, Decl const& decl, Iter first, Iter last)
{
  bool ok = true;
   while (first != last) {
    try {
      check_declarations(cxt, decl, *first);
    } catch (...) {
      ok = false;
    }
    ++first;
  }
  if (!ok)
    throw Declaration_error();
}


void
Elaborate_overloads::declaration(Decl& decl)
{
  // Lookup the declaration. If it hasn't been found, then it's the
  // first (in a non-saved scope), and we can just add it. Otherwise,
  // we need to check it against other entities in the overload set.
  Name& name = decl.name();
  Overload_set& ovl = *cxt.current_scope().lookup(name);

  // Find the position of the declaration within the overload set.
  auto iter = std::find_if(ovl.begin(), ovl.end(), [&decl](Decl& d) {
    return &decl == &d;
  });
  lingo_assert(iter != ovl.end());
  check_declarations(cxt, decl, ++iter, ovl.end());

  // Otherwise, potentially recurse.
  struct fn
  {
    Self& elab;
    void operator()(Decl& d)          { /* Do nothing. */ }
    void operator()(Function_decl& d) { elab.function_declaration(d); }
    void operator()(Class_decl& d)    { elab.class_declaration(d); }
  };
  apply(decl, fn{*this});
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
    void operator()(Function_def& d)   { elab.statement(d.statement()); }
    void operator()(Expression_def& d) { /* Do nothing. */ }
  };

  Enter_scope scope(cxt, d);
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

  Enter_scope scope(cxt, d);
  apply(d.definition(), fn{*this});
}

} // namespace banjo

