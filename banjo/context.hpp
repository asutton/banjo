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


// Used to associate scopes with terms: the translation unit, classes,
// functions, and compound statements.
using Scope_map = std::unordered_map<Term*, Scope*>;


// Used to manage the current declaration context. The top is either the
// translation unit, a function, or a class.
using Context_stack = std::vector<Decl*>;


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
  Scope& make_scope(Term&);
  Scope& saved_scope(Term&);
  void enter_scope(Scope*);
  void leave_scope(Scope*);
  
  // Returns the current scope.
  Scope const& current_scope() const { return *scope; }
  Scope&       current_scope()       { return *scope; }

  // Returns the global scope.
  Scope const& global_scope() const { return *global; }
  Scope&       global_scope()       { return *global; }

  // Declaration contexts.
  void enter_context();
  void leave_context();
  
  // Returns the current declaration context.
  Decl const& current_context() const { return *cxt.back(); }
  Decl&       current_context()       { return *cxt.back(); }

  // Diagnostic state
  bool diagnose_errors() const { return diags; }

  Symbol_table syms;   // The symbol table
  Location     input;  // The input location
 
  // Scope and context.
  Scope*        global; // The global scope
  Scope*        scope;  // The current scope
  Scope_map     saved;  // Saved scopes
  Context_stack cxt;    // Declaration context

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


// Returns a new general purpose scope bound to the given term.
inline Scope&
Context::make_scope(Term& t)
{
  return *new Scope(current_scope(), t);
}


// Retrieve the saved scope for the declaration. If no such scope exists,
// create one. Note that newly created saved scopes are linked to the 
// current scope.
inline Scope&
Context::saved_scope(Term& t)
{
  auto iter = saved.find(&t);
  if (iter != saved.end()) {
    return *iter->second;
  } else {
    Scope& s = make_scope(t);
    saved.emplace(&t, &s);
    return s;
  }
}


// Enter the given scope. Note that the scope must be chained to its
// enclosing scope.
//
// This will establish the declaration used as the global scope if
// it has not been previously established.
//
// Do not call this function directly. Use Enter_scope to enter a new 
// scope, and guarantee cleanup and scope exit.
inline void
Context::enter_scope(Scope* s)
{
  if (!scope)
    global = s;
  scope = s;
}


// Leave the current scope and make the new scope s.
//
// Do not call this function directly. Use Enter_scope to enter a new 
// scope, and guarantee cleanup and scope exit.
inline void
Context::leave_scope(Scope* s)
{
  scope = s;
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
  Enter_scope(Context&, Scope&);
  Enter_scope(Context&, Term&);
  ~Enter_scope();

  Context& cxt;
  Scope*   prev;  // The previous scope.
};


// Enter the given scope.
inline
Enter_scope::Enter_scope(Context& c, Scope& s)
  : cxt(c), prev(&c.current_scope())
{
  cxt.enter_scope(&s);
  cxt.enter_context();
}


// Enter the scope associated with the given term.
inline
Enter_scope::Enter_scope(Context& c, Term& t)
  : cxt(c), prev(&c.current_scope())
{
  cxt.enter_scope(&cxt.saved_scope(t));
  cxt.enter_context();
}


// Restore the previous scope and delete any allocated scopes.
inline
Enter_scope::~Enter_scope()
{
  cxt.leave_context();
  cxt.leave_scope(prev);
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
