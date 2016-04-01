// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CXX_GENERATOR_HPP
#define BANJO_CXX_GENERATOR_HPP

// This module defines a Banjo-to-C++ code generator. This will produce a
// single translation unit (.cpp file) from the AST of a Banjo program.

#include <banjo/language.hpp>

#include <iosfwd>


namespace banjo
{

namespace cxx
{

// The generator class encapsulates the resources needed to generate the
// C++ code corresponding to a Banjo program. 
struct Generator
{
  Generator(Context& cxt, std::ostream& os)
    : cxt(cxt), os(os)
  { }

  void translation_unit();

  Context&      cxt;
  std::ostream& os;
};


} // namespace cxx

} // namespace banjo

#endif
