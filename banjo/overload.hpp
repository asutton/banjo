// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_OVERLOAD_HPP
#define BANJO_OVERLOAD_HPP

// Defines facilities related to overload sets.
//
// TODO: This should probably be merged into the lookup/scope rules
// rather than being an independent thing. Also, function candidate
// needs to move into the resolution module.

#include "language.hpp"


namespace banjo
{

// Represents a set of overloaded declarations. All declarations have
// the same name, scope, and kind, but may differ in their different
// types and constraints.
//
// Note that an overload set is never empty.
struct Overload_set : Decl_list
{
  using iterator       = Decl_list::iterator;
  using const_iterator = Decl_list::const_iterator;

  // Initialize the overload set with a single element.
  Overload_set(Decl& d)
    : Decl_list {&d}
  { }

  // Returns the name of the overloaded declaration.
  Name const& name() const;
  Name&       name();

  // Inserts a new declaration into the overload set. The declaration
  // shall be overloadable with all previous elements of the set.
  void insert(Decl& d) { push_back(d); }
};


// In overload resolution, this represents a candidate for the function
// call. A candidate refers to the function declaration, deduced and/or
// converted arguments, and information about viability.
//
// TODO: Store information about non-viable function candidates.
struct Function_candidate
{
  Function_candidate(Function_decl& f, Expr_list const& a, bool v)
    : fn(&f), args(a), viable(v)
  { }

  // Converts to true iff the candidate is viable.
  explicit operator bool() const { return viable; }

  // Returns the function declaration being called.
  Function_decl const& function() const { return *fn; }
  Function_decl&       function()       { return *fn; }

  // Retrns the list of converted arguments.
  Expr_list const& arguments() const { return args; }
  Expr_list&       arguments()       { return args; }

  Function_decl* fn;
  Expr_list      args;
  bool           viable;
};


bool can_overload(Decl&, Decl&);


} // namespace banjo


#endif
