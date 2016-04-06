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
Parser::elaborate_declarations(Stmt_list& ss)
{
  for (Stmt& s : ss) {
    elaborate_declaration(s);
  }
}

// If the statement is a declaration, elaborate its declared type.
void
Parser::elaborate_declaration(Stmt& s)
{
  if (Declaration_stmt* s1 = as<Declaration_stmt>(&s))
    elaborate_declaration(s1->declaration());
}


void
Parser::elaborate_declaration(Decl& d)
{
  struct fn
  {
    Parser& p;
    void operator()(Decl& d) { lingo_unhandled(d); }
    void operator()(Variable_decl& d) { p.elaborate_variable_declaration(d); }
    void operator()(Function_decl& d) { p.elaborate_function_declaration(d); }
    void operator()(Type_decl& d)     { p.elaborate_type_declaration(d); }
  };
  apply(d, fn{*this});
}


// If the type is unparsed, parse that now.
void
Parser::elaborate_variable_declaration(Variable_decl& d)
{
  if (Unparsed_type* t = as<Unparsed_type>(&d.type())) {
    Save_input_location loc(cxt);
    Token_stream ts(t->tokens());
    Parser parse(cxt, ts);
    Type& type = parse.type();
    d.type_ = &type;
  }
}

void
Parser::elaborate_function_declaration(Function_decl& d)
{
  std::cout << "FN: " << d << '\n';
}


void
Parser::elaborate_type_declaration(Type_decl& d)
{
  std::cout << "TYPE: " << d << '\n';
}


} // namespace banjo
