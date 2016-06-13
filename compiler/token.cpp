// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "token.hpp"


namespace banjo
{

namespace fe
{

// NOTE: Apparently GCC-4.9 does not provide a default hash for all
// integral types.
struct Token_kind_hash
{
  std::size_t operator()(Token_kind k) const
  {
    std::hash<int> h;
    return h(k.kind);
  }
};


// A map of token names to their spelling.
//
// TODO: Lift this into lingo. Maybe make it part of the symbol table.
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
  using tk = Token_kind;

  init_token(syms, tk::lbrace_tok, "{");
  init_token(syms, tk::rbrace_tok, "}");
  init_token(syms, tk::lparen_tok, "(");
  init_token(syms, tk::rparen_tok, ")");
  init_token(syms, tk::lbracket_tok, "[");
  init_token(syms, tk::rbracket_tok, "]");
  init_token(syms, tk::comma_tok, ",");
  init_token(syms, tk::colon_tok, ":");
  init_token(syms, tk::colon_colon_tok, "::");
  init_token(syms, tk::semicolon_tok, ";");
  init_token(syms, tk::dot_tok, ".");
  init_token(syms, tk::ellipsis_tok, "...");

  // Operators
  init_token(syms, tk::plus_tok, "+");
  init_token(syms, tk::minus_tok, "-");
  init_token(syms, tk::star_tok, "*");
  init_token(syms, tk::slash_tok, "/");
  init_token(syms, tk::percent_tok, "%");
  init_token(syms, tk::amp_tok, "&");
  init_token(syms, tk::bar_tok, "|");
  init_token(syms, tk::caret_tok, "^");
  init_token(syms, tk::tilde_tok, "~");
  init_token(syms, tk::eq_tok, "=");
  init_token(syms, tk::eq_eq_tok, "==");
  init_token(syms, tk::bang_eq_tok, "!=");
  init_token(syms, tk::lt_tok, "<");
  init_token(syms, tk::gt_tok, ">");
  init_token(syms, tk::lt_eq_tok, "<=");
  init_token(syms, tk::gt_eq_tok, ">=");
  init_token(syms, tk::lt_eq_gt_tok, "<=>");
  init_token(syms, tk::lt_lt_tok, "<<");
  init_token(syms, tk::gt_gt_tok, ">>");
  init_token(syms, tk::amp_amp_tok, "&&");
  init_token(syms, tk::bar_bar_tok, "||");
  init_token(syms, tk::bang_tok, "!");
  init_token(syms, tk::arrow_tok, "->");
  init_token(syms, tk::question_tok, "?");
  init_token(syms, tk::dollar_tok, "$");

  // Keywords
  init_token(syms, tk::abstract_tok, "abstract");
  init_token(syms, tk::axiom_tok, "axiom");
  init_token(syms, tk::auto_tok, "auto");
  init_token(syms, tk::bool_tok, "bool");
  init_token(syms, tk::break_tok, "break");
  init_token(syms, tk::byte_tok, "byte");
  init_token(syms, tk::char_tok, "char");
  init_token(syms, tk::case_tok, "case");
  init_token(syms, tk::class_tok, "class");
  init_token(syms, tk::concept_tok, "concept");
  init_token(syms, tk::const_tok, "const");
  init_token(syms, tk::consume_tok, "consume");
  init_token(syms, tk::continue_tok, "continue");
  init_token(syms, tk::decltype_tok, "decltype");
  init_token(syms, tk::def_tok, "def");
  init_token(syms, tk::default_tok, "default");
  init_token(syms, tk::delete_tok, "delete");
  init_token(syms, tk::do_tok, "do");
  init_token(syms, tk::double_tok, "double");
  init_token(syms, tk::dynamic_tok, "dynamic");
  init_token(syms, tk::else_tok, "else");
  init_token(syms, tk::enum_tok, "enum");
  init_token(syms, tk::explicit_tok, "explicit");
  init_token(syms, tk::export_tok, "export");
  init_token(syms, tk::false_tok, "false");
  init_token(syms, tk::float_tok, "float");
  init_token(syms, tk::for_tok, "for");
  init_token(syms, tk::forward_tok, "forward");
  init_token(syms, tk::if_tok, "if");
  init_token(syms, tk::implicit_tok, "implicit");
  init_token(syms, tk::import_tok, "import");
  init_token(syms, tk::in_tok, "in");
  init_token(syms, tk::inline_tok, "inline");
  init_token(syms, tk::int_tok, "int");
  init_token(syms, tk::meta_tok, "meta");
  init_token(syms, tk::mutable_tok, "mutable");
  init_token(syms, tk::namespace_tok, "namespace");
  init_token(syms, tk::operator_tok, "operator");
  init_token(syms, tk::out_tok, "out");
  init_token(syms, tk::public_tok, "public");
  init_token(syms, tk::private_tok, "private");
  init_token(syms, tk::protected_tok, "protected");
  init_token(syms, tk::requires_tok, "requires");
  init_token(syms, tk::return_tok, "return");
  init_token(syms, tk::static_tok, "static");
  init_token(syms, tk::struct_tok, "struct");
  init_token(syms, tk::super_tok, "super");
  init_token(syms, tk::switch_tok, "switch");
  init_token(syms, tk::template_tok, "template");
  init_token(syms, tk::true_tok, "true");
  init_token(syms, tk::typename_tok, "typename");
  init_token(syms, tk::uint_tok, "uint");
  init_token(syms, tk::union_tok, "union");
  init_token(syms, tk::using_tok, "using");
  init_token(syms, tk::virtual_tok, "virtual");
  init_token(syms, tk::var_tok, "var");
  init_token(syms, tk::void_tok, "void");
  init_token(syms, tk::volatile_tok, "volatile");
  init_token(syms, tk::yield_tok, "yield");
  init_token(syms, tk::while_tok, "while");

  init_token_class(syms, tk::identifier_tok, "identifier");
  init_token_class(syms, tk::integer_tok, "integer");
}


} // namespace fe

} // namespace banjo
