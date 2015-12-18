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

// FIXME: Implement this.  Better yet, build out a facility that
// handles initialization and spelling simultaenously.
char const*
get_spelling(Token_kind k)
{
  switch (k) {
    case error_tok: return "<error>";
    case lparen_tok: return "(";
    case rparen_tok: return ")";
    case plus_tok: return "+";
    case minus_tok: return "-";
    case star_tok: return "*";
    case slash_tok: return "/";
    case percent_tok: return "%";
    case integer_tok: return "<integer>";
    default: return "<unspecified>";
  }
}


// Initialize the token set used by the language.
void
init_tokens()
{
  symbols.put_symbol(lbrace_tok, "{");
  symbols.put_symbol(rbrace_tok, "}");
  symbols.put_symbol(lparen_tok, "(");
  symbols.put_symbol(rparen_tok, ")");
  symbols.put_symbol(lbracket_tok, "[]");
  symbols.put_symbol(rbracket_tok, "]");
  symbols.put_symbol(comma_tok, ",");
  symbols.put_symbol(colon_tok, ":");
  symbols.put_symbol(colon_colon_tok, "::");
  symbols.put_symbol(semicolon_tok, ";");

  // Operators
  symbols.put_symbol(plus_tok, "+");
  symbols.put_symbol(minus_tok, "-");
  symbols.put_symbol(star_tok, "*");
  symbols.put_symbol(slash_tok, "/");
  symbols.put_symbol(percent_tok, "%");
  symbols.put_symbol(amp_tok, "&");
  symbols.put_symbol(bar_tok, "|");
  symbols.put_symbol(caret_tok, "^");
  symbols.put_symbol(tilde_tok, "~");
  symbols.put_symbol(eq_tok, "=");
  symbols.put_symbol(eq_eq_tok, "==");
  symbols.put_symbol(bang_eq_tok, "!=");
  symbols.put_symbol(lt_tok, "<");
  symbols.put_symbol(gt_tok, ">");
  symbols.put_symbol(lt_eq_tok, "<=");
  symbols.put_symbol(gt_eq_tok, ">=");
  symbols.put_symbol(lt_lt_tok, "<<");
  symbols.put_symbol(gt_gt_tok, ">>");
  symbols.put_symbol(arrow_tok, "->");
  symbols.put_symbol(question_tok, "?");

  // Keywords
  symbols.put_symbol(abstract_tok, "abstract");
  symbols.put_symbol(char_tok, "char");
  symbols.put_symbol(char8_tok, "char8");
  symbols.put_symbol(char16_tok, "char16");
  symbols.put_symbol(char32_tok, "char32");
  symbols.put_symbol(case_tok, "case");
  symbols.put_symbol(struct_tok, "struct");
  symbols.put_symbol(do_tok, "do");
  symbols.put_symbol(dynamic_tok, "dynamic");
  symbols.put_symbol(explicit_tok, "explicit");
  symbols.put_symbol(export_tok, "export");
  symbols.put_symbol(float_tok, "float");
  symbols.put_symbol(float16_tok, "float16");
  symbols.put_symbol(float32_tok, "float32");
  symbols.put_symbol(float64_tok, "float64");
  symbols.put_symbol(float128_tok, "float128");
  symbols.put_symbol(for_tok, "for");
  symbols.put_symbol(if_tok, "if");
  symbols.put_symbol(implicit_tok, "implicit");
  symbols.put_symbol(import_tok, "import");
  symbols.put_symbol(int_tok, "int");
  symbols.put_symbol(int8_tok, "int8");
  symbols.put_symbol(int16_tok, "int16");
  symbols.put_symbol(int32_tok, "int32");
  symbols.put_symbol(int64_tok, "int64");
  symbols.put_symbol(int128_tok, "int128");
  symbols.put_symbol(namespace_tok, "namespace");
  symbols.put_symbol(static_tok, "static");
  symbols.put_symbol(switch_tok, "switch");
  symbols.put_symbol(this_tok, "this");
  symbols.put_symbol(template_tok, "template");
  symbols.put_symbol(uint_tok, "uint");
  symbols.put_symbol(uint8_tok, "uint8");
  symbols.put_symbol(uint16_tok, "uint16");
  symbols.put_symbol(uint32_tok, "uint32");
  symbols.put_symbol(uint64_tok, "uint64");
  symbols.put_symbol(int128_tok, "int128");
  symbols.put_symbol(using_tok, "using");
  symbols.put_symbol(virtual_tok, "virtual");
  symbols.put_symbol(while_tok, "while");
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

    case '(': get(); return symbol();
    case ')': get(); return symbol();
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
      if (is_alpha(lookahead()))
        return word();
      else if (is_decimal_digit(lookahead()))
        return integer();
      else
        error();
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
