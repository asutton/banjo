// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <iostream>


namespace banjo
{


// TOOD: Resolve the id as either a variable name, a function
// name, or an overload name. It can be nothing else.
Expr&
Parser::on_id_expression(Name&)
{
  lingo_unimplemented();
}


Expr&
Parser::on_integer_literal(Token)
{
  lingo_unimplemented();
}


} // namespace banjo
