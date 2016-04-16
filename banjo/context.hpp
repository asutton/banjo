// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONTEXT_HPP
#define BANJO_CONTEXT_HPP

#include "prelude.hpp"
#include "builder.hpp"
#include "scope.hpp"


namespace banjo
{

struct Scope;


// Used to associate scopes with declarations.
using Scope_map = std::unordered_map<Decl*, Scope*>;


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

  // Unique ids
  int get_unique_id();

  // Input location
  Location input_location() const       { return input; }
  void     input_location(Location loc) { input = loc; }

  // Scope management
  Scope& make_scope();
  Scope& make_scope(Decl&);
  Scope& saved_scope(Decl&);
  void   set_scope(Scope&);
  Scope& current_scope();
  Scope& global_scope();

  Decl* immediate_context();
  Decl* current_context();

  // Diagnostic state
  bool diagnose_errors() const { return diags; }

  Symbol_table syms;   // The symbol table
  Location     input;  // The input location
 
  // Scope information
  Scope*       global; // The global scope
  Scope*       scope;  // The current scope
  Scope_map    saved;  // Saved scopes.

  // Store information for generating unique names.
  int             id;     // The current id counter

  // Diagnostic state
  bool diags; // True if diagnostics should be emitted.
};


// Returns a new general purpose scope.
inline Scope&
Context::make_scope()
{
  return *new Scope(current_scope());
}


// Returns a new general purpose scope bound to the given declaration.
inline Scope&
Context::make_scope(Decl& d)
{
  return *new Scope(current_scope(), d);
}


// Retrieve the saved scope for the declaration. If no such scope exists,
// create one. Note that newly created saved scopes are linked to the current
// scope.
inline Scope&
Context::saved_scope(Decl& d)
{
  auto iter = saved.find(&d);
  if (iter != saved.end()) {
    return *iter->second;
  } else {
    Scope& s = make_scope(d);
    saved.emplace(&d, &s);
    return s;
  }
}


// Enter the given scope. Unless `s` is the scope of the global
// namespace, `s` must be linked through its enclosing scopes
// to the global namespace.
//
// Do not call this function directly. Use Context::Scope_sentinel
// to enter a new scope, and guarantee cleanup and scope exit.
inline void
Context::set_scope(Scope& s)
{
  scope = &s;
}


// Returns the current scope.
inline Scope&
Context::current_scope()
{
  return *scope;
}


// Returns a unique id number and updates the context so that the
// next id will be different than this one. This is primarily used
// to maintain placeholder ids.
inline int
Context::get_unique_id()
{
  return id++;
}


// An RAII helper that manages the entry and exit of scopes.
struct Enter_scope
{
  Enter_scope(Context&);
  Enter_scope(Context&, Scope&);
  ~Enter_scope();

  Context& cxt;
  Scope*   prev;  // The previous scope.
  Scope*   alloc; // An owned scope.
};


// Enter a new purpose scope. This scope is destroyed when the sentinel
// goes out of scope.
inline
Enter_scope::Enter_scope(Context& cxt)
  : cxt(cxt), prev(&cxt.current_scope()), alloc(&cxt.make_scope())
{
  cxt.set_scope(*alloc);
}


// Enter the given scope.
inline
Enter_scope::Enter_scope(Context& c, Scope& s)
  : cxt(c), prev(&c.current_scope()), alloc(nullptr)
{
  cxt.set_scope(s);
}


// Restore the previous scope and delete any allocated scopes.
inline
Enter_scope::~Enter_scope()
{
  cxt.set_scope(*prev);
  delete alloc;
}


// -------------------------------------------------------------------------- //
// Input location

struct Save_input_location
{
  Save_input_location(Context& c)
    : cxt(c), prev(cxt.input_location())
  { }

  ~Save_input_location()
  {
    cxt.input_location(prev);
  }

  Context& cxt;
  Location prev;
};


// -------------------------------------------------------------------------- //
// Diagnostic utilities


struct Change_diagnostics
{
  Change_diagnostics(Context& cxt, bool b)
    : cxt(cxt), prev(cxt.diags)
  {
    cxt.diags = b;
  }

  ~Change_diagnostics()
  {
    cxt.diags = prev;
  }

  Context& cxt;
  bool     prev;
};


// Indicate that diagnostics should be suppressed.
struct Suppress_diagnostics : Change_diagnostics
{
  Suppress_diagnostics(Context& cxt)
    : Change_diagnostics(cxt, false)
  { }
};


// Indiate that diagnostics should be emitted
struct Emit_diagnostics : Change_diagnostics
{
  Emit_diagnostics(Context& cxt)
    : Change_diagnostics(cxt, true)
  { }
};


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
