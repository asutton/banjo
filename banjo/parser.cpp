// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast.hpp"

#include <iostream>


namespace banjo
{

namespace
{

// Returns a spelling for the current token. If the token
// stream is at the end of input, then the spelling will
// reflect that state.
String const&
token_spelling(Token_stream& ts)
{
  static String end = "end-of-input";
  if (ts.eof())
    return end;
  else
    return ts.peek().spelling();
}

} // namespace


// Return the symbol table.
Symbol_table&
Parser::symbols()
{
  return cxt.symbols();
}


// Returns the current token.
Token
Parser::peek() const
{
  return tokens.peek();
}


// Returns the first token of lookahead.
Token_kind
Parser::lookahead() const
{
  return Token_kind(peek().kind());
}


// Returns the nth token of lookahead.
Token_kind
Parser::lookahead(int n) const
{
  return Token_kind(tokens.peek(n).kind());
}


// Returns true if the next token has the given kind.
bool
Parser::next_token_is(Token_kind k)
{
  return lookahead() == k;
}


// Returns true if the next token is an identifier with
// the given spelling.
bool
Parser::next_token_is(char const* s)
{
  return next_token_is(identifier_tok) && peek().spelling() == s;
}


// Returns true if the next token does not have the given kind.
bool
Parser::next_token_is_not(Token_kind k)
{
  return lookahead() != k;
}


// Returns true if the next token is not identifier with
// the given spelling.
bool
Parser::next_token_is_not(char const* s)
{
  if (next_token_is_not(identifier_tok))
    return false;
  return peek().spelling() == s;
}


// Require that the next token matches in kind. Emit a diagnostic
// message if it does not.
Token
Parser::match(Token_kind k)
{
  if (lookahead() == k)
    return accept();
  String msg = format("expected '{}' but got '{}'",
                      get_spelling(k),
                      token_spelling(tokens));
  throw Syntax_error(cxt, msg);
}


// If the current token matches k, return the token
// and advance the stream. Otherwise, return an
// invalid token.
//
// Note that invalid tokens evaluate to false.
Token
Parser::match_if(Token_kind k)
{
  if (lookahead() == k)
    return accept();
  else
    return Token();
}


// Require a token of the given kind. Behavior is
// udefined if the token does not match.
Token
Parser::require(Token_kind k)
{
  lingo_assert(lookahead() == k);
  return accept();
}


Token
Parser::require(char const* s)
{
  lingo_assert(next_token_is(s));
  return accept();
}


// Returns the current token and advances the underlying
// token stream.
//
// TODO: Record information about matching braces here.
Token
Parser::accept()
{
  Token tok = tokens.get();

  // Update the global input location.
  cxt.input_location(tok.location());

  // If the token is a brace, then record that for the purpose of
  // brace matching and diagnostics.
  switch (tok.kind()) {
    case lparen_tok:
    case lbrace_tok:
    case lbracket_tok:
      enter_enclosure(tok);
      break;

    case rparen_tok:
    case rbrace_tok:
    case rbracket_tok:
      leave_enclosure(tok);
      break;
  }

  return tok;
}


void
Parser::enter_enclosure(Token tok)
{
  Enclosure& enc = state.enc;
  enc.enter(tok);
}


// TODO: It might be nice to have a function that returns the opener
// for a closer.
static inline bool
is_mismatched_brace(Token left, Token right)
{
  if (left.kind() == lparen_tok)
    return right.kind() != rparen_tok;
  if (left.kind() == lbrace_tok)
    return right.kind() != rbrace_tok;
  if (left.kind() == lbracket_tok)
    return right.kind() != rbracket_tok;
  return false;
}


void
Parser::leave_enclosure(Token tok)
{
  Enclosure& enc = state.enc;
  if (enc.empty()) {
    error(cxt, "unmatched brace '{}'", tok);
    throw Syntax_error("mismatched brace");
  }

  Token top = enc.back();
  if (is_mismatched_brace(top, tok)) {
    // FIXME: show the location of the matching brace.
    error(cxt, "unbalanced brace '{}'", tok);
    throw Syntax_error("unbalanced brace");
  }

  enc.leave();
}


// -------------------------------------------------------------------------- //
// Scope management

// Returns the current scope.
Scope&
Parser::current_scope()
{
  return cxt.current_scope();
}


// Find the innermost declaration context. This is the first
// scope associatd with a declaration.
Decl&
Parser::current_context()
{
  return cxt.current_context();
}


// -------------------------------------------------------------------------- //
// Miscellaneous parsing

// Parse a translation unit.
//
//    translation-unit:
//      [declaration-seq]
//
// FIXME: This currently returns the global namespace, but I'm not
// entirely sure that this is what I want to do. Should we have
Term&
Parser::translation_unit()
{
  Enter_scope scope(cxt, cxt.global_namespace());
  Decl_list ds;

  // NOTE: We match against identifier so that we don't try to
  // parse inspect scripts as real programs. Perhaps we should
  // have a very explicit end-of-program token (e.g., --?).
  if (peek() && lookahead() != identifier_tok)
    ds = declaration_seq();
  return on_translation_unit(ds);
}


Term&
Parser::operator()()
{
  return translation_unit();
}


} // namespace banjo
