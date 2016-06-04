// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "builder.hpp"


namespace banjo
{


Translation_unit&
Builder::make_translation_unit()
{
  return translation_unit();
  return make<Translation_unit>();
}


Translation_unit&
Builder::make_translation_unit(Stmt_list&& ss)
{
  translation_unit(std::move(ss));
}


}
