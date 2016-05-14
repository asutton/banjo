// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-stmt.hpp"
#include "elab-overloads.hpp"
#include "elab-classes.hpp"
#include "elab-expressions.hpp"

#include <iostream>


namespace banjo
{

Stmt&
Parser::on_translation_unit(Stmt_list&& ss)
{
  Translation_stmt& unit = cxt.make_translation_unit(std::move(ss));

  Elaborate_overloads   overloads(*this);
  Elaborate_classes     classes(*this);
  Elaborate_expressions expressions(*this);

  elaborate_declarations(unit.statements()); // Assign types to names

  overloads(unit);   // Check and merge declarations
  classes(unit);     // Complete class definitions
  expressions(unit); // Parse and type expressions

  return unit;
}


} // namespace banjo
