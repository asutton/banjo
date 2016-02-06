// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_ERROR_HPP
#define BANJO_ERROR_HPP

#include "prelude.hpp"


namespace banjo
{

// Represents a translation failure resulting from an internal
// logic error such as a failed precondition or unhandled case.
// See the macros below for simplied usage.
struct Internal_error : std::runtime_error
{
  Internal_error(std::string const& s)
    : std::runtime_error(s)
  { }

  Internal_error(char const* s)
    : std::runtime_error(s)
  { }

  template<typename... Args>
  Internal_error(char const* s, Args const&... args)
    : Internal_error(format(s, args...))
  { }
};


// Throws an internal error to indicate an unhandled case for
// the dynamic type of x.
#define banjo_unhandled_case(x) \
  throw banjo::Internal_error("{}:{}: unhandled case '{}'", __FILE__, __LINE__, lingo::type_str(x))


// Represents an error that occurs during translation. Translation
// errors occurring in certain contexts are recoverable.
struct Translation_error : std::runtime_error
{
  Translation_error(std::string const& s)
    : std::runtime_error(s)
  { }

  Translation_error(char const* s)
    : std::runtime_error(s)
  { }

  template<typename... Args>
  Translation_error(char const* s, Args const&... args)
    : Translation_error(format(s, args...))
  { }
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


// Represets a type error. Type errors occur when one type (or kind
// of type) is expected, but another type is given.
struct Type_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents an error caused by exceeding an implementation limit.
struct Limitation_error : Translation_error
{
  using Translation_error::Translation_error;
};


} // namespace banjo


#endif
