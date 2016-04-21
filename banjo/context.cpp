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
  : Builder(*this), syms()
  , global(&make_scope()), scope(nullptr)
  , id(0)
  , diags(false)
{
  // Initialize the color system. This is a process-level
  // configuration. Perhaps we we should only initialize
  // colors if the default output terminal is actually the
  // output terminal.
  init_colors();

  // Initialize all the tokens.
  init_tokens(syms);
}


// Returns the context associated with the current scope or nullptr if
// there is none.
Decl*
Context::immediate_context()
{
  return scope->context();
}


// Returns the innermost declaration context, or nullptr if there is none.
Decl*
Context::current_context()
{
  Scope* p = scope;
  while (p) {
    if (Decl* d = p->context())
      return d;
    p = p->enclosing_scope();
  }
  return nullptr;
}


} // namespace banjo
