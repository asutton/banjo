// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONTEXT_HPP
#define BANJO_CONTEXT_HPP

#include "prelude.hpp"


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
  Scope*          scope;  // The current scope.
};


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


} // namespace banjo


#endif
