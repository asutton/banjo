// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "scope.hpp"

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
  error(tokens.location(), msg);
  throw Syntax_error("match");
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
  return tokens.get();
}


// -------------------------------------------------------------------------- //
// Scope management


// Enter the given scope. Unless `s` is the scope of the global
// namespace, `s` must be linked through its enclosing scopes
// to the global namespace.
//
// Do not call this function directly. Use Parser::Scope_sentinel
// to enter a new scope, and guarantee cleanup and scope exit.
void
Parser::set_scope(Scope& s)
{
  state.scope = &s;
}


// Create a new initializer scope.
Scope&
Parser::make_initializer_scope(Decl& d)
{
  return *new Initializer_scope(current_scope(), d);
}


// Create a new function scope.
Scope&
Parser::make_function_scope(Decl& d)
{
  return *new Function_scope(current_scope(), d);
}


// Create a new function parameter scope.
Scope&
Parser::make_function_parameter_scope()
{
  return *new Function_parameter_scope(current_scope());
}


// Create a new template parameter scope.
Scope&
Parser::make_template_parameter_scope()
{
  return *new Template_parameter_scope(current_scope());
}


// Returns the current scope.
Scope&
Parser::current_scope()
{
  return *state.scope;
}


// Find the innermost declaration context. This is the first
// scope associatd with a declaration.
Decl&
Parser::current_context()
{
  Scope* p = &current_scope();
  while (!p->context())
    p = p->enclosing_scope();
  return *p->context();
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
  Enter_scope scope(*this, cxt.global_namespace());
  Decl_list ds;
  if (peek())
    ds = declaration_seq();
  return on_translation_unit(ds);
}


Term&
Parser::operator()()
{
  return translation_unit();
}


} // namespace banjo
