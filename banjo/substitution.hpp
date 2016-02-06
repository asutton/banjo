// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_SUBSTITUTION_HPP
#define BANJO_SUBSTITUTION_HPP

#include "prelude.hpp"
#include "context.hpp"
#include "ast.hpp"


namespace banjo
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
  Substitution();
  Substitution(Decl_list&);
  Substitution(Decl_list&, Term_list&);

  void map_to(Decl& d, Term& t);

  // Returns the mapping for this parameter. Note that the mapping
  // can be null, indicating that the a value has not been deduced
  // or assigned. Behavior is undefined if has_mapping(d) is false.
  Term const* get_mapping(Decl& d) const;
  Term*       get_mapping(Decl& d);

  // Returns true if there is a mapping for this parameter.
  bool has_mapping(Decl&) const;

  // Contextually convert to true whe the substitution is valid.
  explicit operator bool() const { return ok; }

  // Invalidate the substitution.
  void fail() { ok = false; }

  bool ok; // Used to invalidate a substitution.
};


// Initialize an empty substitution.
inline
Substitution::Substitution()
  : ok(true)
{ }


// Initialize a partial substitution for the purpose of deducing
// mappings to parameters. Initially map each parameter to a null
// pointer.
inline
Substitution::Substitution(Decl_list& p)
{
  for (Decl& d : p)
    insert({&d, nullptr});
}


// Initialize the substitution with a mapping from each
// `pi` in `p` to its corresponding `ai` in `a`.
inline
Substitution::Substitution(Decl_list& p, Term_list& a)
  : ok(true)
{
  auto pi = p.begin();
  auto ai = a.begin();
  while (pi != p.end()) {
    map_to(*pi, *ai);
    ++pi;
    ++ai;
  }
}


// Create a substitution that sends each occurrence of `d` to
// its a corresponding `t`. If t is mapped to nothing (a nullptr),
// then, re-mapping is allowed (to support deduction). Otherwise,
// a declaration shall not be re-mapped to a different value.
//
inline void
Substitution::map_to(Decl& d, Term& t)
{
  auto iter = find(&d);
  if (iter == end()) {
    emplace(&d, &t);
  } else {
    lingo_assert(!iter->second);
    iter->second = &t;
  }
}


inline bool
Substitution::has_mapping(Decl& d) const
{
  return count(&d) != 0;
}


inline Term const*
Substitution::get_mapping(Decl& d) const
{
  return find(&d)->second;
}


inline Term*
Substitution::get_mapping(Decl& d)
{
  return find(&d)->second;
}


std::ostream& operator<<(std::ostream&, Substitution const&);


// -------------------------------------------------------------------------- //
// Operations

Term& substitute(Context&, Term&, Substitution&);
Type& substitute(Context&, Type&, Substitution&);
Expr& substitute(Context&, Expr&, Substitution&);
Decl& substitute(Context&, Decl&, Substitution&);
Cons& substitute(Context&, Cons&, Substitution&);


} // namespace banjo


#endif
