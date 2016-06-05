// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "builtin.hpp"
#include "scope.hpp"

#include <lingo/io.hpp>

#include <iostream>


namespace banjo
{

Context::Context()
  : scope(nullptr)
  , id(0)
  , diags(false)
  , builtins_(new Builtins())
{
  // Initialize the color system. This is a process-level configuration. 
  // Perhaps we we should only initialize colors if the default output 
  // terminal is actually the output terminal.
  lingo::init_colors();

  // Initialize built-in entity definitions.
  init_builtins(*this, *builtins_);
}


Context::~Context()
{
}


Scope const& 
Context::global_scope() const 
{ 
  // Look away...
  auto& self = const_cast<Context&>(*this);
  auto& tu = const_cast<Translation_unit&>(builtins().translation_unit());
  return self.saved_scope(tu); 
}


Scope&       
Context::global_scope() 
{ 
  return saved_scope(translation_unit()); 
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


// FIXME: We should probably do better for a default implementation.

void
Context::emit_error(Message const& m)
{
  std::stringstream ss;
  ss << "error: " << input << ": ";
  dump(ss, m);
  std::cerr << ss.str() << '\n';
}


void
Context::emit_warning(Message const& m)
{
  std::stringstream ss;
  ss << "error: " << input << ": ";
  dump(ss, m);
  std::cerr << ss.str() << '\n';
}


} // namespace banjo
