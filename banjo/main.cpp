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


using File_seq = std::vector<File*>;


int
main(int argc, char* argv[])
{
  Context cxt;

  // Parse arguments and collect inputs.
  File_seq inputs;
  for (int i = 1; i < argc; ++i) {
    String s = argv[i];
    if (s[0] == '-') {
      error("unknown option '{}'", s);
    }
    inputs.emplace_back(new File(s));
  }

  if (inputs.empty()) {
    error("no input files given");
    return -1;
  }

  // Initial file processing.

  // Perform character and lexical analysis.
  Token_seq toks;
  for (File* f : inputs) {
    Character_stream cs(*f);
    Token_stream ts;
    Lexer lex(cxt, cs, ts);

    // Lex tokens.
    lex();
    if (error_count())
      return 1;
    toks.splice(toks.end(), ts.buf_);
  }

  // Perform syntactic analysis.
  Token_stream ts(toks);
  Parser parse(cxt, ts);
  Stmt& stmt = parse();
  std::cout << stmt << '\n';


  // FIXME: This is inelegant. Use scoped resource management.
  for (File* f : inputs)
    delete f;
}
