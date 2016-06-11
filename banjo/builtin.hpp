// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_BUILTIN_HPP
#define BANJO_BUILTIN_HPP

// The builtin module defines a class that contains a number of
// compiler-defined objects, functions, and types.
//
// TODO: The set of builtins available to any language is defined by
// that language. It's really not appropriate to define all of these
// here. However, it is convenient.

#include "language.hpp"


namespace banjo
{

struct Context;


// The builtins class is simply a container for builtin objects.
// This is inherited by the Context, which provides access to these
// objects after initialization.
struct Builtins
{
  // Returns the translation unit.
  Translation_unit const& translation_unit() const { return *tu_; }
  Translation_unit&       translation_unit()       { return *tu_; }

  // Returns the builtin compiler class.
  Class_decl const& builtin_compiler_class() const { return *comp_class_; }
  Class_decl&       builtin_compiler_class()       { return *comp_class_; }

  // Returns the builtin compiler object.
  Mem_object_decl const& builtin_compiler() const { return *comp_; }
  Mem_object_decl&       builtin_compiler()       { return *comp_; }

  Translation_unit* tu_         = nullptr;
  Class_decl*       comp_class_ = nullptr;
  Mem_object_decl*  comp_       = nullptr;
};


void init_builtins(Context&);


} // namespace banjo


#endif
