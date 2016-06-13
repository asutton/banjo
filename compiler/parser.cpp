// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>


namespace banjo
{

namespace fe
{

namespace
{

// Returns a spelling for the current token. If the token
// stream is at the end of input, then the spelling will
// reflect that state.
char const*
token_spelling(Token_stream& ts)
{
  static char end[] = "end-of-input";
  if (ts.eof())
    return end;
  else
    return ts.peek().spelling().c_str();
}

} // namespace


// Return the symbol table.
Symbol_table&
Parser::symbols()
{
  return cxt.symbols();
}


// Returns true if at the end of input.
bool
Parser::is_eof() const
{
  return !peek();
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
  return next_token_is(tk::identifier_tok) && peek().spelling() == s;
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
  if (next_token_is_not(tk::identifier_tok))
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
  error(cxt, "expected '{}' but got '{}'", get_spelling(k), token_spelling(tokens));
  throw Syntax_error();
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


// Require a token of the given kind. Behavior is udefined if the token
// does not match.
Token
Parser::require(Token_kind k)
{
  lingo_assert(lookahead() == k);
  return accept();
}


// Require an identifier matching the spelling of s.
Token
Parser::require(char const* s)
{
  lingo_assert(next_token_is(s));
  return accept();
}


// Emit an error if the next token is not of the kind given. Note that
// this does not consume the token.
void
Parser::expect(Token_kind k)
{
  if (next_token_is_not(k)) {
    error(cxt, "expected '{}' but got '{}'", get_spelling(k), token_spelling(tokens));
    throw Syntax_error();
  }
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
    case tk::lparen_tok:
    case tk::lbrace_tok:
    case tk::lbracket_tok:
      open_brace(tok);
      break;

    case tk::rparen_tok:
    case tk::rbrace_tok:
    case tk::rbracket_tok:
      close_brace(tok);
      break;
  }

  return tok;
}


void
Parser::open_brace(Token tok)
{
  Braces& braces = state.braces;
  braces.open(tok);
}


// Return the the kind of closing brace that would match the closing
// token.
static inline Token_kind
get_closing_brace(Token tok)
{
  using tk = Token_kind;
  switch (tok.kind()) {
    case tk::lparen_tok: return tk::rparen_tok;
    case tk::lbrace_tok: return tk::rbrace_tok;
    case tk::lbracket_tok: return tk::rbracket_tok;
    default: lingo_unreachable();
  }
}



// TODO: It might be nice to have a function that returns the opener
// for a closer.
static inline bool
is_matching_brace(Token left, Token right)
{
  return get_closing_brace(left) == right.kind();
}


void
Parser::close_brace(Token tok)
{
  Braces& braces = state.braces;

  if (braces.empty()) {
    error(cxt, "unmatched brace '{}'", tok);
    throw Syntax_error("mismatched brace");
  }

  Token prev = braces.back();
  if (!is_matching_brace(prev, tok)) {
    // FIXME: show the location of the matching brace.
    error(cxt, "unbalanced brace '{}'", tok);
    throw Syntax_error("unbalanced brace");
  }

  braces.close();
}


// Return true if the current brace nesting level is non-zero. That is,
// we have accepted at least one bracketing character, but not its matching
// closing brace.
bool
Parser::in_braces() const
{
  return !state.braces.empty();
}


// Returns true when the current brace nesting level is n. This is useful
// when checking for closing tokens at a non-empty nesting level (e.g.,
// inside function parameter lists).
bool
Parser::in_level(int n) const
{
  return brace_level() == n;
}


// Returns the current brace nesting level.
int
Parser::brace_level() const
{
  return state.braces.size();
}


// -------------------------------------------------------------------------- //
// Scope management

// Returns the current scope.
Scope&
Parser::current_scope()
{
  return cxt.current_scope();
}


// -------------------------------------------------------------------------- //
// Parsing state


// Returns true if it looks like we're declaring a non-static member
// of a user-defined type.
//
// TODO: Clean this up a bit.  
bool
Parser::parsing_nonstatic_member() const
{
  bool in_udt = is<Class_decl>(cxt.current_context());
  bool is_static = decl_specs() & static_spec;
  return in_udt && !is_static;
}


} // namespace fe

} // namespace banjo
