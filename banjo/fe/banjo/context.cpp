// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "token.hpp"

#include <lingo/io.hpp>


namespace banjo
{

namespace fe
{

Context::Context()
  : banjo::Context()
{
  // Initialize the tokens.
  init_tokens(syms);
}


} namespace fe

} // namespace banjo
