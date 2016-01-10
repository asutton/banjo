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

  Decl_list ps = {
    &make_parameter("p1", make_bool_type()),
    &make_parameter("p2", make_int_type())
  };
  auto& f = make_function("f1", ps, make_int_type());
  std::cout << f << '\n';
}
