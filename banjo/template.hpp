// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_TEMPLATE_HPP
#define BANJO_TEMPLATE_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "ast.hpp"


namespace banjo
{

Type&     synthesize_template_argument(Context&, Type_parm&);
Expr&     synthesize_template_argument(Context&, Value_parm&);
Decl&     synthesize_template_argument(Context&, Template_parm&);
Term&     synthesize_template_argument(Context&, Decl&);
Term_list synthesize_template_arguments(Context&, Decl_list&);

Decl& specialize_template(Context&, Template_decl&, Term_list&);
bool  more_specialized(Context&, Template_decl&, Template_decl&);
bool  more_constrained(Context&, Template_decl&, Template_decl&);


} // namespace banjo


#endif
