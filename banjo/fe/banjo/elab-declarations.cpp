// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "elab-declarations.hpp"
#include "parser.hpp"

#include <banjo/ast.hpp>


namespace banjo
{

namespace fe
{ 


// Adjust the type of the variable by (possibly) parsing its type.
void
Elaborate_declarations::on_variable_declaration(Object_decl& d)
{
  d.type_ = &parse_type(d.type());
}


// TODO: Make sure that the adjusted type is actually a reference.
void
Elaborate_declarations::on_variable_declaration(Reference_decl& d)
{
  d.type_ = &parse_type(d.type());
}


// Before entering the function, adjust its type based on the adjusted
// types of the parameters.
void
Elaborate_declarations::enter_function_declaration(Function_decl& decl)
{
  Type_list ts;
  for (Decl& d : decl.parameters())
    ts.push_back(cast<Typed_decl>(d).type());
  Type& ret = parse_type(decl.return_type());
  decl.type_ = &cxt.get_function_type(std::move(ts), ret);
}


// Adjust the type of the parameter.
void
Elaborate_declarations::on_parameter(Object_parm& p)
{
  p.type_ = &parse_type(p.type());
}


// Make sure that we don't have any conflicting declarations.
void
Elaborate_declarations::start_class_declaration(Class_decl& p)
{
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
Elaborate_declarations::parse_type(Type& t)
{
  if (Unparsed_type* u = as<Unparsed_type>(&t)) {
    Save_input_location loc(cxt);
    Token_stream ts(u->tokens());
    Parser p(cxt, ts);
    return p.type();    
  }
  return t;
}


} // namespace fe

} // namespace banjo

