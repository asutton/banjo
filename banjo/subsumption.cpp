// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "subsumption.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "constraint.hpp"
#include "normalization.hpp"
#include "substitution.hpp"
#include "hash.hpp"
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
  using Seq            = std::list<Cons const*>;
  using iterator       = Seq::iterator;
  using const_iterator = Seq::const_iterator;

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

  // Replace the term in the list with c. Note that no replacement
  // may be made if c is already in the list.
  //
  // Returns a pair (i, b) where i is an iterator and b a boolean
  // value indicating successful insertion. If b is true, then i
  // i is the position of the inserted constraint. Otherise, i the
  // iterator past the original replaced element.
  iterator replace(iterator pos, Cons const& c)
  {
    pos = erase(pos);
    auto x = insert(pos, c);
    if (x.second)
      return x.first;
    return pos;
  }

  // Replace the term in the list with c1 folowed by c2. Note that
  // no replacements may be made if c1 and c2 are already in the list.
  iterator replace(iterator pos, Cons const& c1, Cons const& c2)
  {
    pos = erase(pos);
    auto x1 = insert(pos, c1);
    auto x2 = insert(pos, c2);
    if (x1.second)
      return x1.first;
    if (x2.second)
      return x2.first;
    return pos;
  }

  iterator begin() { return seq.begin(); }
  iterator end()   { return seq.end(); }

  const_iterator begin() const { return seq.begin(); }
  const_iterator end()   const { return seq.end(); }

  Map      map;
  Seq      seq;
};


std::ostream&
operator<<(std::ostream& os, Prop_list const& cs)
{
  for (auto iter = cs.begin(); iter != cs.end(); ++iter) {
    os << **iter;
    if (std::next(iter) != cs.end())
      os << ", ";
  }
  return os;
}


// An iterator into a proposition lis.
using Prop_iter       = Prop_list::iterator;
using Prop_const_iter = Prop_list::const_iterator;


// A sequent associates a set of antecedents with a set of
// propositions, indicating a proof thereof (the consequences
// follow from the antecedents).
struct Sequent
{
  // Returns the list of antecedents.
  Prop_list const& antecedents() const { return ants; }
  Prop_list&       antecedents()       { return ants; }

  // Returns the list of consequents.
  Prop_list const& consequents() const { return cons; }
  Prop_list&       consequents()       { return cons; }

  Prop_list ants;
  Prop_list cons;
};


std::ostream&
operator<<(std::ostream& os, Sequent const& seq)
{
  return os << seq.antecedents() << " |- " << seq.consequents();
}


// A goal list is a list of sequents to be satisfied.
using Goal_list = std::list<Sequent>;
using Goal_iter = Goal_list::iterator;


// The proof class represents the current work on the proof of a single
// sequent. New work proof tasks can be created with the branch function.
struct Proof
{
  using iterator       = Goal_list::iterator;
  using const_iterator = Goal_list::const_iterator;

  // Initialize a proof state with a single, empty goal.
  Proof(Context& c)
    : cxt(c)
  {
    gs.push_back({});
  }

  Context& context() { return cxt; }

  // Returns true when the goal is empty.
  bool empty() const { return gs.empty(); }

  // Returns size of the proof.
  std::size_t size() const { return gs.size(); }

  // Returns first sequent in the front.
  Sequent const& front() const { return gs.front(); }
  Sequent&       front()       { return gs.front(); }

  // Returns the list of goals for this proof.
  Goal_list const& goals() const { return gs; }
  Goal_list&       goals()       { return gs; }

  // Insert a copy of the given goal after its position.
  iterator branch(iterator i)
  {
    return gs.insert(std::next(i), *i);
  }

  // Discharge the proof obligation referred to by i.
  iterator discharge(iterator i)
  {
    return gs.erase(i);
  }

  iterator begin() { return gs.begin(); }
  iterator end()   { return gs.end(); }

  const_iterator begin() const { return gs.begin(); }
  const_iterator end() const   { return gs.end(); }

  Context&  cxt;  // The global context
  Goal_list gs;   // Global list of goals
};


std::ostream&
operator<<(std::ostream& os, Proof const& p)
{
  os << "[\n";
  std::size_t n = 0;
  for (Sequent const& s : p.goals())
    os << "  " << n << ": " << s << '\n';
  os << "]";
  return os;
}


// -------------------------------------------------------------------------- //
// Atomicity

// Returns true if c is an atomic constraint.
inline bool
is_atomic(Cons const& c)
{
  struct fn
  {
    bool operator()(Cons const& c) const               { return true; }
    bool operator()(Concept_cons const& c) const       { return false; }
    bool operator()(Parameterized_cons const& c) const { return false; }
    bool operator()(Conjunction_cons const& c) const   { return false; }
    bool operator()(Disjunction_cons const& c) const   { return false; }
  };
  return apply(c, fn{});
}


// Returns true whe c is non-atomic.
inline bool
is_non_atomic(Cons const& c)
{
  return !is_atomic(c);
}


// Returns true when the proposition list is fully reduced.
// That is, there are no non-atomic constraints in the list.
//
// FIXME: This is property can be derived during flatttening
// and cached with each list. Do that.
inline bool
is_reduced(Prop_list const& ps)
{
  for (Cons const* c : ps)
    if (is_non_atomic(*c))
      return false;
  return true;
}


// -------------------------------------------------------------------------- //
// Subsumption memoization

// TODO: Memoize the subsumption relation.
bool
is_memoized(Context& cxt, Cons const& a, Cons const& b)
{
  return false;
}


// -------------------------------------------------------------------------- //
// Proof validation
//
// Iterate over the goals in the proof. All goals must be satsified in
// order for the proof to be valid.
//
// TODO: Goal validation is a three-valued logic. Either a goal is
// satisfied (in which case we can discharge it), not satisfied
// (in which case the proof is invalid), or unknown. This latest case
// applies only when sequents have unexpanded propositions.
//
// TODO: Experiment with memoization!


enum Validation
{
  valid_proof,
  invalid_proof,
  incomplete_proof,
};


Validation check_term(Proof&, Prop_list&, Cons const&);


std::ostream&
operator<<(std::ostream& os, Validation v)
{
  switch (v) {
  case valid_proof: return os << "valid";
  case invalid_proof: return os << "invalid";
  case incomplete_proof: return os << "incomplete";
  default: lingo_unreachable();
  }
}


// Given a sequent of the form A |- C, where A and C differ
// syntactically, try to find support for a proof of C by A.
// This essentially means that we're going to consult a list of
// other rules that could make such a proof valid.
//
// TODO: Add extra rules here.
Validation
consult_rules(Proof& p, Cons const& a, Cons const& c)
{
  return invalid_proof;
}


// Try to derive a proof for the sequent of this form:
//
//    A1, A2, ..., An |- C
//
// where C is an atomic constraint. As below, we know that C does
// not occur syntactically in the list of propositions (that's checked
// by validate(cxt, ants, c)). Therefore, we must delegate to case
// analysis to determine if there are other rules that prove C.
Validation
find_atomic_support(Proof& p, Prop_list& ants, Cons const& c)
{
  // std::cout << "SUPPORT: " << c << '\n';
  Validation r = invalid_proof;
  for (Cons const* a : ants) {
    Validation v = consult_rules(p, *a, c);
    if (v == valid_proof)
      return v;
    if (v == incomplete_proof)
      r = v;
  }
  return r;
}


// Validate against the expansion of C.
Validation
find_concept_support(Proof& p, Prop_list& ants, Concept_cons const& c)
{
  return check_term(p, ants, expand(p.context(), c));
}


// Validate against the constraint of C.
Validation
find_parametric_support(Proof& p, Prop_list& ants, Parameterized_cons const& c)
{
  return check_term(p, ants, c.constraint());
}


// A conjunction is valid iff both operands are valid.
//
// TODO: Validate the easier branch first. But what is an "easier"
// branch?
Validation
find_logical_support(Proof& p, Prop_list& ants, Conjunction_cons const& c)
{
  Validation v = check_term(p, ants, c.left());
  if (v == invalid_proof || v == incomplete_proof)
    return v;
  return check_term(p, ants, c.right());
}


// A conjunction is valid iff either oerand is valid.
//
// TODO: Validate the easier branch first. But what is an "easier"
// branch?
Validation
find_logical_support(Proof& p, Prop_list& ants, Disjunction_cons const& c)
{
  Validation v = check_term(p, ants, c.left());
  if (v == valid_proof || v == incomplete_proof)
    return v;
  return check_term(p, ants, c.right());
}


// Try to derive a proof for the sequent of this form:
//
//    A1, A2, ..., An |- C
//
// Note that C does not occur (syntactically) in the list of
// antecedents, so we must decopose C to search for a proof.
Validation
find_support(Proof& p, Prop_list& ants, Cons const& c)
{
  struct fn
  {
    Proof&     p;
    Prop_list& ants;
    Validation operator()(Cons const& c) const               { return find_atomic_support(p, ants, c); }
    Validation operator()(Concept_cons const& c) const       { return find_concept_support(p, ants, c); }
    Validation operator()(Parameterized_cons const& c) const { return find_parametric_support(p, ants, c); }
    Validation operator()(Conjunction_cons const& c) const   { return find_logical_support(p, ants, c); }
    Validation operator()(Disjunction_cons const& c) const   { return find_logical_support(p, ants, c); }
  };
  // std::cout << "DERIVE: " << c << '\n';
  return apply(c, fn{p, ants});
}

// -------------------------------------------------------------------------- //
// Proof checking

// Determine if a sequent of this form:
//
//    A1, A2, ..., An |- C
//
// denotes a valid proof.
//
// The proof is valid if any Ai proves C. The proof is invalid when
// no Ai proves C. The proof is incomplete when it is invalid by
// some Ai is non-atomic.
//
// TODO: I wonder if there's an opportunity to quickly reject a
// proof with non-reduced antecedents. That would avoid multiple
// (potentially) exponential invocations of derive(), but it would
// also likely lead to more aggressive creation of goals.
Validation
check_term(Proof& p, Prop_list& ants, Cons const& c)
{
  // If antecedent set (syntacically) contains C, then the
  // proof is valid.
  if (ants.contains(c))
    return valid_proof;

  // FIXME: Memoization?

  // Actually derive a proof of C from AS. If the result
  // is invalid, by the thre are incomplete terms, then
  // the result is incomplete.
  Validation v = find_support(p, ants, c);
  if (v == invalid_proof) {
    if (!is_reduced(ants))
      return incomplete_proof;
  }
  return v;
}


// Determine if the squent (or goal) of the form:
//
//    A1, A2, ..., An |- C1, C2, ... Cm,
//
// The sequent is a valid proof if any Ai prove any Ci. The sequent
// is invalid only when all Ai provie no Ci. The proof is incomplete
// when it is invalid, but some Ai is a non-atomic proposition.
Validation
check_goal(Proof& p, Sequent& s)
{
  Prop_list& as = s.antecedents();
  Prop_list& cs = s.consequents();
  Validation r = invalid_proof;
  for (Cons const* c : cs) {
    Validation v = check_term(p, as, *c);
    if (v == valid_proof)
      return v;
    if (v == incomplete_proof)
      r = v;
  }
  return r;
}


// Determine if p is a valid proof. A proof is valid only when all
// goals have been dischared. An invalid proof is one where any
// goal is determined to be invalid. A proof is incomplete if any
// subgoaol is incomplete.
Validation
check_proof(Proof& p)
{
  Goal_list& goals = p.goals();
  auto iter = goals.begin();
  while (!goals.empty()) {
    Validation v = check_goal(p, *iter);
    if (v == valid_proof)
      iter = p.discharge(iter);
    else if (v == invalid_proof || v == incomplete_proof)
      return v;
    ++iter;
  }
  return valid_proof;
}


// -------------------------------------------------------------------------- //
// Sequent loading
//
// These operations try to move as many propositions as possible into
// the constraint sets on the left and right of a sequent. This will
// never produce sub-goals.

Prop_iter
load_concept(Proof& p, Prop_list& props, Prop_iter iter, Concept_cons const& c)
{
  Cons const& c1 = expand(p.context(), c);
  return props.replace(iter, c1);
}


// Replace the current consequent with its operand (maybe).
// Parameterized constraints are essentially transparent, so
// they can be reduced immediately.
Prop_iter
load_parameteric(Proof& p, Prop_list& props, Prop_iter iter, Parameterized_cons const& c)
{
  Cons const& c1 = c.constraint();
  return props.replace(iter, c1);
}


// Replace the current antecedent with its operands (maybe).
template<typename T>
Prop_iter
load_logical(Proof& p, Prop_list& props, Prop_iter iter, T const& c)
{
  Cons const& c1 = c.left();
  Cons const& c2 = c.right();
  return props.replace(iter, c1, c2);
}


// -------------------------------------------------------------------------- //
// Antecedent loading

// Select an appropriate action for the current proposition.
Prop_iter
load_antecedent(Proof& p, Prop_list& props, Prop_iter iter)
{
  struct fn
  {
    Proof&     p;
    Prop_list& props;
    Prop_iter  iter;
    Prop_iter operator()(Cons const& c)               { return ++iter; }
    Prop_iter operator()(Concept_cons const& c)       { return load_concept(p, props, iter, c); }
    Prop_iter operator()(Parameterized_cons const& c) { return load_parameteric(p, props, iter, c); }
    Prop_iter operator()(Conjunction_cons const& c)   { return load_logical(p, props, iter, c); }
  };
  return apply(**iter, fn{p, props, iter});
}


// Flatten all propositions in the antecedents.
void
load_antecedents(Proof& p, Sequent& s)
{
  Prop_list& as = s.antecedents();
  for (auto iter = as.begin(); iter != as.end(); ++iter)
    iter = load_antecedent(p, as, iter);
}


void
load_antecedents(Proof& p)
{
  for (auto iter = p.begin(); iter != p.end(); ++iter)
    load_antecedents(p, *iter);
}


// -------------------------------------------------------------------------- //
// Consequent loading

// Select an appropriate action for the current proposition.
Prop_iter
load_consequent(Proof& p, Prop_list& props, Prop_iter iter)
{
  struct fn
  {
    Proof&     p;
    Prop_list& props;
    Prop_iter  iter;
    Prop_iter operator()(Cons const& c)               { return ++iter; }
    Prop_iter operator()(Concept_cons const& c)       { return load_concept(p, props, iter, c); }
    Prop_iter operator()(Parameterized_cons const& c) { return load_parameteric(p, props, iter, c); }
    Prop_iter operator()(Disjunction_cons const& c)   { return load_logical(p, props, iter, c); }
  };
  return apply(**iter, fn{p, props, iter});
}


// Flatten all propositions in the consequents.
void
load_consequents(Proof& p, Sequent& s)
{
  Prop_list& cs = s.consequents();
  for (auto iter = cs.begin(); iter != cs.end(); ++iter)
    iter = load_consequent(p, cs, iter);
}


void
load_consequents(Proof& p)
{
  for (auto iter = p.begin(); iter != p.end(); ++iter)
    load_consequents(p, *iter);
}


// -------------------------------------------------------------------------- //
// Proof expansion
//
// In a single sequent in a proof, select a disjunction for expansion.


bool
expand_antecedent(Proof& p, Goal_iter gi, Prop_iter i)
{
  if (Disjunction_cons const* d = as<Disjunction_cons>(&**i)) {
    Cons const& c1 = d->left();
    Cons const& c2 = d->right();

    Sequent& s1 = *gi;
    Sequent& s2 = *p.branch(gi);
    Prop_list& as1 = s1.antecedents();
    Prop_list& as2 = s2.antecedents();

    Prop_iter j = std::next(as2.begin(), std::distance(as1.begin(), i));

    as1.replace(i, c1);
    as2.replace(j, c2);
    return true;
  }
  return false;
}


bool
expand_antecedents(Proof& p, Goal_iter pi)
{
  Sequent& s = *pi;
  Prop_list& as = s.antecedents();
  for (auto ai = as.begin(); ai != as.end(); ++ai) {
    if (expand_antecedent(p, pi, ai))
      return true;
  }
  return false;
}


// Select, in each goal, a term to expand (and expand it).
//
// TODO: There are other interesting strategies. For example,
// we might choose to expand all concepts first.
void
expand_proof(Proof& p)
{
  for (auto pi = p.begin(); pi != p.end(); ++pi) {
    if (expand_antecedents(p, pi))
      return;
  }
}


// -------------------------------------------------------------------------- //
// Subsumption


// Returns true if a subsumes c.
//
// TODO: How do I know when I've exhuasted all opportunities.
bool
subsumes(Context& cxt, Cons const& a, Cons const& c)
{
  // Check the easy cases before setting up a proof.
  if (is_equivalent(a, c))
    return true;
  if (is_memoized(cxt, a, c))
    return true;

  // Alas... no quick check. We have to prove the implication.
  Proof p(cxt);
  Sequent& s = p.front();
  s.antecedents().insert(a);
  s.consequents().insert(c);
  std::cout << "INIT: " << s << '\n';

  // NOTE: I wonder if the current load implementation is
  // too aggressive when expanding concepts.

  // Initially load consquents.
  load_consequents(p);

  // Continue manipulating the proof state until we know that
  // the implication is valid or not.
  int n = 1;
  Validation v = valid_proof;
  do {
    // Load a round of antecedents.
    load_antecedents(p);

    std::cout << "STEP " << n << ": " << p.front() << '\n';

    // Having done that, determine if the proof is valid (or not).
    // In either case, we can stop.
    v = check_proof(p);
    std::cout << "VALID? " << v << '\n';
    if (v == valid_proof)
      return true;
    if (v == invalid_proof)
      return false;

    // Otherwise, select a term in each goal to expand.
    expand_proof(p);
    ++n;

    // TODO: Actually diagnose implementation limits. Note that the
    // real limiting factor is going to be the goal size, not
    // the step count.
    if (p.size() > 32)
      throw Limitation_error("exceeded proof subgoal limit");
    if (n > 1024)
      throw Limitation_error("exceeded proof step limit");
  } while (v == incomplete_proof);

  return false;
}


bool
subsumes(Context& cxt, Expr const& a, Expr const& c)
{
  Cons& c1 = normalize(cxt, modify(a));
  Cons& c2 = normalize(cxt, modify(c));
  return subsumes(cxt, c1, c2);
}


} // namespace banjo
