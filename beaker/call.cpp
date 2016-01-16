// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "call.hpp"
#include "initialization.hpp"
#include "conversion.hpp"
#include "ast.hpp"

namespace beaker
{


// Build a function call candidate for the `f` given the list of
// function arguments.
//
// TODO: Synthesize default arguments if needed.
void
build_call(Context& cxt, Function_decl& f, Expr_list& args)
{
  Type_seq& parms = f.type().parameter_types();

  // TODO: Handle default arguments here.
  if (args.size() < parms.size())
    throw std::runtime_error("too few arguments");

  Init_list inits;
  std::size_t ti = 0;
  std::size_t ai = 0;
  while (t1 != parms.size() && ai != args.size()) {
    Type& parm = parms[ti];
    Expr& arg = args[ai];

    // TODO: Trap errors and use these to explain failures.
    Init& init = copy_initialize(cxt, parm, arg);
    inits.push_back(init);

    // TODO: The advancement of arguments depends on the
    // kind of parameter. If t1 is the variadic parameter,
    // then don't advance it.
    ++ti;
    ++ai;
  }
}


} // namespace beaker


#endif
