// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lexer.hpp"

#include "lingo/error.hpp"

#include <cassert>
#include <cctype>
#include <string>
#include <iostream>

namespace beaker
{

// -------------------------------------------------------------------------- //
// Symbols

Symbol_table symbols;



// -------------------------------------------------------------------------- //
// Tokens


// A map of token names to their spelling.
//
// TODO: Lift this into lingo.
using Spelling_map = std::unordered_map<Token_kind, char const*>;
Spelling_map spelling;


// Returns te spelling of the given token kind.
char const*
get_spelling(Token_kind k)
{
  return spelling.find(k)->second;
}


void
init_token(Token_kind k, char const* s)
{
  symbols.put_symbol(k, s);
  spelling.emplace(k, s);
}


void
init_token_class(Token_kind k, char const* s)
{
  spelling.emplace(k, s);
}


// Initialize the token set used by the language.
void
init_tokens()
{
  init_token(lbrace_tok, "{");
  init_token(rbrace_tok, "}");
  init_token(lparen_tok, "(");
  init_token(rparen_tok, ")");
  init_token(lbracket_tok, "[");
  init_token(rbracket_tok, "]");
  init_token(comma_tok, ",");
  init_token(colon_tok, ":");
  init_token(colon_colon_tok, "::");
  init_token(semicolon_tok, ";");
  init_token(dot_tok, ".");
  init_token(ellipsis_tok, "...");

  // Operators
  init_token(plus_tok, "+");
  init_token(minus_tok, "-");
  init_token(star_tok, "*");
  init_token(slash_tok, "/");
  init_token(percent_tok, "%");
  init_token(amp_tok, "&");
  init_token(bar_tok, "|");
  init_token(caret_tok, "^");
  init_token(tilde_tok, "~");
  init_token(eq_tok, "=");
  init_token(eq_eq_tok, "==");
  init_token(bang_eq_tok, "!=");
  init_token(lt_tok, "<");
  init_token(gt_tok, ">");
  init_token(lt_eq_tok, "<=");
  init_token(gt_eq_tok, ">=");
  init_token(lt_lt_tok, "<<");
  init_token(gt_gt_tok, ">>");
  init_token(amp_amp_tok, "&&");
  init_token(bar_bar_tok, "||");
  init_token(bang_tok, "!");
  init_token(arrow_tok, "->");
  init_token(question_tok, "?");

  // Keywords
  init_token(abstract_tok, "abstract");
  init_token(auto_tok, "auto");
  init_token(bool_tok, "bool");
  init_token(char_tok, "char");
  init_token(char8_tok, "char8");
  init_token(char16_tok, "char16");
  init_token(char32_tok, "char32");
  init_token(case_tok, "case");
  init_token(class_tok, "class");
  init_token(concept_tok, "concept");
  init_token(const_tok, "const");
  init_token(decltype_tok, "decltype");
  init_token(def_tok, "def");
  init_token(do_tok, "do");
  init_token(double_tok, "double");
  init_token(dynamic_tok, "dynamic");
  init_token(enum_tok, "enum");
  init_token(explicit_tok, "explicit");
  init_token(export_tok, "export");
  init_token(false_tok, "false");
  init_token(float_tok, "float");
  init_token(float16_tok, "float16");
  init_token(float32_tok, "float32");
  init_token(float64_tok, "float64");
  init_token(float128_tok, "float128");
  init_token(for_tok, "for");
  init_token(if_tok, "if");
  init_token(implicit_tok, "implicit");
  init_token(import_tok, "import");
  init_token(int_tok, "int");
  init_token(int8_tok, "int8");
  init_token(int16_tok, "int16");
  init_token(int32_tok, "int32");
  init_token(int64_tok, "int64");
  init_token(int128_tok, "int128");
  init_token(namespace_tok, "namespace");
  init_token(requires_tok, "requires");
  init_token(static_tok, "static");
  init_token(struct_tok, "struct");
  init_token(switch_tok, "switch");
  init_token(this_tok, "this");
  init_token(template_tok, "template");
  init_token(true_tok, "true");
  init_token(typename_tok, "typename");
  init_token(uint_tok, "uint");
  init_token(uint8_tok, "uint8");
  init_token(uint16_tok, "uint16");
  init_token(uint32_tok, "uint32");
  init_token(uint64_tok, "uint64");
  init_token(int128_tok, "int128");
  init_token(union_tok, "union");
  init_token(using_tok, "using");
  init_token(virtual_tok, "virtual");
  init_token(var_tok, "var");
  init_token(void_tok, "void");
  init_token(volatile_tok, "volatile");
  init_token(while_tok, "while");

  init_token_class(identifier_tok, "<identifier>");
  init_token_class(integer_tok, "<integer>");
}



// -------------------------------------------------------------------------- //
// Lexing


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

    case '/':
      // FIXME: Implement '// omments
      get();
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
      } else if (is_decimal_digit(lookahead())) {
        return integer();
      } else {
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


Token
Lexer::word()
{
  letter();
  while (is_alpha(cs_.peek()))
    letter();
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
  Symbol const* sym = symbols.get(buf_.take());
  return Token(loc_, sym);
}


// Try looking up the symbol first. If there is no such
// symbol, then this must be an identifier.
Token
Lexer::on_word()
{
  String str = buf_.take();
  Symbol const* sym = symbols.get(str);
  if (!sym)
    sym = symbols.put_identifier(identifier_tok, str);
  return Token(loc_, sym);
}


Token
Lexer::on_integer()
{
  String str = buf_.take();
  int n = string_to_int<int>(str, 10);
  Symbol* sym = symbols.put_integer(integer_tok, str, n);
  return Token(loc_, sym);
}


void
Lexer::operator()()
{
  while (Token tok = scan())
    ts_.put(tok);
}


} // namespace beaker
