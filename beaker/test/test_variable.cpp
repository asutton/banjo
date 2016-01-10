// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;


int
main(int argc, char* argv[])
{
  init_tokens();

  Default_init def;
  Variable_decl v1(make_id("v1"), make_bool_type(), def);

  // FIXME: What exactly are we testing here?
  std::cout << v1 << '\n';
}
