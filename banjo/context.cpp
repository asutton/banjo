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
  , global(nullptr), scope(nullptr)
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


// If the current scope is associated with a declaration, make that
// the current declaration. This must be called just after entering 
// a new scope.
//
// Do not call this function directly. Use Enter_scope instead.
void
Context::enter_context()
{
  if (Decl* d = as<Decl>(&scope->context()))
    cxt.push_back(d);
}


// If the current scope is associated with a declaration, restore the
// previous context. This must be called just prior to restoring a 
// previous scope.
//
// Do not call this function directly. Use Enter_scope instead.
void
Context::leave_context()
{
  if (is<Decl>(&scope->context()))
    cxt.pop_back();
}


} // namespace banjo
