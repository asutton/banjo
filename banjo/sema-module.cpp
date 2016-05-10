// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast-stmt.hpp"

#include <iostream>


namespace banjo
{

Stmt&
Parser::on_translation_unit(Stmt_list&& ss)
{
  Translation_stmt& unit = cxt.make_translation_unit(std::move(ss));

  // TODO: All of these should be taking the translation unit
  // so that we can compute and push properties up.

  // Assign types to declarations.
  elaborate_declarations(unit.statements());

  // Check overloading sanity.
  elaborate_overloads(unit.statements());

  // Merge partial classes.
  // elaborate_partials(unit.statements());

  // Elaborate all class definitions.
  elaborate_classes(unit.statements());


  // Elaborate all function definitions.


  return unit;
}


} // namespace banjo
