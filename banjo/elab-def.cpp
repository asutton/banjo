// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"

#include <iostream>


namespace banjo
{

// Elaborate the type of each declaration in turn. Note that elaboration
// and "skip forward" if the type of one declaration depends on the type
// or definition of another defined after it.
void
Parser::elaborate_definitions(Stmt_list& ss)
{
  for (Stmt& s : ss) {
    elaborate_definition(s);
  }
}

// If the statement is a declaration, elaborate its declared type.
void
Parser::elaborate_definition(Stmt& s)
{
  if (Declaration_stmt* s1 = as<Declaration_stmt>(&s))
    elaborate_definition(s1->declaration());
}


void
Parser::elaborate_definition(Decl& d)
{
  struct fn
  {
    Parser& p;
    void operator()(Decl& d) { lingo_unhandled(d); }
    void operator()(Variable_decl& d) { p.elaborate_variable_definition(d); }
    void operator()(Function_decl& d) { p.elaborate_function_definition(d); }
    void operator()(Type_decl& d)     { p.elaborate_type_definition(d); }
  };
  apply(d, fn{*this});
}


void
Parser::elaborate_variable_definition(Variable_decl& d)
{
  std::cout << "VAR DEF: " << d << '\n';
}


void
Parser::elaborate_function_definition(Function_decl& d)
{
  std::cout << "FN DEF: " << d << '\n';
}


void
Parser::elaborate_type_definition(Type_decl& d)
{
  std::cout << "TYPE DEF: " << d << '\n';
}


} // namespace banjo
