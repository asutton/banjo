// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "type.hpp"

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
    void operator()(Decl& d)          { lingo_unhandled(d); }
    void operator()(Object_decl& d)   { p.elaborate_object_declaration(d); }
    void operator()(Function_decl& d) { p.elaborate_function_declaration(d); }
    void operator()(Class_decl& d)    { p.elaborate_class_declaration(d); }
  };
  apply(d, fn{*this});
}



void
Parser::elaborate_object_declaration(Object_decl& d)
{
  d.type_ = &elaborate_type(d.type());
}


void
Parser::elaborate_function_declaration(Function_decl& d)
{
  // Elaborate the type of each parameter in turn. Note that this does
  // not declare the parameters, it just checks their types.
  Decl_list& parms = d.parameters();
  for (Decl& d : parms)
    elaborate_parameter_declaration(cast<Object_parm>(d));


  // Elaborate the return type.
  Type& ret = elaborate_type(d.return_type());

  // Rebuild the function type and update the declaration.
  d.type_ = &cxt.get_function_type(parms, ret);

  // TODO: Elaborate the function constraints.
}


void
Parser::elaborate_parameter_declaration(Object_parm& p)
{
  p.type_ = &elaborate_type(p.type());

  // Create template parameters for all of the placeholders in
  // the type of the parameter.
  //
  // TODO: When can (should?) we unify placeholder types. Maybe this
  // is something we should do when we actually elaborate types of
  // the parameters (i.e., bind names to placeholders).
  Type_list types = get_placeholders(p.type());
  // std::cout << "HERE: " << p.name() << ' ' << types.size() << '\n';
}


// Elaborate the kind of a type.
void
Parser::elaborate_class_declaration(Class_decl& d)
{
  d.kind_ = &elaborate_type(d.kind());
}


Type&
Parser::elaborate_type(Type& t)
{
  if (Unparsed_type* soup = as<Unparsed_type>(&t)) {
    Save_input_location loc(cxt);
    Token_stream ts(soup->tokens());
    Parser parse(cxt, ts);
    return parse.type();
  }
  return t;
}


} // namespace banjo
