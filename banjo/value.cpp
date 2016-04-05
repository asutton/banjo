// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "value.hpp"
#include "ast.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// Return a string value for the arary. This is needed for any
// transformation to narrow string literals in the evaluation
// character set.
std::string
Array_value::get_as_string() const
{
  std::string str(len, '\0');
  std::transform(data, data + len, str.begin(), [](Value const& v) -> char {
    return (v.is_integer() ? v.get_integer() : v.get_float());
  });
  return str;
}


// -------------------------------------------------------------------------- //
// Printing

// FIXME: The functions below vary only in their
// enclosing characters. Make a single function.

inline void
print(std::ostream& os, Array_value const& v)
{
  os << '[';
  Value const* p = v.data;
  Value const* q = p + v.len;
  while (p != q) {
    os << *p;
    if (p + 1 != q)
      os << ',';
    ++p;
  }
  os << ']';
}


inline void
print(std::ostream& os, Tuple_value const& v)
{
  os << '{';
  Value const* p = v.data;
  Value const* q = p + v.len;
  while (p != q) {
    os << *p;
    if (p + 1 != q)
      os << ',';
    ++p;
  }
  os << '}';
}


std::ostream&
operator<<(std::ostream& os, Value const& v)
{
  struct Fn
  {
    std::ostream& os;

    void operator()(Error_value const& v) { os << "<error>"; }
    void operator()(Integer_value const& v) { os << v; };
    void operator()(Float_value const& v) { os << v; };
    void operator()(Function_value const& v) { os << v->name(); };
    void operator()(Reference_value const& v) { os << *v << '@' << (void*)v; };
    void operator()(Array_value const& v) { print(os, v); }
    void operator()(Tuple_value const& v) { print(os, v); }
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
  for (std::size_t i = 0; i < v.len; ++i)
    zero_initialize(v.data[i]);
}


// Zero initialize the value. Note that zero initialization of a
// reference or function (which would be a reference) does nothing.
void
zero_initialize(Value& v)
{
  struct fn
  {
    void operator()(Error_value& v)     { };
    void operator()(Integer_value& v)   { zero_initialize(v); };
    void operator()(Float_value& v)     { zero_initialize(v); };
    void operator()(Function_value& v)  { }
    void operator()(Reference_value& v) { }
    void operator()(Aggregate_value& v) { zero_initialize(v); };
  };
  apply(v, fn{});
}


} // namespace banjo
