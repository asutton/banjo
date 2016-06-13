// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "elab-declarations.hpp"
#include "parser.hpp"
#include "printer.hpp"

#include <banjo/ast.hpp>
#include <banjo/declaration.hpp>
#include <banjo/debugging.hpp>

#include <iostream>


namespace banjo
{

namespace fe
{ 


// Adjust the type of the variable by (possibly) parsing its type.
void
Elaborate_declarations::on_variable_declaration(Variable_decl& d)
{
  d.type_ = &get_type(d.type());
  check(d);
}


// Before entering the function, adjust its type based on the adjusted
// types of the parameters.
void
Elaborate_declarations::enter_function_declaration(Function_decl& d)
{
  Type_list ts;
  for (Decl& d : d.parameters())
    ts.push_back(cast<Typed_decl>(d).type());
  Type& ret = get_type(d.return_type());
  d.type_ = &cxt.get_function_type(std::move(ts), ret);
  check(d);
}


// Adjust the type of the parameter.
void
Elaborate_declarations::on_parameter(Variable_parm& d)
{
  d.type_ = &get_type(d.type());
  check(d);
}


// Make sure that we don't have any conflicting declarations.
void
Elaborate_declarations::start_class_declaration(Class_decl& d)
{
  check(d);
}


// -------------------------------------------------------------------------- //
// Types

// Parse the type as needed.
//
// TODO: Probably not specific to this function, but a general idea. If t
// depends on an un-elaborated function or variable definition, then we
// elaborate that definition as needed. For overloaded functions, any 
// not-yet-typed declarations are considered non-viable.
Type&
Elaborate_declarations::get_type(Type& t)
{
  if (Unparsed_type* u = as<Unparsed_type>(&t)) {
    Save_input_location loc(cxt);
    Token_stream ts(u->tokens());
    Parser p(cxt, ts);
    return p.type();    
  }
  return t;
}


// -------------------------------------------------------------------------- //
// Declaration checking

// Lookup the declaration and compare it against all previously elaborated
// declarations in the overload set.
//
// TODO: For declarations allowing "partiality", also link those together
// so that we can unify them later on.
//
// FIXME: This doesn't really work. We would need to ensure that all
// declarations are in scope at the time we call the function. When the
// declaration is in a saved scope, that's fine. When it's not... we have
// some issues.
//
// Find the balance for saving declarations between the parser and the
// elaboration framework.
void
Elaborate_declarations::check(Decl& d)
{
  // Functions elaborated after entering their scope, so we need to adjust
  // the scope prior to checking.
  Scope* scope = &cxt.current_scope();
  if (is<Function_decl>(d))
    scope = scope->enclosing_scope();

  // Get the overload set for this declaration.
  Overload_set& ovl = *scope->lookup(d.name());

  bool ok = true;
  auto iter = ovl.begin();
  while (&*iter != &d) {
    // Trap errors, so we can diagnose as many declaration errors as possible.
    try {
      check_declarations(cxt, d, *iter);
    } 
    catch (...) {
      ok = false;
    }
    ++iter;
  }
  if (!ok)
    throw Declaration_error();
}


} // namespace fe

} // namespace banjo


