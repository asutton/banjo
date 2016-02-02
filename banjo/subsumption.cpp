// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "subsumption.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "hash.hpp"
#include "equivalence.hpp"
#include "print.hpp"

#include <unordered_set>
#include <iostream>


namespace banjo
{


// -------------------------------------------------------------------------- //
// Proof structures

struct Sequent : std::unordered_set<Cons const*, Cons_hash, Cons_eq>
{
  using Base = std::unordered_set<Cons const*, Cons_hash, Cons_eq>;

  void insert(Cons const& c) { Base::insert(&c); }
};


// Stores lists of terms.
struct Goals : std::vector<Sequent>
{
  Goals() { push_back({}); }
};


// Used as a parameter type for decomposition.
struct State
{
  Context&    cxt;
  Goals&      goals;
  std::size_t seq;

  // Return the current sequence.
  Sequent& sequent() { return goals[seq]; }

  // Insert a new sequent into the goals et, and return that.
  State branch()
  {
    goals.push_back(sequent());
    return State{cxt, goals, goals.size() - 1};
  }
};


// -------------------------------------------------------------------------- //
// Right-side decomposition

/*
bool
subsumes_right(Context& cxt, Cons const& a, Concept_cons const& b)
{
  lingo_unimplemented();
}


bool
subsumes_right(Context& cxt, Cons const& a, Predicate_cons const& b)
{
  lingo_unimplemented();
}


bool
subsumes_right(Context& cxt, Cons const& a, Conjunction_cons const& b)
{
  return subsumes_right(a, b.left()) && subsumes_right(a, b.right());
}


bool
subsumes_right(Context& cxt, Cons const& a, Disjunction_cons const& b)
{
  return subsumes_right(a.right(), b) || subsumes_right(a.right(), b);
}


// I think we should only be getting here from atomic constraints.
bool
subsumes_right(Context& cxt, Cons const& a, Cons const& b)
{
  struct fn
  {
    Cons const& b;
    bool operator()(Cons const& a) { lingo_unimplemented(); }
    bool operator()(Concept_cons const& a)     { return subsumes_right(a, b); }
    bool operator()(Predicate_cons const& a)   { return subsumes_right(a, b); }
    bool operator()(Conjunction_cons const& a) { return subsumes_right(a, b); }
    bool operator()(Disjunction_cons const& a) { return subsumes_right(a, b); }
  };
  return apply(a, fn{cxt, b});
}
*/


// -------------------------------------------------------------------------- //
// Left-side decomposition
//
// The state could be passed by value...

void decompose(State&, Cons const&);


void
save(State& s, Cons const& c)
{
  s.sequent().insert(c);
}


void
decompose(State& s, Conjunction_cons const& c)
{
  decompose(s, c.left());
  decompose(s, c.right());
}


void
decompose(State& s1, Disjunction_cons const& c)
{
  State s2 = s1.branch();
  decompose(s1, c.left());
  decompose(s2, c.right());
}


void
decompose(State& s, Cons const& c)
{
  struct fn
  {
    State& s;
    void operator()(Cons const& c)             { save(s, c); }
    void operator()(Conjunction_cons const& c) { decompose(s, c); }
    void operator()(Disjunction_cons const& c) { decompose(s, c); }
  };
  return apply(c, fn{s});
}


// Return the normalized constraint of an expression.
bool
subsumes(Context& cxt, Cons const& a, Cons const& c)
{
  Goals g;
  State s {cxt, g, 0};
  decompose(s, a);
  return false;
}


} // namespace banjo
