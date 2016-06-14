// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "intrinsic.hpp"
#include "context.hpp"
#include "debugging.hpp"

#include <iostream>


namespace banjo
{

namespace intrinsic
{


// -------------------------------------------------------------------------- //
// Boolean intrinsics

Value
eq_bool(Context& cxt, Value const& a, Value const& b)
{
  lingo_unreachable();
}


// Compare two integer expressions for inequality.
Value
ne_bool(Context& cxt, Value const& a, Value const& b)
{
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Integer functions

// Compare two integer expressions for equality.
Value
eq_int(Context& cxt, Value const& a, Value const& b)
{
  lingo_unreachable();
}


// Compare two integer expressions for inequality.
Value
ne_int(Context& cxt, Value const& a, Value const& b)
{
  lingo_unreachable();
}


// -------------------------------------------------------------------------- //
// Compiler meta functions

// Print the value to the compiler's output stream.
//
// TODO: Decorate the output with location and context or not? Probably
// not... the harder we make it to get information out of the compiler,
// the harder it is to use in novel ways.
//
// FIXME: This doesn't work quite the way I thought it would. If we take
// e as a meta variable, then it must be fully reduced, and we can't diagnose
// the actual expression. We would need some reflection over an entire
// expression in order to really do what I want. Maybe something like:
//
//    $(3 + 4)
//
// Would reflect as add(int(3), int(4)) or something like that.
Value
show_value(Context& cxt, Value const& e)
{
  // FIXME: Use the debugging facility.
  std::cerr << e << '\n';
  return Void_value{};
}


} // namespace intrinsic

} // namespace banjo
