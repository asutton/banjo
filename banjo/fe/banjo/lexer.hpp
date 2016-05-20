// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_FE_LEXER_HPP
#define BANJO_FE_LEXER_HPP

#include "token.hpp"

#include <lingo/symbol.hpp>
#include <lingo/token.hpp>
#include <lingo/character.hpp>


namespace banjo
{

namespace fe
{

// The Lexer is a facility that translates sequences of characters into 
// a sequence of tokens. 
//
// TODO: Save comments during lexing.
struct Lexer
{
  // Abbreviate token kinds.
  using tk = Token_kind;

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
  void comment();
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


} // namespace fe

} // namespace banjo


#endif
