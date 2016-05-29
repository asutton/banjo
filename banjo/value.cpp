// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "value.hpp"
#include "ast.hpp"

#include <iostream>


namespace banjo
{

// Return a string value for the aggregate. This is needed for any
// transformation to narrow string literals in the evaluation
// character set.
String
Aggregate_value::get_string() const
{
  String str(size(), 0);
  std::transform(begin(), end(), str.begin(), [](Value const& v) -> char {
    return v.get_integer();
  });
  return str;
}

// -------------------------------------------------------------------------- //
// Printing

// FIXME: The functions below vary only in their
// enclosing characters. Make a single function.

inline void
print(std::ostream& os, Aggregate_value const& v)
{
  os << '{';
  for (auto iter = v.begin(); iter != v.end(); ++iter) {
    os << *iter;
    if (std::next(iter) != v.end())
      os << ',';
  }
  os << '}';
}


std::ostream&
operator<<(std::ostream& os, Value const& v)
{
  struct Fn
  {
    std::ostream& os;

    void operator()(Error_value const& v)     { os << "<error>"; }
    void operator()(Void_value const& v)      { os << "<void>"; };
    void operator()(Integer_value const& v)   { os << v; };
    void operator()(Float_value const& v)     { os << v; };
    void operator()(Aggregate_value const& v) { print(os, v); }
    void operator()(Reference_value const& v) { os << '@' << v; };
  };
  apply(v, Fn{os});
  return os;
}


// -------------------------------------------------------------------------- //
// Zero initialization


// Set to 0.
inline void
zero_initialize(Integer_value& v)
{
  v = 0;
}


// Set to 0.
inline void
zero_initialize(Float_value& v)
{
  v = 0.0;
}


// Recursively zero initialize the aggregate.
void
zero_initialize(Aggregate_value& v)
{
  for (Value& elem : v)
    zero_initialize(elem);
}


// Zero initialize the value. Note that zero initialization of a
// reference or function (which would be a reference) does nothing.
void
zero_initialize(Value& v)
{
  struct fn
  {
    void operator()(Error_value& v)     { /* Do nothing. */ };
    void operator()(Void_value& v)      { /* Do nothing. */ };
    void operator()(Integer_value& v)   { zero_initialize(v); };
    void operator()(Float_value& v)     { zero_initialize(v); };
    void operator()(Aggregate_value& v) { zero_initialize(v); };
    void operator()(Reference_value& v) { /* Do nothing. */ }
  };
  apply(v, fn{});
}


} // namespace banjo
