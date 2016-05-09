// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-stmt.hpp"

#include <iostream>

namespace banjo
{

// Parse a translation unit.
//
//    translation:
//      [directive-list]
//
// FIXME: Rethink the program structure for the language. In particular,
// we should prefer to think in terms of program fragments. Fragments can
// be combined to define libraries (archives?), modules, and programs.
// This can probably be achieved by simply renaming Translation_stmt to
// Fragment_stmt, although it would probably be nice to revisit the design 
// of compound statements in general.
Stmt&
Parser::translation()
{
  // TODO: We should enter the global scope and not create a temporary.
  Enter_scope scope(cxt);

  Stmt_list ss;
  while (!is_eof())
    Stmt& s = directive();
    ss.push_back(s);
  }

  return on_translation_statement(std::move(ss));
}


// Parse a directive.
//
//    directive:
//      include-directive     -- Probably?
//      pragma-directive      -- Maybe?
//      declaration-directive
//
//    declaration-directive:
//      declaration
//
// A directive is a top-level statement in a program fragment. This includes 
// information defining properties of the eventual module or program, 
// including its declarations.
//
// Note that directives are set of statements that can appear only in the
// global scope.
//
// TODO: What kinds of directives can we have? Note that imports and
// modules are declarations. Maybe pragma-like things? What about 
// metaprogamming facilities?
Stmt&
Parser::directive()
{
  Decl& d = declaration();
  return on_declaration_directive(d);
}



} // namespace banjo
