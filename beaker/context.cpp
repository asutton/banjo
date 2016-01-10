// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "token.hpp"

#include <lingo/io.hpp>


namespace beaker
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
}


} // namespace beaker
