// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "printer.hpp"

#include <lingo/file.hpp>
#include <lingo/io.hpp>
#include <lingo/error.hpp>

#include <iostream>


using namespace lingo;
using namespace banjo;


int
main(int argc, char* argv[])
{
  Context cxt;

  if (argc != 2) {
    std::cerr << "usage: test_parse <input-file>\n";
    return -1;
  }

  File input(argv[1]);
  Character_stream cs(input);
  Token_stream ts;
  Lexer lex(cxt, cs, ts);
  Parser parse(cxt, ts);

  try {
    // Transform characters into tokens.
    lex();
    if (error_count())
      return -1;

    // Transform tokens into a syntax tree.
    Term& unit = parse();
    if (error_count())
      return 1;
    std::cout << unit;
    return 0;
  } catch (Compiler_error& err) {
    std::cerr << err.what();
    return 1;
  }

}
