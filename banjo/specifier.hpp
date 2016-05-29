// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SPECIFIER_HPP
#define BANJO_SPECIFIER_HPP


namespace banjo
{

// A specifier set records a set of declaration specifiers.
enum Specifier_set
{
  empty_spec     = 0,
  static_spec    = 1 << 0,
  dynamic_spec   = 1 << 1,  // TODO: Find a use.
  implicit_spec  = 1 << 2,  // TODO: Find a use.
  explicit_spec  = 1 << 3,
  virtual_spec   = 1 << 4,
  abstract_spec  = 1 << 5,
  inline_spec    = 1 << 6,
  public_spec    = 1 << 7,
  private_spec   = 1 << 8,
  protected_spec = 1 << 9,
  ref_spec       = 1 << 10,
  consume_spec   = 1 << 11,
  in_spec        = 1 << 12,
  out_spec       = 1 << 13,
  mutable_spec   = 1 << 14,
  forward_spec   = 1 << 15,

  const_spec     = 1 << 16, // FIXME: Replaced by meta
  meta_spec      = 1 << 16,
  
  internal_spec  = 1 << 30, // Internal to the language
};


inline Specifier_set&
operator|=(Specifier_set& a, Specifier_set b)
{
  return a = Specifier_set(a | b);
}


} // namespace banjo

#endif
