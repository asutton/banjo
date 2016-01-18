// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_SUBSTITUTION_HPP
#define BEAKER_SUBSTITUTION_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "ast.hpp"


namespace beaker
{

// -------------------------------------------------------------------------- //
// Substitution

// A substitution is a mapping of declared parameters to substituted
// arguments.
//
// This mapping is general. We assume that the kind and type of
// arguments match their corresponding declarations.
//
// Note that declarations are guaranteed to be unique, so we can
// hash on identity rather than syntax.
struct Substitution : std::unordered_map<Decl*, Term*>
{
  using std::unordered_map<Decl*, Term*>::unordered_map;

  void put(Decl& d, Term& t);

  Term const* get(Decl& d) const;
  Term*       get(Decl& d);
};


// Create a substitution for the parameter d with t.
//
// FIXME: If we use this during template argument deduction, do we
// need this to act as a unifier (i.e. reject re-mappings of
// previously deduced terms?). Probably.
inline void
Substitution::put(Decl& d, Term& t)
{
  emplace(&d, &t);
}


// Returns the term substituted for the declaration d
// or nullptr if there is no such term.
inline Term const*
Substitution::get(Decl& d) const
{
  auto iter = find(&d);
  if (iter != end())
    return iter->second;
  else
    return nullptr;
}


inline Term*
Substitution::get(Decl& d)
{
  auto iter = find(&d);
  if (iter != end())
    return iter->second;
  else
    return nullptr;
}


// -------------------------------------------------------------------------- //
// Operations

Type& substitute(Context&, Type&, Substitution&);
Expr& substitute(Context&, Expr&, Substitution&);
Decl& substitute(Context&, Decl&, Substitution&);


} // namespace beaker


#endif
