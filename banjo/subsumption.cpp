// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "subsumption.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "hash.hpp"
#include "equivalence.hpp"
#include "print.hpp"

#include <list>
#include <unordered_set>
#include <iostream>


namespace banjo
{


// -------------------------------------------------------------------------- //
// Proof structures

// A list of propositions (constraints). These are accumulated on either
// side of a sequent. This is actually a list equipped with a side-table
// to optimize list membership.
//
// FIXME: An unordered map on constraint *values* will be inefficient.
// We really want to work on constraint identities, which means we
// need to canonicalize constraints.
struct Prop_list
{
  using Seq = std::list<Cons const*>;
  using iterator = Seq::iterator;

  using Map = std::unordered_map<Cons const*, iterator, Cons_hash, Cons_eq>;


  // Returns true if the list has a constraint that is identical
  // to c.
  bool contains(Cons const& c) const
  {
    return map.count(&c) != 0;
  }

  // Insert a new constraint. No action is taken if the constraint
  // is already in the set. Returns the index of the added
  // constraint or that of the original constraint.
  std::pair<iterator, bool> insert(Cons const& c)
  {
    auto iter = map.find(&c);
    if (iter != map.end()) {
      return {iter->second, false};
    } else {
      auto pos = seq.insert(seq.end(), &c);
      map.emplace(&c, pos);
      return {pos, true};
    }
  }

  // Positionally insert the constraint before pos. This does nothing if
  // c is in the map, returing the same iterator.
  std::pair<iterator, bool> insert(iterator pos, Cons const& c)
  {
    auto iter = map.find(&c);
    if (iter != map.end()) {
      return {pos, false};
    } else {
      pos = seq.insert(pos, &c);
      map.emplace(&c, pos);
      return {pos, true};
    }
  }

  // Erase the constraint from the list. This nulls the entry in the
  // sequence, so it must be replaced.
  iterator erase(iterator pos)
  {
    map.erase(*pos);
    return seq.erase(pos);
  }

  // Replace the term in the list with these two, int the
  // order given. Note that this original term may be dropped
  // from the list if c1 and c2 are already in the list.
  //
  // This returns the value of the first inserted value if any.
  // If no values are inserted, return the position after
  // the erased element.
  std::pair<iterator, bool> replace(iterator pos, Cons const& c1, Cons const& c2)
  {
    pos = erase(pos);
    auto x1 = insert(pos, c1);
    auto x2 = insert(pos, c2);
    if (x1.second)
      return x1;
    if (x2.second)
      return x2;
    return {pos, false};
  }

  // Iterators
  iterator begin() { return seq.begin(); }
  iterator end()   { return seq.end(); }

  Map map;
  Seq seq;
};


// An iterator into a proposition lis.
using Prop_iter = Prop_list::iterator;


// A sequent associates a set of antecedents with a set of
// propositions, indicating a proof thereof (the consequences
// follow from the antecedents).
struct Sequent
{
  // Create a sequent having the antecedent a and the consequent c.
  Sequent(Cons const& a, Cons const& c)
  {
    ants.insert(a);
    cons.insert(c);
    ai = ants.begin();
    ci = cons.begin();
  }

  Prop_list& antecedents() { return ants; }
  Prop_list& consequents() { return cons; }

  Prop_iter current_antecedent() const { return ai; }
  Prop_iter current_consequent() const { return ci; }

  Prop_list ants;
  Prop_list cons;

  Prop_iter ai; // The current sequent
  Prop_iter ci; // The current consequent
};


// The goal list stores the current set of goals in the syntactic
// proof of a sequent. Proof tactics manipulate the goal list.
//
// Proof strategies must attempt to minimize the creation of
// sub-goals in the proof.
struct Goal_list : std::vector<Sequent>
{
  // Copy semantics
  Goal_list(Goal_list const&) = default;
  Goal_list& operator=(Goal_list const&) = default;

  // Initalize with a single sequent s.
  Goal_list(Sequent&& s)
    : std::vector<Sequent>{std::move(s)}
  { }
};


// The proof class represents the current work on the proof of a single
// sequent. New work proof tasks can be created with the branch function.
struct Proof
{
  Proof(Context& c, Goal_list& g, std::size_t n = 0)
    : cxt(c), goals(g), seq(n)
  { }

  // Return the current sequence.
  Sequent& sequent() { return goals[seq]; }

  // Return the antecedents or consequents of the current sequent.
  Prop_list& antecedents() { return sequent().antecedents(); }
  Prop_list& consequents() { return sequent().consequents(); }

  Prop_iter current_antecedent() { return sequent().current_antecedent(); }
  Prop_iter current_consequent() { return sequent().current_consequent(); }

  Prop_iter next_antecedent() { return ++sequent().ai; }
  Prop_iter next_consequent() { return ++sequent().ci; }

  // Insert a new sequent into the goals et, and return that.
  //
  // NOTE: If we can branch, can we ever join (i.e. discharge) a
  // proof obligation.
  Proof branch()
  {
    // Create a copy of the current sequence.
    goals.push_back(sequent());

    // And create a new
    return Proof(cxt, goals, goals.size() - 1);
  }

  Context&    cxt;   // The global context
  Goal_list&  goals; // Global list of goals
  std::size_t seq;   // The active sequent
};


// -------------------------------------------------------------------------- //
// Proof validation
//
// Iterate over the goals in the proof. All goals must be satsified in
// order for the proof to be valid.
//
// TODO: Experiment with memoization!


// Validate a sequent of the form A |- C when A and C differ
// syntactically.
//
// TODO: Implement me.
bool
validate_extended_implication(Context& cxt, Cons const& a, Cons const& c)
{
  return false;
}


// Validate a sequent of the form A1, A2, ..., An |- C when Ai and C differs
// syntactically from any Ai.
//
// TODO: Re-enable this loop.
bool
validate_extended_implication(Context& cxt, Prop_list& as, Cons const& c)
{
  return false;
  // for (Cons const& a : as)
  //   if (validate_extended_implication(cxt, as, c))
  //     return true;
  // return false;
}


// Validate a sequent of the form A |- C when it is unknown whether
// A and C are equivalent constraints.
bool
validate_simple_implication(Context& cxt, Cons const& a, Cons const& c)
{
  if (is_equivalent(a, c))
    return true;
  else
    return validate_extended_implication(cxt, a, c);
}


// Given a list of antecedents and a conclusion, determine if
// A1, A2, ..., An |- C. This is valid if any Ai proves C (i.e., they
// are equivalent).
bool
validate_simple_implication(Context& cxt, Prop_list& as, Cons const& c)
{
  if (as.contains(c))
    return true;
  else
    return validate_extended_implication(cxt, as, c);
}


// Given a sequent of the form A1, A2, ..., An |- C1, C2, ... Cm,
// returns true if any Ci is proven by all Ai.
bool
validate_obligation(Context cxt, Sequent& s)
{
  Prop_list& as = s.antecedents();
  Prop_list& cs = s.consequents();
  auto contains = [&cxt, &as](Cons const* c) { return as.contains(*c); };
  return std::any_of(cs.begin(), cs.end(), contains);
}


// Verify that all proof goals are satisfied.
bool
validate_proof(Context& cxt, Proof& p)
{
  auto check =[&cxt](Sequent& s) { return validate_obligation(cxt, s); };
  return std::all_of(p.goals.begin(), p.goals.end(), check);
}



// -------------------------------------------------------------------------- //
// Left-side decomposition
//
// The state could be passed by value...

Prop_iter flatten_left(Proof, Cons const&);


// Do nothing for atomic constraints.
Prop_iter
flatten_left_atom(Proof p, Cons const& c)
{
  return p.next_antecedent();
}


Prop_iter
flatten_left_conjunction(Proof p, Conjunction_cons const& c)
{
  Prop_list& cs = p.antecedents();
  auto x = cs.replace(p.current_antecedent(), c.left(), c.right());
  return x.first;
}


// DO NOTHING. This would require branching. Just advance to
// the next proposition.
Prop_iter
flatten_left_disjunction(Proof p, Disjunction_cons const& c)
{
  return p.next_antecedent();
}


// Select an appropriate action for the current proposition.
Prop_iter
flatten_left(Proof p, Cons const& c)
{
  struct fn
  {
    Proof p;
    Prop_iter operator()(Cons const& c)             { return flatten_left_atom(p, c); }
    Prop_iter operator()(Conjunction_cons const& c) { return flatten_left_conjunction(p, c); }
    Prop_iter operator()(Disjunction_cons const& c) { return flatten_left_disjunction(p, c); }
  };
  return apply(c, fn{p});
}


// Recursively flatten the left-side of the current sequent.
void
flatten_left(Proof p)
{
  Prop_list& cs = p.antecedents();
  auto iter = p.current_antecedent();
  while (iter != cs.end())
    iter = flatten_left(p, **iter);
}


// Return the normalized contraint of an expression.
//
// TODO: How do I know when I've exhuasted all opportunities.
bool
subsumes(Context& cxt, Cons const& a, Cons const& c)
{
  // Fastest possible discharge.
  if (validate_simple_implication(cxt, a, c))
    return true;

  // Otherwise, we're going to have to do analysis.
  Goal_list goals(Sequent(a, c));
  Proof p(cxt, goals);

  // Initially flatten both sides of the proof.
  flatten_left(p);


  return false;
}


} // namespace banjo
