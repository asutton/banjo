// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_AST_HPP
#define BANJO_AST_HPP

#include "ast_base.hpp"
#include "ast_name.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "ast_stmt.hpp"
#include "ast_decl.hpp"
#include "ast_def.hpp"
#include "ast_req.hpp"
#include "ast_cons.hpp"


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
