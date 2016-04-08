// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "test.hpp"

#include <iostream>
#include <unordered_set>


void
test_types()
{
  Type* v1 = new Void_type();
  Type* v2 = new Void_type();
  Type* b1 = new Boolean_type();

  Type* z1 = new Integer_type(true);
  Type* z2 = new Integer_type(false);

  std::unordered_set<Type*, Type_hash, Type_eq> s;
  auto x1 = s.insert(v1);
  assert(x1.second);  // inserted
  auto x2 = s.insert(v2);
  assert(!x2.second); // not inserted

  auto x3 = s.insert(b1);
  assert(x3.second); // inserted
  auto x4 = s.insert(b1);
  assert(!x4.second); // not inserted

  auto x5 = s.insert(z1);
  assert(x5.second); // inserted
  auto x6 = s.insert(z2);
  assert(x6.second); // also inserted
}


int
main(int argc, char* argv[])
{
  test_types();
}
