// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


void
test_types()
{

  Type& t1 = *new Void_type();
  Type& t2 = *new Boolean_type();
  Type& t3 = *new Integer_type();
  Type& t4 = *new Float_type();

  assert(is_equivalent(t1, t1));
  assert(is_equivalent(t2, t2));
  assert(is_equivalent(t3, t3));
  assert(is_equivalent(t4, t4));
  assert(!is_equivalent(t1, t2));
}


int
main(int argc, char* argv[])
{
  test_types();
}
