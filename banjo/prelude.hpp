// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_PRELUDE_HPP
#define BANJO_PRELUDE_HPP

#include <lingo/assert.hpp>
#include <lingo/utility.hpp>
#include <lingo/string.hpp>
#include <lingo/error.hpp>
#include <lingo/location.hpp>
#include <lingo/symbol.hpp>


namespace banjo
{

// Bring useful structures into scope.
using lingo::String;
using lingo::Location;
using lingo::Region;
using lingo::Symbol;
using lingo::Symbol_table;
using lingo::Generic_visitor;
using lingo::Generic_mutator;
using lingo::Diagnostic;
using lingo::Diagnostic_kind;

// And some useful functions...
using lingo::is;
using lingo::as;
using lingo::cast;
using lingo::cast_as;
using lingo::modify;

} // namespace banjo


#endif
