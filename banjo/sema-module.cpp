// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast-stmt.hpp"

namespace banjo
{


Stmt&
Parser::on_translation_statement(Stmt_list&& ss)
{
  return cxt.make_translation_statement(std::move(ss));
}


} // namespace banjo
