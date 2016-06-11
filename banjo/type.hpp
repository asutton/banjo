// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_TYPE_HPP
#define BANJO_TYPE_HPP

// Tools for working with types.

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

Type& resolve_id_type(Context&, Name&);

Type& make_reference_type(Context&, Type&);
Type& make_qualified_type(Context&, Type&, Qualifier_set);

Type& make_pointer_type(Context&, Type&);
Type& make_array_type(Context&, Type&, Expr&);
Type& make_tuple_type(Context&, Type_list&);
Type& make_fresh_type(Context&);


} // namespace banjo


#endif
