// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONTEXT_HPP
#define BANJO_CONTEXT_HPP

#include "prelude.hpp"


namespace banjo
{

struct Namespace_decl;


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
  Namespace_decl const& global_namespace() const { return *top; }
  Namespace_decl&       global_namespace()       { return *top; }

  Symbol_table    syms;
  Namespace_decl* top;  // The global namespace

};


} // namespace banjo


#endif
