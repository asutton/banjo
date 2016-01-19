// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_LEXER_HPP
#define BEAKER_LEXER_HPP

#include "prelude.hpp"

#include <lingo/symbol.hpp>
#include <lingo/token.hpp>
#include <lingo/character.hpp>


namespace beaker
{

struct Context;


// The Lexer is a facility that translates sequences of
// characters into tokens. This is primarily a callback
// interface for the lexing function for the language.
//
// TODO: Make this take a context instead of just the symbol
// table? That would allow us to pass configuration information
// and diagnostics into the lexer.
struct Lexer
{
  Lexer(Context& cxt, Character_stream& cs, Token_stream& ts)
    : cxt_(cxt), cs_(cs), ts_(ts)
  { }

  void operator()();

  // Scanners
  Token scan();
  Token eof();
  Token symbol();
  Token word();
  Token integer();

  // Consumers
  void error();
  void space();
  void letter();
  void digit();

  // Semantic actions.
  Token on_symbol();
  Token on_word();
  Token on_integer();

  char lookahead() const;
  void get();

  Symbol_table& symbols();

  Context&          cxt_;
  Character_stream& cs_;
  Token_stream&     ts_;
  String_builder    buf_;
  Location          loc_;
};


} // namespace beaker


#endif
