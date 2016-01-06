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
  Symbol const* sym = syms.put_identifier(identifier_tok, n);
  return new Simple_id(sym);
}


void
test_names()
{
  Name* n0 = new Global_id();
  Name* n1 = make_id("N1");
  Name* n2 = make_id("N2");

  auto* global = new Namespace_decl(n0);
  auto* ns1 = new Namespace_decl(global, n1);
  auto* ns2 = new Namespace_decl(ns1, n2);

  // TODO: How do I automate these tests?
  Name* qn1 = new Qualified_id(global, n1);
  std::cout << *qn1 << '\n'; // ::N1

  Name* qn2 = new Qualified_id(ns1, n2);
  std::cout << *qn2 << '\n'; // N1::N2

  std::cout << *ns2->qualified_id() << '\n'; // N1::N2
  std::cout << *ns2->fully_qualified_id() << '\n'; // ::N1::N2
}


void
test_types()
{
  Type* t1 = new Void_type();
  std::cout << *t1 << '\n'; // void

  Type* t2 = new Boolean_type();
  std::cout << *t2 << '\n'; // bool

  Type* t3 = new Integer_type();
  std::cout << *t3 << '\n'; // bool

  Type* t4 = new Float_type();
  std::cout << *t4 << '\n'; // bool
}


int
main(int argc, char* argv[])
{
  test_names();
  test_types();
}
