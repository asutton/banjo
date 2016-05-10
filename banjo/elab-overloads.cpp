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
}


} // namespace banjo
