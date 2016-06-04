// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILDER_HPP
#define BANJO_BUILDER_HPP

// The builder module defines an interface for constructing core language
// terms. The builder manages the memory for every object (except symbols)
// constructed through its methods.

#include "build-type.hpp"
#include "build-expr.hpp"

#include <lingo/token.hpp>


namespace banjo
{


// An interface to an AST builder.
//
// This class uses the PIMPL idiom to define "sub-builders" for each kind
// of term. The purpose is to keep the AST header out of the include path
// for the Context. I wish there were a better way to do this.
struct Builder : Type_builder, Expr_builder
{
  Builder(Context& c)
    : Type_builder(c)
    , Expr_builder(c)
    , cxt(c)
  { }

  // Names
  Simple_id&      get_id(char const*);
  Simple_id&      get_id(std::string const&);
  Simple_id&      get_id(Symbol const&);
  Simple_id&      get_id(Symbol const*);
  Simple_id&      get_id(Token const&);
  Placeholder_id& get_id();
  Operator_id&    get_id(Operator_kind);
  // Conversion_id&  get_id();
  // Literal_id&     get_id();
  Destructor_id&  get_destructor_id(Type const&);
  Template_id&    get_template_id(Template_decl&, Term_list const&);
  Concept_id&     get_concept_id(Concept_decl&, Term_list const&);
  Qualified_id&   get_qualified_id(Decl&, Name&);
  Global_id&      get_global_id();


  // Toplevel structures
  Translation_unit& make_translation_unit();
  Translation_unit& make_translation_unit(Stmt_list&&);


  // Constraints
  // Note that constraints are canonicalized in order
  // ensure efficient hashing and equivalence comparison.
  Concept_cons&       get_concept_constraint(Decl&, Term_list const&);
  Predicate_cons&     get_predicate_constraint(Expr&);
  Expression_cons&    get_expression_constraint(Expr&, Type&);
  Conversion_cons&    get_conversion_constraint(Expr&, Type&);
  Parameterized_cons& get_parameterized_constraint(Decl_list const&, Cons&);
  Conjunction_cons&   get_conjunction_constraint(Cons&, Cons&);
  Disjunction_cons&   get_disjunction_constraint(Cons&, Cons&);

  // Resources
  Symbol_table& symbols();

  // Allocate an objet of the given type.
  //
  // TODO: This is a placeholder for using a legitimate object
  // pool in the context (or somewhere else).
  template<typename T, typename... Args>
  T& make(Args&&... args)
  {
    return *new T(std::forward<Args>(args)...);
  }

  Context& cxt;
};


} // namespace banjo


#endif
