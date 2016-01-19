// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_TEMPLATE_HPP
#define BEAKER_TEMPLATE_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "ast.hpp"

namespace beaker
{


Decl& specialize_template(Context&, Template_decl&, Term_list&);


} // namespace beaker


#endif
