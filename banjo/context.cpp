// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "scope.hpp"
#include "token.hpp"

#include <lingo/io.hpp>


namespace banjo
{

Context::Context()
  : syms()
{
  // Initialize the color system. This is a process-level
  // configuration. Perhaps we we should only initialize
  // colors if the default output terminal is actually the
  // output terminal.
  init_colors();

  // Initialize all the tokens.
  init_tokens(syms);

  // Initialize the global namepace.
  Builder build(*this);
  global = &build.get_global_namespace();
}


// -------------------------------------------------------------------------- //
// Scope management

// Enter the given scope. Unless `s` is the scope of the global
// namespace, `s` must be linked through its enclosing scopes
// to the global namespace.
//
// Do not call this function directly. Use Context::Scope_sentinel
// to enter a new scope, and guarantee cleanup and scope exit.
void
Context::set_scope(Scope& s)
{
  scope = &s;
}


// Create a new initializer scopee.
Scope&
Context::make_initializer_scope(Decl& d)
{
  return *new Initializer_scope(current_scope(), d);
}


// Create a new function scope.
Scope&
Context::make_function_scope(Decl& d)
{
  return *new Function_scope(current_scope(), d);
}


// Create a new function parameter scope.
Scope&
Context::make_function_parameter_scope()
{
  return *new Function_parameter_scope(current_scope());
}


// Create a new template parameter scope.
Scope&
Context::make_template_parameter_scope()
{
  return *new Template_parameter_scope(current_scope());
}


// Returns the current scope.
Scope&
Context::current_scope()
{
  return *scope;
}


// Find the innermost declaration context. This is the first
// scope associatd with a declaration.
Decl&
Context::current_context()
{
  Scope* p = &current_scope();
  while (!p->context())
    p = p->enclosing_scope();
  return *p->context();
}


// -------------------------------------------------------------------------- //
// Enter scope

// Enter the scope associated with a namespace definition.
Enter_scope::Enter_scope(Context& c, Namespace_decl& ns)
  : cxt(c), prev(&c.current_scope()), alloc(nullptr)
{
  cxt.set_scope(*ns.scope());
}


// Enter the given scope. This assumes ownership of the given
// scope and deletes it when the class goes out of scope.
Enter_scope::Enter_scope(Context& c, Scope& s)
  : cxt(c), prev(&c.current_scope()), alloc(&s)
{
  cxt.set_scope(*alloc);
}


// Restore the previous scope and delete any allocated scopes.
Enter_scope::~Enter_scope()
{
  cxt.set_scope(*prev);
  delete alloc;
}


} // namespace banjo
