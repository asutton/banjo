// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>


namespace banjo
{

namespace fe
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
    case tk::virtual_tok:
    case tk::abstract_tok:
    case tk::static_tok:
    case tk::inline_tok:
    case tk::explicit_tok:
    case tk::implicit_tok:
    case tk::public_tok:
    case tk::private_tok:
    case tk::protected_tok:
    // Declaration introducers.
    case tk::var_tok:
    case tk::const_tok:
    case tk::def_tok:
    case tk::class_tok:
    case tk::template_tok:
      return declaration_statement();
    
    default:
      error("expected toplevel-statement");
      throw Syntax_error();
  }
}


} // namespace fe

} // namespace banjo
