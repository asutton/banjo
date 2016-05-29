// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_QUALIFIER_HPP
#define BANJO_QUALIFIER_HPP


namespace banjo
{

// Represents a set of type qualifiers.
enum Qualifier_set
{
  empty_qual    = 0,
  const_qual    = 1 << 0,
  volatile_qual = 1 << 1,
  cv_qual       = const_qual | volatile_qual
};


inline Qualifier_set&
operator|=(Qualifier_set& a, Qualifier_set b)
{
  return a = Qualifier_set(a | b);
}


// Returns true if a is a superset of b.
inline bool
is_superset(Qualifier_set a, Qualifier_set b)
{
  return (a & b) == b;
}


// Returns true if a is strictly more qualified than b.
inline bool
is_more_qualified(Qualifier_set a, Qualifier_set b)
{
  return is_superset(a, b) && a != b;
}


} // namesapce banjo

#endif
