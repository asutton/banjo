// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"

#include <banjo/ast.hpp>
#include <banjo/expression.hpp>


namespace banjo
{

namespace fe
{

// -------------------------------------------------------------------------- //
// Identifiers

Name&
Parser::on_simple_id(Token tok)
{
  return cxt.get_id(tok);
}


#if 0
Name&
Parser::on_destructor_id(Token, Type& t)
{
  return cxt.get_destructor_id(t);
}


Name&
Parser::on_operator_id(Token tok, Operator_kind op)
{
  return cxt.get_id(op);
}


Name&
Parser::on_conversion_id()
{
  lingo_unimplemented("on conversion-id");
}


Name&
Parser::on_literal_id()
{
  lingo_unimplemented("on user-defined-literal");
}


// FIXME: Actually match template arguments? Or do that when we
// require the id to be resolved (e.g., as a type-name or an
// id-expression)?
Name&
Parser::on_template_id(Decl& d, Term_list const& a)
{
  return cxt.get_template_id(cast<Template_decl>(d), a);
}


// FIXME: Actually match the template arguments? Or do that when
// we require the id to be resolved (i.e., as a check expression).
Name&
Parser::on_concept_id(Decl& d, Term_list const& a)
{
  return cxt.get_concept_id(cast<Concept_decl>(d), a);
}


Name&
Parser::on_qualified_id(Decl& d, Name& n)
{
  return cxt.get_qualified_id(d, n);
}

// -------------------------------------------------------------------------- //
// Resolved names
//
// All of these functions perform lookup on their id and
// check that the resolved declaration matches the specified
// name.


// FIXME: What if the identifier refers to a set of declarations?
Decl&
Parser::on_template_name(Token tok)
{
  Simple_id& id = cxt.get_id(tok);
  Decl& decl = simple_lookup(cxt, id);
  if (is<Template_decl>(&decl))
    return decl;
  throw Lookup_error("'{}' does not name a template", id);
}


// FIXME: What if the identifier refers to a set of declarations?
Decl&
Parser::on_concept_name(Token tok)
{
  Simple_id& id = cxt.get_id(tok);
  Decl& decl = simple_lookup(cxt, id);
  if (is<Concept_decl>(&decl))
    return decl;
  throw Lookup_error("'{}' does not name a concept", id);
}
#endif


} // namespace fe

} // namespace banjo
