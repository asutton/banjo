// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast_type.hpp"
#include "ast_expr.hpp"
#include "ast_decl.hpp"
#include "context.hpp"
#include "type.hpp"
#include "constraint.hpp"
#include "lookup.hpp"
#include "print.hpp"

#include <iostream>


namespace banjo
{

// Make a dependent function call expression.
Expr&
make_dependent_call(Context& cxt, Expr& e, Expr_list& args)
{
  Type& t = make_fresh_type(cxt);
  Expr& f = cxt.make_call(t, e, args);

  // Don't do lookup here.
  if (cxt.in_requirements())
    return f;
  if (!cxt.current_template_constraints())
    return f;

  // Determine if the constraints explicitly admit this declaration.
  Expr& cons = *cxt.current_template_constraints();
  if (Expr* ret = admit_expression(cxt, cons, f))
    return *ret;

  std::cout << "NO ADMIT\n";

  return f;
}


// Make a non-dependent call expression.
//
// FIXME: Allow calls to expressions of any function type.
//
// NOTE: The builtin function call operator requries an object-to-value
// conversion on the target. Be sure to apply that.
//
// FIXME: If e has user-defined type then we need to look for an
// overloaded operator.
Expr&
make_regular_call(Context& cxt, Expr& e, Expr_list& args)
{
  if (Reference_expr* ref = as<Reference_expr>(&e)) {
    Decl& d = ref->declaration();
    Type& t = declared_type(d);

    if (Function_type* f = as<Function_type>(&t))
      return cxt.make_call(f->return_type(), e, args);

    throw Translation_error(cxt, "'{}' is not callable", e);
  }

  banjo_unhandled_case(e);
}



Expr&
make_call(Context& cxt, Expr& e, Expr_list& args)
{
  if (is_type_dependent(e) || is_type_dependent(args))
    return make_dependent_call(cxt, e, args);
  else
    return make_regular_call(cxt, e, args);
}


} // namespace banjo
