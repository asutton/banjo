// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>
#include <banjo/declaration.hpp>


namespace banjo
{

namespace fe
{

// Build and declare the class.
Decl&
Parser::start_class_declaration(Name& n)
{
  Decl& d = cxt.make_class_declaration(n);
  declare(cxt, current_scope(), d);
  return d;
}


// Associate the class with it's definition.
Decl&
Parser::finish_class_definition(Decl& decl, Def& def)
{
  Class_decl& cls = cast<Class_decl>(decl);
  cls.def_ = &def;
  return cls;
}


// Build a class definition from the statement sequence.
Def&
Parser::on_class_body(Stmt_list&& ss)
{
  return cxt.make_class_definition(std::move(ss));
}


Decl&
Parser::on_super_declaration(Name& n, Type& t)
{
  lingo_unreachable();
  // Decl& d = cxt.make_super_declaration(t);
  // declare(cxt, d);
  // return d;
}


} // namespace fe

} // namespace banjo
