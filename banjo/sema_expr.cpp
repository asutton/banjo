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
Parser::on_boolean_literal(Token, bool b)
{
  return build.get_bool(b);
}


Expr&
Parser::on_integer_literal(Token tok)
{
  Type& t = build.get_int_type();
  Integer n = tok.spelling();
  return build.get_integer(t, n);
}


} // namespace banjo
