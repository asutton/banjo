// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast-stmt.hpp"

namespace banjo
{


Stmt&
Parser::on_translation_unit(Stmt_list&& ss)
{
  Translation_stmt& unit = cxt.make_translation_unit(std::move(ss));

  // Analyze the types of declarations.
  elaborate_declarations(unit.statements());

  return unit;
}


} // namespace banjo
