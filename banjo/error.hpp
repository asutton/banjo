// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_ERROR_HPP
#define BANJO_ERROR_HPP

#include "prelude.hpp"


namespace banjo
{

// Denotes an error that occurs during translation.
struct Translation_error : std::runtime_error
{
  using std::runtime_error::runtime_error;
};


// Represents a syntactic error.
struct Syntax_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents a lookup error. Lookup errors occur when lookup
// fails to find a declaration or fails to find a declaration
// of the right kind.
struct Lookup_error : Translation_error
{
  using Translation_error::Translation_error;
};


} // namespace banjo


#endif
