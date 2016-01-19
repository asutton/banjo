// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


int
main(int argc, char* argv[])
{
  Context cxt;
  Builder build(cxt);

  Variable_decl& v1 = build.make_variable("v1", build.get_int_type());

  // FIXME: What exactly are we testing here?
  std::cout << v1 << '\n';
}
