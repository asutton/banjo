// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

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
Decl&
Parser::finish_translation_unit(Decl& d, Stmt_list&& ss)
{
  Translation_unit& tu = cast<Translation_unit>(d);
  tu.stmts_ = std::move(ss);
  return tu;
}


} // namespace fe

} // namespace banjo
