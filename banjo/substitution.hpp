// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CORE_SUBSTITUTION_HPP
#define BANJO_CORE_SUBSTITUTION_HPP

#include "prelude.hpp"
#include "language.hpp"


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
  Substitution(Type_list&);
  Substitution(Decl_list&);
  Substitution(Decl_list&, Term_list&);

  void seed_with(Type& d);
  void seed_with(Decl& d);
  void map_to(Decl& d, Term& t);

  // Returns the mapping for this parameter. Note that the mapping
  // can be null, indicating that the a value has not been deduced
  // or assigned. Behavior is undefined if has_mapping(d) is false.
  Term const* get_mapping(Decl& d) const;
  Term*       get_mapping(Decl& d);

  // Returns true if there is a mapping for this parameter.
  bool has_mapping(Decl&) const;

  // Returns true if the mapping is incomplete (i.e., has declarations)
  // not mapped to values.
  bool is_incomplete() const;

  Decl_list parameters() const;
  Term_list arguments() const;

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


// Initialize the substitution with an empty mapping for each type in the
// given list.
inline
Substitution::Substitution(Type_list& ts)
{
  for (Type& t : ts)
    seed_with(t);
}


// Initialize the substitution with an empty mapping for each declaration 
// in the given list.
inline
Substitution::Substitution(Decl_list& p)
{
  for (Decl& d : p)
    seed_with(d);
}


// Initialize the substitution with a mapping from each `pi` in `p` to 
// its corresponding `ai` in `a`.
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


// Insert an unmapped declaration into the set.
//
// TODO: Verify that any prior seeding is unmapped.
inline void
Substitution::seed_with(Decl& d)
{
  emplace(&d, nullptr);
}


// Create a substitution that sends each occurrence of `d` to
// its a corresponding `t`. If t is mapped to nothing (a nullptr),
// then, re-mapping is allowed (to support deduction). Otherwise,
// a declaration shall not be re-mapped to a different value.
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


inline bool
Substitution::is_incomplete() const
{
  for (auto const& x : *this)
    if (x.second == nullptr)
      return true;
  return false;
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


// Returns the list of parameters in the substitution.
inline Decl_list
Substitution::parameters() const
{
  Decl_list ds;
  for (auto const& x : *this)
    ds.push_back(modify(*x.first));
  return ds;
}


// Returns the list of arguments in the substitution.
inline Term_list
Substitution::arguments() const
{
  Term_list ts;
  for (auto const& x : *this)
    ts.push_back(modify(*x.second));
  return ts;
}


std::ostream& operator<<(std::ostream&, Substitution const&);


// -------------------------------------------------------------------------- //
// Operations

void unify(Context&, Substitution&, Substitution&);

Term& substitute(Context&, Term&, Substitution&);
Type& substitute(Context&, Type&, Substitution&);
Expr& substitute(Context&, Expr&, Substitution&);
Decl& substitute(Context&, Decl&, Substitution&);
Cons& substitute(Context&, Cons&, Substitution&);


} // namespace banjo


#endif
