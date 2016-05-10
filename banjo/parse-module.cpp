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
//    translation-unit:
//      [toplevel-statement-seq]
//
// A translation unit is physically comprised of a sequence of input 
// files. The order in which these files appear in the translation is 
// not consequential, except in some cases.
Stmt&
Parser::translation_unit()
{
  // TODO: We should enter the global scope and not create a temporary.
  Enter_scope scope(cxt);
  Stmt_list ss = toplevel_statement_seq();
  return on_translation_unit(std::move(ss));
}


// Parse a sequence of top-level statements.
//
//    toplevel-statement-seq:
//      toplevel-statement
//      toplevel-statement-seq toplevel-statement
Stmt_list
Parser::toplevel_statement_seq()
{
  Stmt_list ss;
  while (!is_eof()) {
    Stmt& s = toplevel_statement();
    ss.push_back(s);
  }
  return ss;
}


// Parse a toplevel statement.
//
//    toplevel-statement:
//      declaration-statement
//
// TODO: What other kinds of toplevel-statements should we have. Note that
// imports and modules are declarations. 
Stmt&
Parser::toplevel_statement()
{
  return declaration_statement();
}



} // namespace banjo
