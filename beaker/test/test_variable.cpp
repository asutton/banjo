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

  Namespace_decl* global = make_global_ns();
  Variable_decl* v1 = make_variable(global, "v1", make_bool_type());

  // FIXME: What exactly are we testing here?
  std::cout << *v1 << '\n';
}
