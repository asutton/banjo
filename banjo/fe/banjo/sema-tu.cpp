// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

// #include "elab-declarations.hpp"
// #include "elab-overloads.hpp"
// #include "elab-classes.hpp"
// #include "elab-expressions.hpp"

#include <banjo/ast.hpp>


namespace banjo
{

namespace fe
{

// Returns the translation unit.
Decl&
Parser::start_translation_unit()
{
  return cxt.builtins().translation_unit();
}


// Analyze the remainder of the translation unit and assign assign the
// statement sequence to the unit.
//
// NOTE: The level of semantic analysis can be varied by adding, removing,
// or interchanging the elaboration passes. Also note that non-modifying
// checks can be run in parallel, not sequentially.
//
// TODO: *big project*: Factor out a grammar-based visitor for elaboration
// passes. This should reflect the grammar, manage scope, and call into
// a function object that defines the elaboration rules.
Decl&
Parser::finish_translation_unit(Decl& d, Stmt_list&& ss)
{
  // Add statements to the translation unit.
  Translation_unit& tu = cast<Translation_unit>(d);
  tu.stmts_ = std::move(ss);

  // Elaborate_declarations declarations(*this);
  // Elaborate_overloads    overloads(*this);
  // Elaborate_classes      classes(*this);
  // Elaborate_expressions  expressions(*this);

  // declarations(tu); // Assign types to declarations

  // // TODO: Transform abbreviated templates into templates.

  // overloads(tu);    // Analyze overloaded/reopened declarations
  // classes(tu);      // Complete class definitions
  // expressions(tu);  // Update expressions

  return tu;
}


} // namespace fe

} // namespace banjo
