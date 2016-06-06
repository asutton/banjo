// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "intrinsic.hpp"
#include "context.hpp"
#include "debugging.hpp"


namespace banjo
{

namespace intrinsic
{

// Print the value to the compiler's output stream.
//
// TODO: Decorate the output with location and context or not? Probably
// not... the harder we make it to get information out of the compiler,
// the harder it is to use in novel ways.
Expr& 
show_value(Context& cxt, Expr& e)
{
  std::cout << debug(e) << '\n';
  return cxt.get_void();
}


} // namespace intrinsic

} // namespace banjo
