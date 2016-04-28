// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_TOKEN_HPP
#define BANJO_TOKEN_HPP

#include "prelude.hpp"

#include <lingo/token.hpp>


namespace banjo
{

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
  dot_tok,
  ellipsis_tok,

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
  lt_eq_gt_tok,
  lt_lt_tok,
  gt_gt_tok,
  amp_amp_tok,
  bar_bar_tok,
  bang_tok,
  arrow_tok,
  question_tok,
  dollar_tok,

  // Keywords
  first_keyword_tok,
  abstract_tok,
  axiom_tok,
  auto_tok,
  bool_tok,
  break_tok,
  byte_tok,
  char_tok,
  case_tok,
  class_tok,
  concept_tok,
  const_tok,
  consume_tok,
  continue_tok,
  decltype_tok,
  def_tok,
  default_tok,
  delete_tok,
  do_tok,
  double_tok,
  dynamic_tok,
  else_tok,
  enum_tok,
  explicit_tok,
  export_tok,
  false_tok,
  float_tok,
  for_tok,
  forward_tok,
  if_tok,
  implicit_tok,
  import_tok,
  in_tok,
  inline_tok,
  int_tok,
  mutable_tok,
  namespace_tok,
  operator_tok,
  out_tok,
  public_tok,
  private_tok,
  protected_tok,
  requires_tok,
  return_tok,
  static_tok,
  struct_tok,
  super_tok,
  switch_tok,
  template_tok,
  true_tok,
  typename_tok,
  uint_tok,
  union_tok,
  using_tok,
  var_tok,
  virtual_tok,
  void_tok,
  volatile_tok,
  while_tok,
  last_keyword_tok,

  // Character classes
  identifier_tok,
  integer_tok,
};


// Returns true if k is a keyword.
inline bool
is_keyword(Token_kind k)
{
  return first_keyword_tok < k && k < last_keyword_tok;
}


// Returns true if k is a word (a keyword or identifier).
inline bool
is_word(Token_kind k)
{
  return k == identifier_tok || is_keyword(k);
}

char const* get_spelling(Token_kind);

void init_tokens(Symbol_table&);


} // namespace banjo


#endif
