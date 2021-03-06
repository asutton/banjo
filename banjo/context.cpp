// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "ast.hpp"
#include "scope.hpp"
#include "declaration.hpp"
#include "debugging.hpp"

#include <lingo/io.hpp>

#include <iostream>


namespace banjo
{

Context::Context(Symbol_table& s)
  : List_allocator()
  , Builder(*this, s)
  , syms_(s)
  , scope(nullptr)
  , id(0)
  , diags(false)
{
  // Initialize the color system. This is a process-level configuration. 
  // Perhaps we we should only initialize colors if the default output 
  // terminal is actually the output terminal.
  lingo::init_colors();

  // Initialize built-in entity definitions.
  init_builtins(*this);
}


Translation_unit const&
Context::translation_unit() const
{
  return builtins_.translation_unit();
}


Translation_unit&      
Context::translation_unit()
{
  return builtins_.translation_unit();
}


Scope const& 
Context::global_scope() const 
{ 
  return const_cast<Context&>(*this).global_scope();
}


Scope&       
Context::global_scope() 
{ 
  return saved_scope(builtins_.translation_unit()); 
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
  if (!diags) return;
  
  std::cerr << "error: " << input << ": ";
  dump(std::cerr, m);
  std::cerr << '\n';
}


void
Context::emit_warning(Message const& m)
{
  if (!diags) return;

  std::cerr << "warning: " << input << ": ";
  dump(std::cerr, m);
  std::cerr << '\n';
}


// Just write the rendered string to cerr.
void
Context::emit_info(Message const& m)
{
  dump(std::cerr, m);
}


// Create a function call operator for the given declaration. We guarantee 
// that the operator is unique for all functions of equivalent type.
Decl&
Context::synthesize_call_operator(Function_decl& d)
{
  // Don't synthesize the operator twice.
  auto iter = op_calls_.find(&d.type());
  if (iter != op_calls_.end())
    return *iter->second;

  // Build the parameters for the operator.
  //
  // TODO: Do I need to clone the parameters?
  Decl_list& ps = d.parameters();
  Decl& fp = make_variable_parameter(get_id(), d.type());
  Decl_list ps2 = {&fp};
  for (Decl&  p : ps) ps2.push_back(p);

  // Build the function type.
  //
  // TODO: Do I need to clone the types?
  Type_list& ts = d.type().parameter_types();
  Type_list ts2 = {&d.type()};
  for (Type& t : ts) ts2.push_back(t);
  Type& type = get_function_type(std::move(ts2), d.return_type());

  // Declare the function in the global scope.
  //
  // FIXME: Generate an intrinsic definition of the operator. The
  // behavior should be to simply set up a call the named function.
  Name& name = get_id(call_op);
  Decl& op = make_function_declaration(name, type, std::move(ps2));
  
  declare(*this, global_scope(), op);

  // Remember the operator.
  op_calls_.insert({&d.type(), &op});

  return op;
}



} // namespace banjo
