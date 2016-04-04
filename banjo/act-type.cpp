// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast-type.hpp"

#include <iostream>


namespace banjo
{


Type&
Parser::on_void_type(Token)
{
  return build.get_void_type();
}


Type&
Parser::on_bool_type(Token)
{
  return build.get_bool_type();
}


Type&
Parser::on_int_type(Token)
{
  return build.get_int_type();
}

Type&
Parser::on_byte_type(Token)
{
  return build.get_byte_type();
}

Type&
Parser::on_decltype_type(Token, Expr&)
{
  lingo_unimplemented();
}


Type&
Parser::on_function_type(Type_list& p, Type& r)
{
  return build.get_function_type(p, r);
}


Type&
Parser::on_reference_type(Token, Type& t)
{
  return build.get_reference_type(t);
}


// Returns a type with the quaification that includes `q`.
// If `t` is already qualified, union `q` into the qualifier
// of `t`.
//
// TODO: Verify that we can actually qualify this type.
Type&
Parser::on_qualified_type(Token, Type& t, Qualifier_set q)
{
  if (Qualified_type* qt = as<Qualified_type>(&t)) {
    qt->qual |= q;
    return *qt;
  } else {
    return build.get_qualified_type(t, q);
  }
}


// Returns a const-qualified type.
Type&
Parser::on_const_type(Token tok, Type& t)
{
  return on_qualified_type(tok, t, const_qual);
}


// Returns a volatile-qualified type.
Type&
Parser::on_volatile_type(Token tok, Type& t)
{
  return on_qualified_type(tok, t, volatile_qual);
}


// TODO: Verify that we can point to t.
Type&
Parser::on_pointer_type(Token, Type& t)
{
  return build.get_pointer_type(t);
}


Type&
Parser::on_sequence_type(Type& t)
{
  return build.get_sequence_type(t);
}


} // namespace banjo
