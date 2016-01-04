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

  // Unresolved names
  void id(Name const*);
  void unqualified_id(Name const*);
  void destructor_id(Destructor_id const*);
  void operator_id(Operator_id const*);
  void conversion_id(Conversion_id const*);
  void literal_id(Literal_id const*);
  void template_id(Template_id const*);
  void qualified_id(Qualified_id const*);

  // Nested name specifiers
  void leading_name_specifier(Qualified_id const*);
  void nested_name_specifier(Qualified_id const*);

  std::ostream& os;
};

std::ostream& operator<<(std::ostream&, Name const&);

} // namespace beaker

#endif
