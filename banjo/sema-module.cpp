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

  // Assign types to declarations.
  elaborate_declarations(unit.statements());
  std::cout << "HERE: " << unit << '\n';

  // Check overloading sanity.
  elaborate_overloads(unit.statements());

  // Merge partial classes.
  // elaborate_partials(unit.statements());

  // Check that overloads are sane.


  return unit;
}


} // namespace banjo
