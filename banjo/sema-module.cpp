// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-stmt.hpp"
#include "elab-expressions.hpp"

#include <iostream>


namespace banjo
{

Stmt&
Parser::on_translation_unit(Stmt_list&& ss)
{
  Translation_stmt& unit = cxt.make_translation_unit(std::move(ss));

  // TODO: All of these should be taking the translation unit
  // so that we can compute and push properties up.

  Elaborate_expressions expr(*this);


  elaborate_declarations(unit.statements()); // Assign types to names
  elaborate_overloads(unit.statements());    // Check overloads
  // elaborate_partials(unit.statements());     // Manage partial definitions
  elaborate_classes(unit.statements());      // Analyze class definitions
  // elaborate_statements(unit.statements());   // Finish up all statements

  expr(unit); // Typecheck expressions

  return unit;
}


} // namespace banjo
