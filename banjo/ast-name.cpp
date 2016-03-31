// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "ast_name.hpp"
#include "ast_decl.hpp"

namespace banjo
{

Concept_decl const&
Concept_id::declaration() const
{
  return cast<Concept_decl>(*decl);
}


Concept_decl&
Concept_id::declaration()
{
  return cast<Concept_decl>(*decl);
}


Template_decl const&
Template_id::declaration() const
{
  return cast<Template_decl>(*decl);
}


Template_decl&
Template_id::declaration()
{
  return cast<Template_decl>(*decl);
}


} // namespace
