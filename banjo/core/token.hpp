// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_TOKEN_HPP
#define BANJO_CORE_TOKEN_HPP

#include "prelude.hpp"

#include <lingo/token.hpp>


namespace banjo
{

// The token kind class is an extensible enumeration. This class defines
// a set of common token kinds to support various core language facilities 
// that require knowledge of e.g., identifiers and numeric values.
struct Token_kind
{
  enum : int 
  {
    error_tok = -1,  // An invalid token
    identifier_tok,  // An identifier

    last_common_tok, // This must always be last.
  };

  Token_kind() = default;

  // Implicitly convert from int.
  Token_kind(int k)
    : kind(k)
  { }

  // Implicitly convert to int.
  operator int() const { return kind; }

  int kind;
};


} // namespace banjo


#endif
