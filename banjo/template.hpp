// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_TEMPLATE_HPP
#define BANJO_TEMPLATE_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "ast.hpp"

namespace banjo
{


Decl& specialize_template(Context&, Template_decl&, Term_list&);


} // namespace banjo


#endif
