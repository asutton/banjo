
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>
#include <banjo/requirement.hpp>


namespace banjo
{

namespace fe
{

Req&
Parser::on_type_requirement(Expr&)
{
  lingo_unimplemented("on type-req");
}


Req&
Parser::on_syntactic_requirement(Expr& e)
{
  lingo_unimplemented("requirements");
  // return make_syntactic_requirement(cxt, e);
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
  lingo_unimplemented("requirements");
  // return make_basic_requirement(cxt, e);
}


Req&
Parser::on_basic_requirement(Expr& e, Type& t)
{
  lingo_unimplemented("requirements");
  // return make_basic_requirement(cxt, e, t);
}


Req&
Parser::on_conversion_requirement(Expr& e, Type& t)
{
  lingo_unimplemented("requirements");
  // return make_conversion_requirement(cxt, e, t);
}


Req&
Parser::on_deduction_requirement(Expr& e, Type& t)
{
  lingo_unimplemented("requirements");
  // return make_deduction_requirement(cxt, e, t);
}


} // namespace fe

} // namespace banjo
