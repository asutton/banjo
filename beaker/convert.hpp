// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_CONVERT_HPP
#define BEAKER_CONVERT_HPP

#include "prelude.hpp"


namespace beaker
{

struct Type;
struct Expr;


Expr& convert(Expr const&, Type const&);

bool is_similar(Type const&, Type const&);


using Qualifier_list = std::vector<int>;

Qualifier_list get_qualification_signature(Type const&);


} // namespace beaker


#endif
