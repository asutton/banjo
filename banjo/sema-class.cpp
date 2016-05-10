// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"
#include "declaration.hpp"

#include <iostream>


namespace banjo
{

// Handle the declaration of the class.
Decl&
Parser::on_class_declaration(Name& n, Type& t)
{
  Decl& d = cxt.make_class_declaration(n, t);
  declare(cxt, current_scope(), d);
  return d;
}


// Associate the class with it's definition.
Decl&
Parser::on_class_definition(Decl& decl, Def& def)
{
  Class_decl& cls = cast<Class_decl>(decl);
  cls.def_ = &def;
  return cls;
}


Def&
Parser::on_class_body(Stmt_list&& ss)
{
  Def& def = cxt.make_class_definition(std::move(ss));

  // TODO: Partition the original statements of the class in order
  // to recognize member variables, bases, constructors, etc. We can't
  // determine their specific properties (in general), but we do know
  // about declarations and statements.

  return def;
}


Decl&
Parser::on_super_declaration(Name& n, Type& t)
{
  Decl& d = cxt.make_super_declaration(t);
  declare(cxt, d);
  return d;
}





} // namespace banjo
