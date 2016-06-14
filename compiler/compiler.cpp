// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "printer.hpp"

#include "elab-declarations.hpp"
#include "elab-expressions.hpp"
// #include "elab-classes.hpp"

#include <banjo/ast.hpp>

#include <codegen/generator.hpp>

#include <lingo/file.hpp>
#include <lingo/io.hpp>
#include <lingo/error.hpp>

#include <iostream>


using namespace lingo;
using namespace banjo;


using File_seq = std::vector<File*>;


struct Options
{
  ~Options();

  String   emit    = "llvm";
  File_seq inputs  = {};
};


Options::~Options()
{
  for (File* f : inputs)
    delete f;
}


using Parse_fn = void (*)(int&, int, char**, Options&);
using Options_map = std::unordered_map<String, Parse_fn>;


void
parse_emit(int& argn, int argc, char* argv[], Options& opts)
{
  if (argn == argc) {
    error("expected one of 'banjo|cxx|llvm' after '-emit'");
    exit(1);
  }
  opts.emit = argv[++argn];
}


void
parse_positional(int& argn, int argc, char* argv[], Options& opts)
{
  opts.inputs.push_back(new File(argv[argn]));
}


void
parse_args(int argc, char* argv[], Options& opts)
{
  static Options_map all {
    {"-emit", parse_emit}
  };


  for (int i = 1; i < argc; ++i) {
    char const* arg = argv[i];
    if (arg[0] == '-') {
      auto iter = all.find(arg);
      if (iter == all.end()) {
        error("unknown option '{}'", argv[i]);
        exit(1);
      }
      iter->second(i, argc, argv, opts);
    } else {
      parse_positional(i, argc, argv, opts);
    }
  }
}



int
main(int argc, char* argv[])
{
  Symbol_table syms;
  fe::Context cxt(syms);

  Options opts;
  parse_args(argc, argv, opts);

  // Check post-configuration options.
  if (opts.inputs.empty()) {
    error("no input files given");
    return -1;
  }

  // Initial file processing.

  // Perform character and lexical analysis.
  Token_seq toks;
  for (File* f : opts.inputs) {
    Character_stream cs(*f);
    Token_stream ts;
    fe::Lexer lex(cxt, cs, ts);

    // Lex the file and splice its tokens into the input stream.
    lex();
    if (error_count())
      return 1;
    toks.splice(toks.end(), ts.buf_);
  }

  // Perform syntactic analysis.
  Token_stream ts(toks);
  fe::Parser parse(cxt, ts);
  parse();

  // Elaboration passes.
  fe::elaborate<fe::Elaborate_declarations>(parse);
  fe::elaborate<fe::Elaborate_expressions>(parse);

  // Elaborate_overloads    overloads(*this);
  // Elaborate_classes      classes(*this);
  // Elaborate_expressions  expressions(*this);


  // // TODO: Transform abbreviated templates into templates.

  // overloads(tu);    // Analyze overloaded/reopened declarations
  // classes(tu);      // Complete class definitions
  // expressions(tu);  // Update expressions


  if (opts.emit == "tokens") {
    for (Token k : toks)
      std::cout << k << ' ';
    std::cout << '\n';
  }
  else if (opts.emit == "banjo") {
    std::cout << cxt.translation_unit() << '\n';
  }
  else if (opts.emit == "llvm") {
    std::cout << "-- parsed --\n";
    std::cout << cxt.translation_unit() << '\n';
    std::cout << "-- generating --\n";
    ll::Generator gen(cxt);
    gen(cxt.translation_unit());
  }
}
