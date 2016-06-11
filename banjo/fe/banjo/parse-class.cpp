// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>


namespace banjo
{

namespace fe
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
  Match_token_pred end_kind(*this, tk::lbrace_tok);

  require(tk::class_tok);
  Name& name = identifier();

  // Match the metatype.
  Type* kind;
  if (match_if(tk::colon_tok)) {
    if (next_token_is(tk::lbrace_tok))
      kind = &cxt.get_type_type();
    else
      kind = &unparsed_type(end_kind);
  } else {
    kind = &cxt.get_type_type();
  }

  // Point of declaration.
  Decl& decl = start_class_declaration(name, *kind);
  Enter_scope scope(cxt, cxt.saved_scope(decl));

  // Match the class body.
  Def& def = class_body();

  return finish_class_definition(decl, def);
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
  lingo_unreachable();
  
  // Stmt_list ss;
  // match(tk::lbrace_tok);
  // if (lookahead() != tk::rbrace_tok)
  //   ss = member_statement_seq();
  // match(tk::rbrace_tok);
  // return on_class_body(std::move(ss));
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
  } while (lookahead() != tk::rbrace_tok);
  return ss;
}


// A member is a declaration within a class.
//
//    member-statement:
//      member-declaration
//
//    member-declaration:
//      variable-declaration
//      constant-declaration
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
    case tk::super_tok:
    case tk::def_tok:
    case tk::class_tok:
    case tk::template_tok:
      return declaration_statement();
    
    default:
      error("expected member-statement");
      throw Syntax_error();
  }
}


} // namespace fe

} // namespace banjo
