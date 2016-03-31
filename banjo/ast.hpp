// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_HPP
#define BANJO_AST_HPP

#include "ast-base.hpp"
#include "ast-name.hpp"
#include "ast-type.hpp"
#include "ast-expr.hpp"
#include "ast-stmt.hpp"
#include "ast-decl.hpp"
#include "ast-def.hpp"
#include "ast-req.hpp"
#include "ast-cons.hpp"


namespace banjo
{


// -------------------------------------------------------------------------- //
// Miscellaneous

// TODO: I'm not currently using this, but it might be useful.
struct Translation_unit : Term
{
  Decl_list first;
};


} // namespace banjo


#endif
