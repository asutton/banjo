// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_BUILTIN_HPP
#define BANJO_CORE_BUILTIN_HPP

#include "language.hpp"


namespace banjo
{

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
  Constant_decl const& builtin_compiler() const { return *comp_; }
  Constant_decl&       builtin_compiler()       { return *comp_; }

  Translation_unit* tu_;
  Class_decl*       comp_class_;
  Constant_decl*    comp_;
};


void init_builtins(Context&);

} // namespace banjo


#endif
