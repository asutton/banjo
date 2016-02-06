// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "subsumption.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "substitution.hpp"
#include "constraint.hpp"
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
  std::pair<iterator, bool> replace(iterator pos, Cons const& c)
  {
    pos = erase(pos);
    auto x = insert(pos, c);
    if (x.second)
      return x;
    return {pos, false};
  }

  // Replace the term in the list with c1 folowed by c2. Note that
  // no replacements may be made if c1 and c2 are already in the list.
  //
  // Returns a pair (i, b) where i is an iterator and b a boolean
  // value indicating successful insertion. If b is true, then i
  // i is the position of the first inserted constraint. Otherwise,
  // i is the iterator past the original replaced element.
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
  iterator start()   { return (cur = seq.begin()); }
  iterator advance() { return ++cur; }
  iterator current() { return cur; }

  iterator begin() { return seq.begin(); }
  iterator end()   { return seq.end(); }

  const_iterator begin() const { return seq.begin(); }
  const_iterator end()   const { return seq.end(); }

  Map      map;
  Seq      seq;
  iterator cur;
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

  // Create a sequent having the antecedent a and the consequent c.
  Sequent(Cons const& a, Cons const& c)
  {
    ants.insert(a);
    cons.insert(c);
  }

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


// The goal list stores the current set of goals in the syntactic
// proof of a sequent. Proof tactics manipulate the goal list.
//
// Proof strategies must attempt to minimize the creation of
// sub-goals in the proof.
struct Goal_list : std::list<Sequent>
{
  using iterator       = std::list<Sequent>::iterator;
  using const_iterator = std::list<Sequent>::const_iterator;

  // Initalize with a single sequent s.
  Goal_list(Sequent&& s)
    : std::list<Sequent>{std::move(s)}
  { }

  // Generate a new proof obligation as a copy of s, returning
  // an iterator referring to the sequent.
  iterator generate(Sequent const& s)
  {
    return insert(end(), s);
  }

  // Discharge the proof obligation referred to by i.
  iterator discharge(iterator i)
  {
    return erase(i);
  }
};


using Goal_iter = Goal_list::iterator;


// The proof class represents the current work on the proof of a single
// sequent. New work proof tasks can be created with the branch function.
struct Proof
{
  Proof(Context& c, Goal_list& g)
    : cxt(c), gs(g), iter(g.begin())
  { }

  Proof(Context& c, Goal_list& g, Goal_iter i)
    : cxt(c), gs(g), iter(i)
  { }

  Context& context() { return cxt; }

  // Returns the list of goals for this proof.
  Goal_list const& goals() const { return gs; }
  Goal_list&       goals()       { return gs; }

  // Return the current goal (sequent).
  Sequent& sequent() { return *iter; }

  // Return the antecedents or consequents of the current sequent.
  Prop_list& antecedents() { return sequent().antecedents(); }
  Prop_list& consequents() { return sequent().consequents(); }

  // Insert a new sequent into the goals et, and return that.
  //
  // NOTE: If we can branch, can we ever join (i.e. discharge) a
  // proof obligation.
  Proof branch()
  {
    // Create a *copy* of the current sequence.
    auto iter = gs.insert(gs.end(), sequent());

    // And yield a new proof object.
    return Proof(cxt, gs, iter);
  }

  Context&   cxt;  // The global context
  Goal_list& gs;   // Global list of goals
  Goal_iter  iter; // The current goal
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


Validation  validate(Context&, Cons const&, Cons const&);
Validation  validate(Context&, Prop_list&, Cons const&);
Validation  match(Context&, Prop_list&, Cons const&);


// Given a sequent of the form A |- C, where A and C differ
// syntactically, try to find support for a proof of C by A.
// This essentially means that we're going to consult a list of
// other rules that could make such a proof valid.
//
// TODO: Add extra rules here.
Validation
find_support(Context& cxt, Cons const& a, Cons const& c)
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
find_support(Context& cxt, Prop_list& ants, Cons const& c)
{
  // std::cout << "SUPPORT: " << c << '\n';
  Validation r = invalid_proof;
  for (Cons const* a : ants) {
    Validation v = find_support(cxt, *a, c);
    if (v == valid_proof)
      return v;
    if (v == incomplete_proof)
      r = v;
  }
  return r;
}


// Validate against the expansion of C.
Validation
derive(Context& cxt, Prop_list& ants, Concept_cons const& c)
{
  return validate(cxt, ants, expand(cxt, c));
}


// Validate against the constraint of C.
Validation
derive(Context& cxt, Prop_list& ants, Parameterized_cons const& c)
{
  return validate(cxt, ants, c.constraint());
}


// A conjunction is valid iff both operands are valid.
//
// TODO: Validate the easier branch first. But what is an "easier"
// branch?
Validation
derive(Context& cxt, Prop_list& ants, Conjunction_cons const& c)
{
  Validation v = validate(cxt, ants, c.left());
  if (v == invalid_proof || v == incomplete_proof)
    return v;
  return validate(cxt, ants, c.right());
}


// A conjunction is valid iff either oerand is valid.
//
// TODO: Validate the easier branch first. But what is an "easier"
// branch?
Validation
derive(Context& cxt, Prop_list& ants, Disjunction_cons const& c)
{
  Validation v = validate(cxt, ants, c.left());
  if (v == valid_proof || v == incomplete_proof)
    return v;
  return validate(cxt, ants, c.right());
}


// Try to derive a proof for the sequent of this form:
//
//    A1, A2, ..., An |- C
//
// Note that C does not occur (syntactically) in the list of
// antecedents, so we must decopose C to search for a proof.
Validation
derive(Context& cxt, Prop_list& ants, Cons const& c)
{
  struct fn
  {
    Context&   cxt;
    Prop_list& ants;
    Validation operator()(Cons const& c) const               { return find_support(cxt, ants, c); }
    Validation operator()(Concept_cons const& c) const       { return derive(cxt, ants, c); }
    Validation operator()(Parameterized_cons const& c) const { return derive(cxt, ants, c); }
    Validation operator()(Conjunction_cons const& c) const   { return derive(cxt, ants, c); }
    Validation operator()(Disjunction_cons const& c) const   { return derive(cxt, ants, c); }
  };
  // std::cout << "DERIVE: " << c << '\n';
  return apply(c, fn{cxt, ants});
}


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
validate(Context& cxt, Prop_list& ants, Cons const& c)
{
  // If antecedent set (syntacically) contains C, then the
  // proof is valid.
  if (ants.contains(c))
    return valid_proof;

  // If we had previously memoized the proof, then use that
  // result.
  for (Cons const* a : ants) {
    if (is_memoized(cxt, *a, c))
      return valid_proof;
  }

  // Actually derive a proof of C from AS. If the result
  // is invalid, by the thre are incomplete terms, then
  // the result is incomplete.
  Validation v = derive(cxt, ants, c);
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
validate(Context& cxt, Sequent& s)
{
  Prop_list& as = s.antecedents();
  Prop_list& cs = s.consequents();

  Validation r = invalid_proof;
  for (Cons const* c : cs) {
    Validation v = validate(cxt, as, *c);
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
validate(Proof& p)
{
  Context& cxt = p.cxt;
  Goal_list& goals = p.goals();

  auto iter = goals.begin();
  while (iter != goals.end()) {
    Validation v = validate(cxt, *iter);
    if (v == valid_proof)
      iter = goals.discharge(iter);
    else if (v == invalid_proof || v == incomplete_proof)
      return v;
    ++iter;
  }
  if (goals.empty())
    return valid_proof;
  else
    return incomplete_proof;
}


// -------------------------------------------------------------------------- //
// Flattening
//
// These operations try to move as many propositions as possible into
// the constraint sets on the left and right of a sequent. This will
// never produce sub-goals.

Prop_iter flatten_left(Proof, Cons const&);
Prop_iter flatten_right(Proof, Cons const&);


inline Prop_iter
advance(Prop_list& ps)
{
  return ps.advance();
}


inline Prop_iter
replace(Prop_list& ps, Cons const& c)
{
  auto x = ps.replace(ps.current(), c);
  return x.first;
}


inline Prop_iter
replace(Prop_list& ps, Cons const& c1, Cons const& c2)
{
  auto x = ps.replace(ps.current(), c1, c2);
  return x.first;
}


// Do nothing for atomic constraints.
Prop_iter
flatten_left_atom(Proof p, Cons const& c)
{
  return advance(p.sequent().antecedents());
}


// Replace the current consequent with its operand (maybe).
// Parameterized constraints are essentially transparent, so
// they can be reduced immediately.
Prop_iter
flatten_left_lambda(Proof p, Parameterized_cons const& c)
{
  Cons const& c1 = c.constraint();
  return replace(p.sequent().antecedents(), c1);
}


// Replace the current antecedent with its operands (maybe).
Prop_iter
flatten_left_conjunction(Proof p, Conjunction_cons const& c)
{
  Cons const& c1 = c.left();
  Cons const& c2 = c.right();
  return replace(p.sequent().antecedents(), c1, c2);
}


// Advance to the next term so we don't produce subgoals.
Prop_iter
flatten_left_disjunction(Proof p, Disjunction_cons const& c)
{
  return advance(p.sequent().antecedents());
}


// Select an appropriate action for the current proposition.
Prop_iter
flatten_left(Proof p, Cons const& c)
{
  struct fn
  {
    Proof p;
    Prop_iter operator()(Cons const& c)               { return flatten_left_atom(p, c); }
    Prop_iter operator()(Parameterized_cons const& c) { return flatten_left_lambda(p, c); }
    Prop_iter operator()(Conjunction_cons const& c)   { return flatten_left_conjunction(p, c); }
    Prop_iter operator()(Disjunction_cons const& c)   { return flatten_left_disjunction(p, c); }
  };
  return apply(c, fn{p});
}


// Flatten all propositions in the antecedents.
void
flatten_left(Proof p, Sequent& s)
{
  Prop_list& as = s.antecedents();
  auto ai = as.start();
  while (ai != as.end())
    ai = flatten_left(p, **ai);
}


// Advance to the next goal.
Prop_iter
flatten_right_atom(Proof p, Cons const& c)
{
  return advance(p.sequent().consequents());
}


// Replace the current consequent with its operand (maybe).
// Parameterized constraints are essentially transparent, so
// they can be reduced immediately.
Prop_iter
flatten_right_lambda(Proof p, Parameterized_cons const& c)
{
  Cons const& c1 = c.constraint();
  return replace(p.sequent().consequents(), c1);
}


// Advance to the next term so we don't produce subgoals.
Prop_iter
flatten_right_conjunction(Proof p, Conjunction_cons const& c)
{
  return advance(p.sequent().consequents());
}


// Replace the current antecedent with its operands (maybe).
Prop_iter
flatten_right_disjunction(Proof p, Disjunction_cons const& c)
{
  Cons const& c1 = c.left();
  Cons const& c2 = c.right();
  return replace(p.sequent().consequents(), c1, c2);
}


// Select an appropriate action for the current proposition.
Prop_iter
flatten_right(Proof p, Cons const& c)
{
  struct fn
  {
    Proof p;
    Prop_iter operator()(Cons const& c)               { return flatten_right_atom(p, c); }
    Prop_iter operator()(Parameterized_cons const& c) { return flatten_right_lambda(p, c); }
    Prop_iter operator()(Conjunction_cons const& c)   { return flatten_right_conjunction(p, c); }
    Prop_iter operator()(Disjunction_cons const& c)   { return flatten_right_disjunction(p, c); }
  };
  return apply(c, fn{p});
}


// Flatten all terms in the consequents.
void
flatten_right(Proof p, Sequent& s)
{
  Prop_list& cs = s.consequents();
  auto ci = cs.start();
  while (ci != cs.end())
    ci = flatten_right(p, **ci);
}


// Flatten each sequent in the proof.
//
// FIXME: Cache the "flatness" of each constraint set so that we
// can avoid redundant computations.
//
// Note that we never expand terms on the right hand side after the
// first pass. This is because we expect that most concept are defined
// using conjunctions, and expanding on the right could consume a
// lot of memory (instead of just time).
void
flatten(Proof p)
{
  for (Sequent& s : p.goals()) {
    flatten_left(p, s);
    flatten_right(p, s);
  }
}

// -------------------------------------------------------------------------- //
// Expansion
//
// This tries to select an antecedent to expand. In general, we prefer
// to expand concepts before disjunctions unless the concept containts
// disjunctions.
//
// TODO: We need to cache properties of concepts so that we can quickly
// determine the relative cost of expanding those terms.


// Returns true if a is a better choice for expansion than b. In general,
// we always prefer to expand a concept before a disjunction UNLESS
//
// - the concept contains nested disjunction
// - the disjunction is relatively shallow
//
// Note that a and b can be atomic. We never prefer to constrain
// an atomic expression. In other words, a concept is better
// than anything except a concept. A disjunction is only better
// than atomic constraint.
//
// TODO: Implement ordering heuristics.
inline bool
is_better_expansion(Cons const* a, Cons const* b)
{
  // A concept is better than anything other than another concept.
  if (is<Concept_cons>(a)) {
    if (is<Concept_cons>(b))
      return false;
    return true;
  }

  // A disjunction is better than atomic constraints.
  if (is<Disjunction_cons>(a))
    return is_atomic(*b);

  return false;
}


// Select a term in the sequent to expand. Collect all non-atomic
// terms and select the best term to expand.
//
// NOTE: We should never have conjunctions or parameterized constraints
// as non-atomic propositions in the list of antecedents. Those must
// have been flattened during the previous pass on the proof state.
//
// TODO: If we cache the "flattness" state, then we could skip the
// ordering step since no non-atomic constraint would be expanded.
void
expand_left(Proof& p, Sequent& s)
{
  Prop_list& ps = s.antecedents();

  // Select the best candidate to expand. Only expand if
  // the selected element is non-atomic.
  auto best = std::min_element(ps.begin(), ps.end(), is_better_expansion);
  if (Concept_cons const* c = as<Concept_cons>(*best))
    ps.replace(best, expand(p.context(), *c));
  else if (Disjunction_cons const* d = as<Disjunction_cons>(*best))
    ps.replace(best, d->left(), d->right());
}


// Find a concept on the right hand side, and expand it.
//
// FIXME: This only really works if a concept appears in the
// consequents of the goal. Any degree of conunjunctive nesting
// will ensure that this does not happen.
void
expand_right(Proof& p, Sequent& s)
{
  Prop_list& ps = s.consequents();

  // Replace the first concept.
  auto cmp = [](Cons const* c) { return is<Concept_cons>(c); };
  auto iter = std::find_if(ps.begin(), ps.end(), cmp);
  if (iter != ps.end()) {
    // std::cout << "RIGHT: " << **iter << '\n';
    Concept_cons const& c = cast<Concept_cons>(**iter);
    ps.replace(iter, expand(p.context(), c));
  }
}


// Select, in each goal, a term to expand (and expand it).
//
// TODO: There are other interesting strategies. For example,
// we might choose to expand all concepts first.
void
expand(Proof p)
{
  for (Sequent& s : p.goals()) {
    expand_left(p, s);
    // expand_right(p, s);
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
  Goal_list goals(Sequent(a, c));
  Proof p(cxt, goals);
  // std::cout << "INIT: " << p.sequent() << '\n';

  // Continue manipulating the proof state until we know that
  // the implication is valid or not.
  int n = 1;
  Validation v;
  do {
    // Opportunistically flatten sequents in each goal.
    flatten(p);
    // std::cout << "------------\n";
    // std::cout << "STEP " << n << ": " << p.sequent() << '\n';

    // Having done that, determine if the proof is valid (or not).
    // In either case, we can stop.
    v = validate(p);
    // std::cout << "VALID? " << v << '\n';
    if (v == valid_proof)
      return true;
    if (v == invalid_proof)
      return false;

    // Otherwise, select a term in each goal to expand.
    expand(p);
    ++n;

    // TODO: Actually diagnose implementation limits. Note that the
    // real limiting factor is going to be the goal size, not
    // the step count.
    if (goals.size() > 32)
      throw Limitation_error("exceeded proof subgoal limit");
    if (n > 1024)
      throw Limitation_error("exceeded proof step limit");
  } while (v == incomplete_proof);

  return false;
}


} // namespace banjo
