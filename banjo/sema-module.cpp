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

// Analyze the remainder of the translation unit and assign assign the
// statement sequence to the unit.
//
// NOTE: The level of semantic analysis can be varied by adding, removing,
// or interchanging the elaboration passes. Also note that non-modifying
// checks can be run in parallel, not sequentially.
Decl&
Parser::on_translation_unit(Decl& d, Stmt_list&& ss)
{
  // Add statements to the translation unit.
  Translation_unit& tu = cast<Translation_unit>(d);
  tu.stmts_ = std::move(ss);

  Elaborate_overloads   overloads(*this);
  Elaborate_classes     classes(*this);
  Elaborate_expressions expressions(*this);

  elaborate_declarations(tu.statements()); // Assign types to names

  overloads(tu);   // Check and merge declarations
  classes(tu);     // Complete class definitions
  expressions(tu); // Parse and type expressions

  return tu;
}


} // namespace banjo
