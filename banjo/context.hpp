// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONTEXT_HPP
#define BANJO_CONTEXT_HPP

#include "prelude.hpp"
#include "scope.hpp"
#include "builder.hpp"


namespace banjo
{

struct Scope;


// A repository of information to support translation.
//
// TODO: Add an allocator/object pool and management support.
//
// TODO: Integrate diagnostics.
struct Context : Builder
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

  // Input location
  Location input_location() const       { return input; }
  void     input_location(Location loc) { input = loc; }

  // Scope management
  void   set_scope(Scope&);
  Initializer_scope&        make_initializer_scope(Decl&);
  Function_scope&           make_function_scope(Decl&);
  Function_parameter_scope& make_function_parameter_scope();
  Template_scope&           make_template_scope();
  Template_parameter_scope& make_template_parameter_scope();
  Block_scope&              make_block_scope();
  Requires_scope&           make_requires_scope();
  Concept_scope&            make_concept_scope(Decl&);
  Constrained_scope&        make_constrained_scope(Expr&);

  // Scope queries
  Scope&          current_scope();
  Template_scope* current_template_scope();
  Decl_list*      current_template_parameters();
  Expr*           current_template_constraints();
  Requires_scope* current_requires_scope();

  // Declaration context queries
  Decl&           current_context();
  Template_decl*  current_template();

  // Returns true if there we are in the scope of a template.
  bool in_template() const;

  // Returns true we are in the scope of a requires-expression.
  bool in_requirements() const;

  Symbol_table    syms;
  Location        input;  // The input location
  Namespace_decl* global; // The global namespace
  Scope*          scope;  // The current scope

  int id = 0; // The current id counter
};


// Returns true if there we are in the scope of a template.
inline bool
Context::in_template() const
{
  return modify(this)->current_template_scope();
}

// Returns true we are in the scope of a requires-expression.
inline bool
Context::in_requirements() const
{
  return modify(this)->current_requires_scope();
}



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


struct Enter_requires_scope : Enter_scope
{
  Enter_requires_scope(Context& cxt)
    : Enter_scope(cxt, cxt.make_requires_scope())
  { }
};



// -------------------------------------------------------------------------- //
// Diagnostic utilities

using lingo::error;
using lingo::warning;
using lingo::note;


// Emit a formatted message at the current input position.
template<typename... Args>
inline void
error(Context& cxt, char const* msg, Args const&... args)
{
  error(cxt.input_location(), msg, args...);
}


// Emit a formatted message at the current input position.
template<typename... Args>
inline void
warning(Context& cxt, char const* msg, Args const&... args)
{
  warning(cxt.input_location(), msg, args...);
}


// Emit a formatted message at the current input position.
template<typename... Args>
inline void
note(Context& cxt, char const* msg, Args const&... args)
{
  note(cxt.input_location(), msg, args...);
}


} // namespace banjo


#endif
