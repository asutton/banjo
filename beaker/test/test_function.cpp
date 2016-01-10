// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>


using namespace lingo;
using namespace beaker;


int
main(int argc, char* argv[])
{
  Context cxt;
  Builder build(cxt);

  Type& b = build.get_bool_type();
  Type& z = build.get_int_type();

  Decl_list ps = {
    &build.make_object_parm("p1", b),
    &build.make_object_parm("p2", z)
  };
  auto& f = build.make_function("f1", ps, z);
  std::cout << f << '\n';
}
