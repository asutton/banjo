// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_FE_ELAB_DECLARATIONS_HPP
#define BANJO_FE_ELAB_DECLARATIONS_HPP

#include "elaboration.hpp"

#include <banjo/language.hpp>


namespace banjo
{

namespace fe
{

struct Parser;
struct Context;


// Recursively parse and analyze the types of all declared names. After 
// elaboration, every declaration has a type, which may be a placeholder.
struct Elaborate_declarations : Basic_elaborator
{
  using Basic_elaborator::Basic_elaborator;

  void on_variable_declaration(Object_decl&);
  void on_variable_declaration(Reference_decl&);
  
  void enter_function_declaration(Function_decl&);
  
  void start_class_declaration(Class_decl&);

  void on_parameter(Object_parm&);

  Type& get_type(Type&);

  void check(Decl&);
};


} // namespace fe

} // nammespace banjo


#endif
