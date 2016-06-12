// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_DECLARATION_HPP
#define BANJO_DECLARATION_HPP

#include "language.hpp"


namespace banjo
{

struct Context;
struct Decl;
struct Scope;


void declare(Context&, Decl&);
void declare(Context&, Scope&, Decl&);

void declare_required_expression(Context&, Expr&);


// Declaration checking.
void check_declarations(Context& cxt, Decl const&, Decl const&);
void check_declarations(Context& cxt, Object_decl const&, Object_decl const&);
void check_declarations(Context& cxt, Function_decl const&, Function_decl const&);
void check_declarations(Context& cxt, Type_decl const&, Type_decl const&);


} // namespace banjo


#endif
