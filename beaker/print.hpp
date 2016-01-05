// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_PRINT_HPP
#define BEAKER_PRINT_HPP

#include "ast.hpp"

#include <iosfwd>


namespace beaker
{

struct Printer
{
  Printer(std::ostream& os)
    : os(os)
  { }

  void operator()(Name const* n) { id(n); }
  void operator()(Type const* t) { type(t); }

  // Unresolved names
  void id(Name const*);
  void unqualified_id(Name const*);
  void unqualified_id(Simple_id const*);
  void destructor_id(Destructor_id const*);
  void operator_id(Operator_id const*);
  void conversion_id(Conversion_id const*);
  void literal_id(Literal_id const*);
  void template_id(Template_id const*);
  void qualified_id(Qualified_id const*);
  void nested_name_specifier(Qualified_id const*);

  // Types
  void type(Type const*);
  void simple_type(Type const*);
  void simple_type(Void_type const*);
  void simple_type(Boolean_type const*);
  void simple_type(Integer_type const*);
  void simple_type(Float_type const*);
  void simple_type(Auto_type const*);
  void simple_type(Decltype_type const*);
  void simple_type(Declauto_type const*);

  // Declarations

  std::ostream& os;
};

std::ostream& operator<<(std::ostream&, Name const&);
std::ostream& operator<<(std::ostream&, Type const&);

} // namespace beaker

#endif
