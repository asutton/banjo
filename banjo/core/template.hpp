// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_TEMPLATE_HPP
#define BANJO_CORE_TEMPLATE_HPP

#include "prelude.hpp"
#include "language.hpp"
#include "substitution.hpp"


namespace banjo
{

Decl_list get_template_parameters(Context&, Expr&);

Type&     synthesize_template_argument(Context&, Type_parm&);
Expr&     synthesize_template_argument(Context&, Value_parm&);
Decl&     synthesize_template_argument(Context&, Template_parm&);
Term&     synthesize_template_argument(Context&, Decl&);
Term_list synthesize_template_arguments(Context&, Decl_list&);

Decl& specialize_template(Context&, Template_decl&, Term_list&);
Decl& specialize_template(Context&, Template_decl&, Substitution&);


// Encapsulates the results from a partial order.
enum Partial_ordering
{
  lt_ord = -1, // The left operand is preferred
  un_ord = 0,  // Neither operand is preferred
  gt_ord = 1   // The right operand is preferred
};


bool is_more_specialized(Context&, Template_decl&, Template_decl&);
bool is_more_constrained(Context&, Template_decl&, Template_decl&);


} // namespace banjo


#endif
