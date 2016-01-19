// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_LOOKUP_HPP
#define BANJO_LOOKUP_HPP

#include "prelude.hpp"
#include "overload.hpp"
#include "context.hpp"

#include <lingo/environment.hpp>


namespace banjo
{


// A scope defines a maximal lexical region of text where an 
// entity  may be referred to without qualification. A scope can 
// be (but is not always) associated with a declaration.
struct Scope : Environment<Symbol const*, Overload_set>
{
  Scope()
    : decl(nullptr)
  { }

  Scope(Decl* d)
    : decl(d)
  { }

  Decl* context() const { return decl; }
  Decl* context()       { return decl; }

  Decl* decl;
};


// The scope stack maintains the current scope during elaboration.
struct Scope_stack : Stack<Scope>
{
  Scope const& current() const { return top(); }
  Scope&       current()       { return top(); }

  Scope const& global() const { return bottom(); }
  Scope&       global()       { return bottom(); }
};


Decl_list unqualified_lookup(Scope_stack&, Simple_id const&);

// Decl_list qualified_lookup(Scope&, Symbol const&);
// Decl_list argument_dependent_lookup(Scope&, Expr_list&);


} // namespace banjo


#endif
