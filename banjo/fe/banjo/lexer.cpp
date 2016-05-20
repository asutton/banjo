// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lexer.hpp"
#include "context.hpp"

#include "lingo/error.hpp"

#include <cassert>
#include <cctype>
#include <string>
#include <iostream>

namespace banjo
{

namespace fe
{

Symbol_table&
Lexer::symbols()
{
  return cxt_.symbols();
}


// Copy the current character into the save buffer.
void
Lexer::get()
{
  buf_.put(cs_.get());
}


char
Lexer::lookahead() const
{
  return cs_.peek();
}


// Lexically analyze a single token.
Token
Lexer::scan()
{
  while (!cs_.eof()) {
    space();

    loc_ = cs_.location();
    switch (lookahead()) {
    case '\0': return eof();

    case '{': get(); return symbol();
    case '}': get(); return symbol();
    case '(': get(); return symbol();
    case ')': get(); return symbol();
    case '[': get(); return symbol();
    case ']': get(); return symbol();
    case ',': get(); return symbol();

    case ':':
      get();
      if (lookahead() == ':')
        get();
      return symbol();

    case ';': get(); return symbol();

    case '.':
      get();
      if (lookahead() == '.') {
        get();

        // FIXME: The diagnosis of this error is wrong.
        // We should diagnose the occurrence of "..".
        if (lookahead() == '.') {
          get();
        } else {
          error();
          continue;
        }
      }
      return symbol();

    case '+': get(); return symbol();

    case '-':
      get();
      if (lookahead() == '>')
        get();
      return symbol();

    case '*': get(); return symbol();
    case '%': get(); return symbol();

    case '/':
      get();
      if (lookahead() == '/') {
        get();
        comment();
        continue;
      }

      return symbol();

    case '&':
      get();
      if (lookahead() == '&')
        get();
      return symbol();

    case '|':
      get();
      if (lookahead() == '|')
        get();
      return symbol();

    case '^': get(); return symbol();
    case '~': get(); return symbol();

    case '=':
      get();
      if (lookahead() == '=')
        get();
      return symbol();

    case '!':
      get();
      if (lookahead() == '=')
        get();
      return symbol();

    case '<':
      get();
      if (lookahead() == '=' || lookahead() == '<')
        get();
      return symbol();

    case '>':
      get();
      if (lookahead() == '=' || lookahead() == '>')
        get();
      return symbol();

    default:
      // FIXME: Handle underscores in identifiers.
      if (is_alpha(lookahead())) {
        return word();
      } 
      else if (is_decimal_digit(lookahead())) {
        return integer();
      } 
      else {
        error();
        continue;
      }
    }
  }
  return {};
}


void
Lexer::error()
{
  lingo::error(loc_, "unrecognized character '{}'", cs_.get());
}


void
Lexer::space()
{
  while (is_space(cs_.peek()))
    cs_.ignore();
}


// Consume all characters through the end of line. Clear the character
// cache before continuing.
void
Lexer::comment()
{
  while (lookahead() != '\n')
    cs_.ignore();
  buf_.clear();
}


Token
Lexer::eof()
{
  return Token{};
}


Token
Lexer::symbol()
{
  // Nothing to do here... we've already buffered all of
  // the characters for the symbol.
  return on_symbol();
}


// digit ::= [0-9]
void
Lexer::digit()
{
  assert(is_decimal_digit(cs_.peek()));
  get();
}


// letter ::= [a-z][A-Z]
//
// FIXME: Make this handle identifiers.
void
Lexer::letter()
{
  assert(is_alpha(cs_.peek()));
  get();
}


inline bool
is_identifier_alpha(char c)
{
  return is_alpha(c) || is_decimal_digit(c) || c == '_';
}


Token
Lexer::word()
{
  letter();
  while (is_identifier_alpha(lookahead()))
    get();
  return on_word();
}


// FIXME: Rewrite this to handle general numbers.
Token
Lexer::integer()
{
  digit();
  while (is_decimal_digit(cs_.peek()))
    digit();
  return on_integer();
}


Token
Lexer::on_symbol()
{
  Symbol const* sym = symbols().get(buf_.take());
  return Token(loc_, sym);
}


// Try looking up the symbol first. If there is no such
// symbol, then this must be an identifier.
Token
Lexer::on_word()
{
  String str = buf_.take();
  Symbol const* sym = symbols().get(str);
  if (!sym)
    sym = symbols().put_identifier(tk::identifier_tok, str);
  return Token(loc_, sym);
}


Token
Lexer::on_integer()
{
  String str = buf_.take();
  int n = string_to_int<int>(str, 10);
  Symbol* sym = symbols().put_integer(tk::integer_tok, str, n);
  return Token(loc_, sym);
}


void
Lexer::operator()()
{
  while (Token tok = scan())
    ts_.put(tok);
}


} // namespace fe

} // namespace banjo
