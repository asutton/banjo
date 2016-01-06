// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include <beaker/ast.hpp>
#include <beaker/lexer.hpp>
#include <beaker/print.hpp>

#include <lingo/symbol.hpp>

#include <iostream>


using namespace lingo;
using namespace beaker;


// A symbol table.
Symbol_table syms;


// Create an identifier and return its symbol.
Name*
make_id(char const* n)
{
  Symbol* sym = syms.put_identifier(identifier_tok, n);
  return new Simple_id(sym);
}



Namespace_decl*
make_global_ns()
{
  return new Namespace_decl(new Global_id());
}

Boolean_type*
make_bool_type() { return new Boolean_type(); }


Variable_decl*
make_variable(Decl* cxt, char const* n)
{
  return new Variable_decl(cxt, make_id(n), make_bool_type(), new Default_init());
}


int
main(int argc, char* argv[])
{
  Namespace_decl* global = make_global_ns();
  Variable_decl* v1 = make_variable(global, "v1");

  // FIXME: What exactly are we testing here?
  std::cout << *v1 << '\n';
}
