// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

// A dumb test stub for AST experiments.

#include <iostream>

#include "ast.hpp"
#include "debugging.hpp"

#include <lingo/io.hpp>


using namespace banjo;


Allocator alloc;


Symbol const&
sym(Symbol_table& syms, char const* n)
{
  return *syms.put_identifier(Token_kind::identifier_tok, n);
}


int main()
{
  lingo::init_colors();

  Symbol_table syms;

  Type_list parms { 
    &Boolean_type::make(alloc, reference_type, meta_qual | volatile_qual),
    &Integer_type::make(alloc) 
  };

  auto& f = Function_type::make(alloc, parms, Void_type::make(alloc));
  debug(f);

  // auto& n = Simple_id::make(alloc, sym(syms, "x"));
  // debug(n);
}
