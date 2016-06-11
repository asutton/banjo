// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "token.hpp"

#include <lingo/io.hpp>


namespace banjo
{

namespace fe
{

Context::Context(Symbol_table& s)
  : banjo::Context(s)
{
  init_tokens(symbols());
}


} // namespace fe

} // namespace banjo
