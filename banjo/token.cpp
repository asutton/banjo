// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "token.hpp"

namespace banjo
{


// NOTE: Apparently GCC-4.9 does not provide a default hash
// function  for scalar types.
struct Token_kind_hash
{
  std::size_t operator()(Token_kind k) const
  {
    using Int = std::underlying_type<Token_kind>::type;
    std::hash<Int> h;
    return h(k);
  }
};


// A map of token names to their spelling.
//
// TODO: Lift this into lingo. Maybe make it part of the
// symbol table.
using Spelling_map = std::unordered_map<Token_kind, char const*, Token_kind_hash>;
Spelling_map spelling;


// Returns te spelling of the given token kind.
char const*
get_spelling(Token_kind k)
{
  return spelling.find(k)->second;
}


void
init_token(Symbol_table& syms, Token_kind k, char const* s)
{
  syms.put_symbol(k, s);
  spelling.emplace(k, s);
}


void
init_token_class(Symbol_table& syms, Token_kind k, char const* s)
{
  spelling.emplace(k, s);
}


// Initialize the token set used by the language.
void
init_tokens(Symbol_table& syms)
{
  init_token(syms, lbrace_tok, "{");
  init_token(syms, rbrace_tok, "}");
  init_token(syms, lparen_tok, "(");
  init_token(syms, rparen_tok, ")");
  init_token(syms, lbracket_tok, "[");
  init_token(syms, rbracket_tok, "]");
  init_token(syms, comma_tok, ",");
  init_token(syms, colon_tok, ":");
  init_token(syms, colon_colon_tok, "::");
  init_token(syms, semicolon_tok, ";");
  init_token(syms, dot_tok, ".");
  init_token(syms, ellipsis_tok, "...");

  // Operators
  init_token(syms, plus_tok, "+");
  init_token(syms, minus_tok, "-");
  init_token(syms, star_tok, "*");
  init_token(syms, slash_tok, "/");
  init_token(syms, percent_tok, "%");
  init_token(syms, amp_tok, "&");
  init_token(syms, bar_tok, "|");
  init_token(syms, caret_tok, "^");
  init_token(syms, tilde_tok, "~");
  init_token(syms, eq_tok, "=");
  init_token(syms, eq_eq_tok, "==");
  init_token(syms, bang_eq_tok, "!=");
  init_token(syms, lt_tok, "<");
  init_token(syms, gt_tok, ">");
  init_token(syms, lt_eq_tok, "<=");
  init_token(syms, gt_eq_tok, ">=");
  init_token(syms, lt_eq_gt_tok, "<=>");
  init_token(syms, lt_lt_tok, "<<");
  init_token(syms, gt_gt_tok, ">>");
  init_token(syms, amp_amp_tok, "&&");
  init_token(syms, bar_bar_tok, "||");
  init_token(syms, bang_tok, "!");
  init_token(syms, arrow_tok, "->");
  init_token(syms, question_tok, "?");
  init_token(syms, dollar_tok, "$");

  // Keywords
  init_token(syms, abstract_tok, "abstract");
  init_token(syms, axiom_tok, "axiom");
  init_token(syms, auto_tok, "auto");
  init_token(syms, bool_tok, "bool");
  init_token(syms, break_tok, "break");
  init_token(syms, byte_tok, "byte");
  init_token(syms, char_tok, "char");
  init_token(syms, case_tok, "case");
  init_token(syms, class_tok, "class");
  init_token(syms, concept_tok, "concept");
  init_token(syms, const_tok, "const");
  init_token(syms, consume_tok, "consume");
  init_token(syms, continue_tok, "continue");
  init_token(syms, decltype_tok, "decltype");
  init_token(syms, def_tok, "def");
  init_token(syms, default_tok, "default");
  init_token(syms, delete_tok, "delete");
  init_token(syms, do_tok, "do");
  init_token(syms, double_tok, "double");
  init_token(syms, dynamic_tok, "dynamic");
  init_token(syms, else_tok, "else");
  init_token(syms, enum_tok, "enum");
  init_token(syms, explicit_tok, "explicit");
  init_token(syms, export_tok, "export");
  init_token(syms, false_tok, "false");
  init_token(syms, float_tok, "float");
  init_token(syms, for_tok, "for");
  init_token(syms, forward_tok, "forward");
  init_token(syms, if_tok, "if");
  init_token(syms, implicit_tok, "implicit");
  init_token(syms, import_tok, "import");
  init_token(syms, in_tok, "in");
  init_token(syms, inline_tok, "inline");
  init_token(syms, int_tok, "int");
  init_token(syms, mutable_tok, "mutable");
  init_token(syms, namespace_tok, "namespace");
  init_token(syms, operator_tok, "operator");
  init_token(syms, out_tok, "out");
  init_token(syms, public_tok, "public");
  init_token(syms, private_tok, "private");
  init_token(syms, protected_tok, "protected");
  init_token(syms, requires_tok, "requires");
  init_token(syms, return_tok, "return");
  init_token(syms, static_tok, "static");
  init_token(syms, struct_tok, "struct");
  init_token(syms, super_tok, "super");
  init_token(syms, switch_tok, "switch");
  init_token(syms, template_tok, "template");
  init_token(syms, true_tok, "true");
  init_token(syms, typename_tok, "typename");
  init_token(syms, uint_tok, "uint");
  init_token(syms, union_tok, "union");
  init_token(syms, using_tok, "using");
  init_token(syms, virtual_tok, "virtual");
  init_token(syms, var_tok, "var");
  init_token(syms, void_tok, "void");
  init_token(syms, volatile_tok, "volatile");
  init_token(syms, while_tok, "while");

  init_token_class(syms, identifier_tok, "<identifier>");
  init_token_class(syms, integer_tok, "<integer>");
}


} // namespace banjo
