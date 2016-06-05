// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{

// Returns a simple identifier with the given spelling.
//
// TODO: Unique this?
Simple_id&
Builder::get_id(char const* s)
{
  Symbol const* sym = syms_.put_identifier(Token_kind::identifier_tok, s);
  return simple_id(*sym);
}


// Returns a simple identifier with the given spelling.
Simple_id&
Builder::get_id(std::string const& s)
{
  Symbol const* sym = syms_.put_identifier(Token_kind::identifier_tok, s);
  return simple_id(*sym);
}


// Returns a simple identifier for the given symbol.
Simple_id&
Builder::get_id(Symbol const& sym)
{
  return simple_id(sym);
}


// Returns a simple identifier for the symbol.
Simple_id&
Builder::get_id(Symbol const* sym)
{
  return get_id(*sym);
}


// Returns a simple identifier for the symbol.
Simple_id&
Builder::get_id(Token const& tok)
{
  return get_id(tok.symbol());
}


// Returns a placeholder for a name.
//
// TODO: This implementation isn't particularly good. It works for now,
// but I'm not happy with it.
Placeholder_id&
Builder::get_id()
{
  static int n = 0;
  return placeholder_id(n++);
}


Operator_id&
Builder::get_id(Operator_kind k)
{
  return operator_id(k);
}


// // Returns a destructor-id for the given type.
// Destructor_id&
// Builder::get_destructor_id(Type const& t)
// {
//   lingo_unreachable();
// }


// Template_id&
// Builder::get_template_id(Template_decl& d, Term_list const& t)
// {
//   return make<Template_id>(d, t);
// }


// Concept_id&
// Builder::get_concept_id(Concept_decl& d, Term_list const& t)
// {
//   return make<Concept_id>(d, t);
// }


// // Returns a qualified-id.
// Qualified_id&
// Builder::get_qualified_id(Decl& d, Name& n)
// {
//   return make<Qualified_id>(d, n);
// }


// // Return the global identifier.
// Global_id&
// Builder::get_global_id()
// {
//   // TODO: Global or no?
//   static Global_id n;
//   return n;
// }


} // namespace banjo
