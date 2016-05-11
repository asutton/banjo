// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-stmt.hpp"

#include <iostream>

namespace banjo
{


// Parse a class definition.
//
//    class-declaration:
//      'class' identifier [':'] class-body
//      'class' identifier [':' type] class-body
//
// NOTE: The parser currently allows the omission of the ':' because it
// looks weird when the kind is not given explicitly omitted.
//
// TODO: We could use '=' notation in bodies to create new derived types.
Decl&
Parser::class_declaration()
{
  Match_token_pred end_kind(*this, lbrace_tok);

  require(class_tok);
  Name& name = identifier();

  // Match the metatype.
  Type* kind;
  if (match_if(colon_tok)) {
    if (next_token_is(lbrace_tok))
      kind = &cxt.get_type_type();
    else
      kind = &unparsed_type(end_kind);
  } else {
    kind = &cxt.get_type_type();
  }

  // Point of declaration.
  Decl& decl = on_class_declaration(name, *kind);
  Enter_scope scope(cxt, cxt.saved_scope(decl));

  // Match the class body.
  Def& def = class_body();
  
  return on_class_definition(decl, def);
};


// Parse a class body, which represents the body of a class.
//
//    class-body:
//      '{' [member-statement-seq] '}'
//
//    member-seq:
//      member
//      member-seq member
//
// Note that that the scope into which declarations are added is pushed 
// prior to the parsing of the member statement.
Def&
Parser::class_body()
{
  Stmt_list ss;
  match(lbrace_tok);
  if (lookahead() != rbrace_tok)
    ss = member_statement_seq();
  match(rbrace_tok);
  return on_class_body(std::move(ss));
}


// Parse a sequence of member statements.
//
//    member-statement-seq:
//      member-statement
//      member-statement-seq member-statement
Stmt_list
Parser::member_statement_seq()
{
  Stmt_list ss;
  do {
    Stmt& s = member_statement();
    ss.push_back(s);
  } while (lookahead() != rbrace_tok);
  return ss;
}


// A member is a declaration within a class.
//
//    member-statement:
//      member-declaration
//
//    member-declaration:
//      variable-declaration
//      super-declaration
//      function-declaration
//      class-declaration
//      template-declaration
//
// TODO: Allow other kinds of statements to support metaprogramming.
Stmt&
Parser::member_statement()
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
    case super_tok:
    case def_tok:
    case class_tok:
    case template_tok:
      return declaration_statement();
    
    default:
      error("expected member-statement");
      throw Syntax_error();
  }
}


} // namespace banjo
