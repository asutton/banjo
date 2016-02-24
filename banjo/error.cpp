// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "error.hpp"

#include <lingo/io.hpp>

#include <iostream>
#include <sstream>


namespace banjo
{

char const*
Compiler_error::what() const noexcept
{
  std::stringstream ss;
  ss.iword(ios_color_flag) = std::cerr.iword(ios_color_flag);
  ss << diag;
  buf = ss.str();
  return buf.c_str();
}


} // namespace banjo
