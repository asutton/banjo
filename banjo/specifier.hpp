// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SPECIFIER_HPP
#define BANJO_SPECIFIER_HPP


namespace banjo
{

// A specifier set records a set of declaration specifiers.
enum Specifier_set
{
  empty_spec    = 0,
  virtual_spec  = 1 << 5,
  abstract_spec = 1 << 6
};


inline Specifier_set&
operator|=(Specifier_set& a, Specifier_set b)
{
  return a = Specifier_set(a | b);
}


} // namesapce banjo

#endif
