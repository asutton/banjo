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
  Stmt_list ss = parse();
  for (Stmt& s : ss)
    std::cout << s << '\n';


  // // Elaborate entity declarations.
  // unit = process_declarations(cxt, unit);
  //

  // FIXME: This is inelegant. Use scoped resource management.
  for (File* f : inputs)
    delete f;

  //
  // Context cxt;
  //
  // if (argc != 2) {
  //   std::cerr << "usage: test_parse <input-file>\n";
  //   return -1;
  // }
  //
  // File input(argv[1]);
  // Character_stream cs(input);
  // Token_stream ts;
  // Lexer lex(cxt, cs, ts);
  // Parser parse(cxt, ts);
  //
  // try {
  //   // Transform characters into tokens.
  //   lex();
  //   if (error_count())
  //     return -1;
  //
  //   // Transform tokens into a syntax tree.
  //   Term& unit = parse();
  //   if (error_count())
  //     return 1;
  //   std::cout << unit;
  //   return 0;
  // } catch (Compiler_error& err) {
  //   std::cerr << err.what();
  //   return 1;
  // }

}
