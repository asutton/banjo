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
Symbol*
make_id(char const* n)
{
  return syms.put_identifier(identifier_tok, n);
}


int
main(int argc, char* argv[])
{
  Name* n0 = new Global_id();
  Name* n1 = new Simple_id(make_id("N1"));
  Name* n2 = new Simple_id(make_id("N2"));

  auto* global = new Namespace_decl(n0);
  auto* ns1 = new Namespace_decl(global, n1);
  // auto* ns2 = new Namespace_decl(ns1, n2);

  Name* qn1 = new Qualified_id(global, n1);
  std::cout << "Q1: " << *qn1 << '\n';

  Name* qn2 = new Qualified_id(ns1, n2);
  std::cout << "Q1: " << *qn2 << '\n';

}
