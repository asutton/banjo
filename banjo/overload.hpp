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


// A solution to overload resolution. This contains the function and
// and its converted arguments.
struct Resolution
{
  Resolution(Function_decl& f, Expr_list const& a)
    : fn_(&f), args_(a)
  { }

  // Returns the function declaration being called.
  Function_decl const& function() const { return *fn_; }
  Function_decl&       function()       { return *fn_; }

  // Returns the list of converted arguments.
  Expr_list const& arguments() const { return args_; }
  Expr_list&       arguments()       { return args_; }

  Function_decl* fn_;
  Expr_list      args_;
};


// In overload resolution, this represents a candidate for the function
// call. That is, it is a potential resolution. This type extends the
// resolution with bookkeeping information used to support resolution.
//
// TODO: Store information about non-viable function candidates.
struct Function_candidate : Resolution
{
  Function_candidate(Function_decl& f, Expr_list const& a, bool v)
    : Resolution(f, a), viable_(v)
  { }

  // Converts to true iff the candidate is viable.
  explicit operator bool() const { return viable_; }

  bool viable_;
};


bool can_overload(Decl&, Decl&);


} // namespace banjo


#endif
