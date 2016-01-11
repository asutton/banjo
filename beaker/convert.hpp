// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_CONVERT_HPP
#define BEAKER_CONVERT_HPP

#include "prelude.hpp"
#include "ast.hpp"


namespace beaker
{

struct Type;
struct Expr;


using Qualifier_list = std::vector<int>;

Expr&     convert_to_type(Expr const&, Type const&);
Expr_pair convert_to_common(Expr const&, Expr const&);

bool is_similar(Type const&, Type const&);

Qualifier_list get_qualification_signature(Type const&);



} // namespace beaker


#endif
