// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_CALL_HPP
#define BANJO_CORE_CALL_HPP

#include "prelude.hpp"
#include "language.hpp"


namespace banjo
{

struct Context;


// Represents a candidate for overload resolution.
struct Function_candidate
{
  Function_candidate(Function_decl& f, Expr_list const& a, bool v)
    : fn(f), args(a), viable(v)
  { }

  // Converts to true iff the candidate is viable.
  explicit operator bool() const { return viable; }

  // Returns the function declaration being called.
  Function_decl const& function() const { return fn; }
  Function_decl&       function()       { return fn; }

  // Retrns the list of converted arguments.
  Expr_list const& arguments() const { return args; }
  Expr_list&       arguments()       { return args; }

  Function_decl& fn;
  Expr_list      args;
  bool           viable;
};


// TODO: Rename this to argument_initialize and move
// it into the initialization module.
Expr_list initialize_parameters(Context&, Type_list&, Expr_list&);

Expr& build_function_call(Context&, Function_decl&, Expr_list&);


} // namespace banjo


#endif
