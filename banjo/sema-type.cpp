// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "ast-type.hpp"
#include "type.hpp"

#include <iostream>


namespace banjo
{


Type&
Parser::on_class_type(Token)
{
  return build.get_type_type();
}


Type&
Parser::on_void_type(Token)
{
  return build.get_void_type();
}


Type&
Parser::on_byte_type(Token)
{
  return build.get_byte_type();
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
Parser::on_id_type(Name& n)
{
  return make_type(cxt, n);
}


Type&
Parser::on_decltype_type(Token, Expr&)
{
  lingo_unimplemented("on decltype-type");
}


// FIXME: Verify that all the component types are actually valid.
// For example, '() -> int...' is probably not valid.
Type&
Parser::on_function_type(Type_list& p, Type& r)
{
  return build.get_function_type(p, r);
}


// Returns a const-qualified type.
Type&
Parser::on_const_type(Type& t)
{
  return make_qualified_type(cxt, t, const_qual);
}


// Returns a volatile-qualified type.
Type&
Parser::on_volatile_type(Type& t)
{
  return make_qualified_type(cxt, t, volatile_qual);
}


Type&
Parser::on_pointer_type(Type& t)
{
  return make_pointer_type(cxt, t);
}


Type&
Parser::on_array_type(Type& t, Expr& e)
{
  return make_array_type(cxt, t, e);
}

Type&
Parser::on_tuple_type(Type_list& t)
{
  return make_tuple_type(cxt, t);
}


Type&
Parser::on_slice_type(Type& t)
{
  return make_slice_type(cxt, t);
}


Type&
Parser::on_reference_type(Type& t)
{
  return make_reference_type(cxt, t);
}


Type&
Parser::on_pack_type(Type& t)
{
  return make_pack_type(cxt, t);
}


Type&
Parser::on_unparsed_type(Token_seq&& toks)
{
  // FIXME: Use a factory method.
  return *new Unparsed_type(std::move(toks));
}


} // namespace banjo
