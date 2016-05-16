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
Decl&
Parser::translation_unit()
{
  // Create the translation unit and its associated global scope.
  Decl& tu = start_translation_unit();
  Enter_scope scope(cxt, cxt.saved_scope(tu));
  
  Stmt_list ss = toplevel_statement_seq();
  return finish_translation_unit(tu, std::move(ss));
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
  switch (lookahead()) {
    // Declaration specifiers.
    case virtual_tok:
    case abstract_tok:
    case static_tok:
    case inline_tok:
    case explicit_tok:
    case implicit_tok:
    case public_tok:
    case private_tok:
    case protected_tok:
    // Declaration introducers.
    case var_tok:
    case const_tok:
    case def_tok:
    case class_tok:
    case template_tok:
      return declaration_statement();
    
    default:
      error("expected toplevel-statement");
      throw Syntax_error();
  }
}



} // namespace banjo
