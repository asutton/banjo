// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_CONTEXT_HPP
#define BEAKER_CONTEXT_HPP

#include "prelude.hpp"


namespace beaker
{

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

  Symbol_table& symbols() { return syms; }

  Symbol_table syms;
};


} // namespace beaker


#endif
