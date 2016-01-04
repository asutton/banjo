// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "lexer.hpp"
#include "parser.hpp"

#include <lingo/file.hpp>
#include <lingo/io.hpp>
#include <lingo/error.hpp>

#include <iostream>


using namespace lingo;
using namespace beaker;


int
main(int argc, char* argv[])
{
  init_colors();
  init_tokens();

  if (argc != 2) {
    std::cerr << "usage: beaker-compile <input-file>\n";
    return -1;
  }

  File input(argv[1]);
  Character_stream cs(input);
  Token_stream ts(input);
  Lexer lex(cs, ts);
  Parser parse(ts);

  // Transform characters into tokens.
  lex();
  if (error_count())
    return -1;

  // Transform tokens into a syntax tree.
  Term* unit = parse();
  if (error_count())
    return 1;
  (void)unit;
}
