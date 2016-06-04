// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{

// -------------------------------------------------------------------------- //
// Builder definition

Symbol_table&
Builder::symbols() { return cxt.symbols(); }


// -------------------------------------------------------------------------- //
// Names

// Returns a simple identifier with the given spelling.
//
// TODO: Unique this?
Simple_id&
Builder::get_id(char const* s)
{
  Symbol const* sym = symbols().put_identifier(Token_kind::identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a simple identifier with the given spelling.
Simple_id&
Builder::get_id(std::string const& s)
{
  Symbol const* sym = symbols().put_identifier(Token_kind::identifier_tok, s);
  return make<Simple_id>(*sym);
}


// Returns a simple identifier for the given symbol.
Simple_id&
Builder::get_id(Symbol const& sym)
{
  lingo_assert(is<Identifier_sym>(&sym));
  return make<Simple_id>(sym);
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
// TODO: Make placeholders unique. Globally?
Placeholder_id&
Builder::get_id()
{
  return make<Placeholder_id>(cxt.get_unique_id());
}


Operator_id&
Builder::get_id(Operator_kind k)
{
  return make<Operator_id>(k);
}


// Returns a destructor-id for the given type.
Destructor_id&
Builder::get_destructor_id(Type const& t)
{
  lingo_unimplemented("destructor-id");
}


Template_id&
Builder::get_template_id(Template_decl& d, Term_list const& t)
{
  return make<Template_id>(d, t);
}


Concept_id&
Builder::get_concept_id(Concept_decl& d, Term_list const& t)
{
  return make<Concept_id>(d, t);
}


// Returns a qualified-id.
Qualified_id&
Builder::get_qualified_id(Decl& d, Name& n)
{
  return make<Qualified_id>(d, n);
}


// Return the global identifier.
Global_id&
Builder::get_global_id()
{
  // TODO: Global or no?
  static Global_id n;
  return n;
}




// -------------------------------------------------------------------------- //
// Expressions



// -------------------------------------------------------------------------- //
// Statements




// -------------------------------------------------------------------------- //
// Initializers

Trivial_init&
Builder::make_trivial_init()
{
  return make<Trivial_init>(get_void_type());
}


Copy_init&
Builder::make_copy_init(Expr& e)
{
  return make<Copy_init>(get_void_type(), e);
}


Bind_init&
Builder::make_bind_init(Expr& e)
{
  return make<Bind_init>(get_void_type(), e);
}


Direct_init&
Builder::make_direct_init(Decl& d, Expr_list const& es)
{
  return make<Direct_init>(get_void_type(), d, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list const& es)
{
  return make<Aggregate_init>(t, es);
}


Aggregate_init&
Builder::make_aggregate_init(Type& t, Expr_list&& es)
{
  return make<Aggregate_init>(t, std::move(es));
}


// -------------------------------------------------------------------------- //
// Declarations




// -------------------------------------------------------------------------- //
// Requirements

Basic_req&
Builder::make_basic_requirement(Expr& e, Type& t)
{
  return make<Basic_req>(e, t);
}


Conversion_req&
Builder::make_conversion_requirement(Expr& e, Type& t)
{
  return make<Conversion_req>(e, t);
}


Syntactic_req&
Builder::make_syntactic_requirement(Expr& e)
{
  return make<Syntactic_req>(e);
}


// -------------------------------------------------------------------------- //
// Statements

Translation_unit&
Builder::make_translation_unit()
{
  return make<Translation_unit>();
}


Translation_unit&
Builder::make_translation_unit(Stmt_list&& ss)
{
  return make<Translation_unit>(std::move(ss));
}


// -------------------------------------------------------------------------- //
// Constraints

// FIXME: Save all uniqued terms in the context, not as global variables.

Concept_cons&
Builder::get_concept_constraint(Decl& d, Term_list const& ts)
{
  static Factory<Concept_cons> f;
  return f.make(d, ts);
}


Predicate_cons&
Builder::get_predicate_constraint(Expr& e)
{
  static Factory<Predicate_cons> f;
  return f.make(e);
}


Expression_cons&
Builder::get_expression_constraint(Expr& e, Type& t)
{
  static Factory<Expression_cons> f;
  return f.make(e, t);
}


Conversion_cons&
Builder::get_conversion_constraint(Expr& e, Type& t)
{
  static Factory<Conversion_cons> f;
  return f.make(e, t);

}


Parameterized_cons&
Builder::get_parameterized_constraint(Decl_list const& ds, Cons& c)
{
  static Factory<Parameterized_cons> f;
  return f.make(ds, c);
}


Conjunction_cons&
Builder::get_conjunction_constraint(Cons& c1, Cons& c2)
{
  static Factory<Conjunction_cons> f;
  return f.make(c1, c2);
}


Disjunction_cons&
Builder::get_disjunction_constraint(Cons& c1, Cons& c2)
{
  static Factory<Disjunction_cons> f;
  return f.make(c1, c2);
}


}
