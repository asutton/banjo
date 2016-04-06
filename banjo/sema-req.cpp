
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "requirement.hpp"

#include <iostream>


namespace banjo
{

Req&
Parser::on_type_requirement(Expr&)
{
  lingo_unimplemented("on type-req");
}


Req&
Parser::on_syntactic_requirement(Expr& e)
{
  return make_syntactic_requirement(cxt, e);
}


Req&
Parser::on_semantic_requirement(Decl&)
{
  lingo_unimplemented("on semantic-req");
}


Req&
Parser::on_expression_requirement(Expr&)
{
  lingo_unimplemented("on expression-req");
}


Req&
Parser::on_basic_requirement(Expr& e)
{
  return make_basic_requirement(cxt, e);
}


Req&
Parser::on_basic_requirement(Expr& e, Type& t)
{
  return make_basic_requirement(cxt, e, t);
}


Req&
Parser::on_conversion_requirement(Expr& e, Type& t)
{
  return make_conversion_requirement(cxt, e, t);
}


Req&
Parser::on_deduction_requirement(Expr& e, Type& t)
{
  return make_deduction_requirement(cxt, e, t);
}


} // namespace banjo
