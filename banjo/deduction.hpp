// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_DEDUCTION_HPP
#define BANJO_DEDUCTION_HPP

#include "prelude.hpp"
#include "substitution.hpp"
#include "ast.hpp"
#include "error.hpp"


namespace banjo
{

struct Function_type;


// Represents an template argument deduction error.
struct Deduction_failure : Translation_error
{
  using Translation_error::Translation_error;
};

bool deduce_from_type(Type&, Type&, Substitution&);
bool deduce_from_types(Type_list&, Type_list&, Substitution&);

void deduce_from_call(Type&, Type&, Substitution&);
void deduce_from_address(Type&, Type&, Substitution&);
void deduce_from_conversion(Type&, Type&, Substitution&);
void deduce_from_declaration(Type&, Type&, Substitution&);


// Return the substitution mapping paramters in t2 to arguments in t2.
inline Substitution
deduce_from_type(Type& t1, Type& t2)
{
  Substitution sub;
  try {
    deduce_from_type(t1, t2, sub);
  } catch(Deduction_failure& err) {
    sub.fail();
  }
  return sub;
}


} // namespace banjo


#endif
