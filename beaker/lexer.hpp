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

// -------------------------------------------------------------------------- //
// Symbols

// The symbol table is a global resource.
//
// TODO: Consider protecting this with a
// a set of accessor functions.
extern Symbol_table symbols;


// -------------------------------------------------------------------------- //
// Tokens

enum Token_kind
{
  // Punctiation
  error_tok = -1,
  lbrace_tok,
  rbrace_tok,
  lparen_tok,
  rparen_tok,
  lbracket_tok,
  rbracket_tok,
  comma_tok,
  colon_tok,
  colon_colon_tok,
  semicolon_tok,

  // Operators
  plus_tok,
  minus_tok,
  star_tok,
  slash_tok,
  percent_tok,
  amp_tok,
  bar_tok,
  caret_tok,
  tilde_tok,
  eq_tok,
  eq_eq_tok,
  bang_eq_tok,
  lt_tok,
  gt_tok,
  lt_eq_tok,
  gt_eq_tok,
  lt_lt_tok,
  gt_gt_tok,
  amp_amp_tok,
  bar_bar_tok,
  bang_tok,
  arrow_tok,
  question_tok,

  // Keywords
  abstract_tok,
  char_tok,
  char8_tok,
  char16_tok,
  char32_tok,
  case_tok,
  struct_tok,
  do_tok,
  dynamic_tok,
  explicit_tok,
  export_tok,
  float_tok,
  float16_tok,
  float32_tok,
  float64_tok,
  float128_tok,
  for_tok,
  if_tok,
  implicit_tok,
  import_tok,
  int_tok,
  int8_tok,
  int16_tok,
  int32_tok,
  int64_tok,
  int128_tok,
  namespace_tok,
  static_tok,
  switch_tok,
  this_tok,
  template_tok,
  uint_tok,
  uint8_tok,
  uint16_tok,
  uint32_tok,
  uint64_tok,
  uint128_tok,
  using_tok,
  virtual_tok,
  while_tok,

  // Character classes
  identifier_tok,
  integer_tok,
};


char const* get_spelling(Token_kind);


// -------------------------------------------------------------------------- //
// Lexing


// The Lexer is a facility that translates sequences of
// characters into tokens. This is primarily a callback
// interface for the lexing function for the language.
struct Lexer
{
  Lexer(Character_stream& cs, Token_stream& ts)
    : cs_(cs), ts_(ts)
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

  Character_stream& cs_;
  Token_stream&     ts_;
  String_builder    buf_;
  Location          loc_;
};


} // namespace beaker


#endif
