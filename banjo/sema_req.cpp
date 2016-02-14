
// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "ast_decl.hpp"
#include "ast_req.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

Req&
Parser::on_type_requirement(Expr&)
{
  lingo_unimplemented();
}


Req&
Parser::on_syntactic_requirement(Expr&)
{
  lingo_unimplemented();
}


Req&
Parser::on_semantic_requirement(Decl&)
{
  lingo_unimplemented();
}


Req&
Parser::on_expression_requirement(Expr&)
{
  lingo_unimplemented();
}


Req&
Parser::on_simple_requirement(Expr&)
{
  lingo_unimplemented();
}


Req&
Parser::on_conversion_requirement(Expr&, Type&)
{
  lingo_unimplemented();
}


Req&
Parser::on_deduction_requirement(Expr&, Type&)
{
  lingo_unimplemented();
}


} // namespace banjo
