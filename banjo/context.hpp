// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONTEXT_HPP
#define BANJO_CONTEXT_HPP

#include "prelude.hpp"
#include "lookup.hpp"


namespace banjo
{

struct Decl;
struct Variable_decl;
struct Function_decl;
struct Namespace_decl;
struct Scope;


// A repository of information to support translation.
//
// TODO: Add an allocator/object pool and management support.
//
// TODO: Integrate diagnostics.
//
// TODO: Have a global context?
struct Context
{
  Context();

  // Non-copyable
  Context(Context const&) = delete;
  Context& operator=(Context const&) = delete;

  // Returns the symbol table.
  Symbol_table const& symbols() const { return syms; }
  Symbol_table&       symbols()       { return syms; }

  // Returns the global namespace.
  Namespace_decl const& global_namespace() const { return *global; }
  Namespace_decl&       global_namespace()       { return *global; }

  // Unique ids
  int get_unique_id();

  // Scope management
  void   set_scope(Scope&);
  Scope& make_initializer_scope(Decl&);
  Scope& make_function_scope(Decl&);
  Scope& make_function_parameter_scope();
  Scope& make_template_parameter_scope();
  Scope& current_scope();
  Decl&  current_context();

  Symbol_table    syms;
  Namespace_decl* global; // The global namespace
  Scope*          scope;  // The current scope
  Evidence*       facts;  // Knowledge of dependent terms

  int id = 0; // The current id counter.

  // Determines the kind of code under analysis.
  enum State
  {
    regular_code,  // Analyzing regular (non-dependent) code
    template_code, // Analyzing template code
    concept_code   // Analyzing dependent code
  };

  State state;
};


// Returns a unique id number and updates the context so that the
// next id will be different than this one. This is primarily used
// to maintain placehoder ids.
inline int
Context::get_unique_id()
{
  return id++;
}


// An RAII helper that manages the entry and exit of scopes.
struct Enter_scope
{
  Enter_scope(Context&, Namespace_decl&);
  Enter_scope(Context&, Scope&);
  ~Enter_scope();

  Context& cxt;
  Scope*   prev;  // The previous socpe.
  Scope*   alloc; // Only set when locally allocated.
};


// An RAII helper that manages the entry and exit of factual
// environments with a concept.
struct Enter_concept
{
  using State = Context::State;

  Enter_concept(Context&);
  ~Enter_concept();

  Context&  cxt;
  Evidence* facts;
  State     state;
};


inline
Enter_concept::Enter_concept(Context& c)
  : cxt(c), facts(cxt.facts), state(cxt.state)
{
  cxt.facts = new Evidence(facts);
  cxt.state = Context::concept_code;
}


inline
Enter_concept::~Enter_concept()
{
  delete cxt.facts;
  cxt.facts = facts;
  cxt.state = state;
}


} // namespace banjo


#endif
