// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_DEDUCTION_HPP
#define BANJO_CORE_DEDUCTION_HPP

#include "language.hpp"
#include "substitution.hpp"


namespace banjo
{


// Core deduction.
bool deduce_type(Context&, Type&, Type&, Substitution&);
Substitution deduce_type(Context&, Type&, Type&);

// Deduce of an expression's type.
bool deduce_type(Context&, Type&, Expr&, Substitution&);
Substitution deduce_type(Context&, Type&, Expr&);

// Deductions from initializers.
bool deduce_from_initializer(Context&, Type&, Expr&, Substitution&);
Substitution deduce_from_initializer(Context&, Type&, Expr&);

void deduce_declared_type(Context&, Decl&, Def&);

Decl_list get_placeholders(Type&);

/*
bool deduce_from_types(Context& cxt, Type_list&, Type_list&, Substitution&);
void deduce_from_call(Context&, Decl_list&, Expr_list&, Substitution&);
void deduce_from_address(Context&, Type&, Type&, Substitution&);
void deduce_from_conversion(Context&, Type&, Type&, Substitution&);
void deduce_from_declaration(Context&, Type&, Type&, Substitution&);
*/

} // namespace banjo


#endif
