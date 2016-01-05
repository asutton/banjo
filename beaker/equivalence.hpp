// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BEAKER_EQUIVALENCE_HPP
#define BEAKER_EQUIVALENCE_HPP

#include "prelude.hpp"

#include <vector>
#include <unordered_map>


namespace beaker
{

struct Type;


bool is_equivalent(Type const*, Type const*);


template<typename T>
struct Term_eq
{
  bool operator()(T const* a, T const* b) const
  {
    return is_equivalent(a, b);
  }
};


using Type_eq = Term_eq<Type>;


// Union-find data structure.

// A node in a union-find forest.
template<typename T>
struct Disjoint_node
{
  Disjoint_node(T const& x)
    : elem(x), parent(nullptr)
  { }

  T              elem;
  Disjoint_node* parent;
};


// A union forest is a sequnence of disjoint nodes, used
// to implement a union-find data structure.
template<typename T, typename Hash = std::hash<T>, typename Eq = std::equal_to<T>>
struct Union_forest
{
  using Node  = Disjoint_node<T>;
  using Store = std::vector<Node*>;
  using Map   = std::unordered_map<T, std::size_t, Hash, Eq>;

  ~Union_forest();

  Store nodes;
  Map   map;
};


template<typename T, typename H, typename E>
Union_forest<T, H, E>::~Union_forest()
{
  for (Node* n : nodes)
    delete n;
}


} // namespace beaker

#endif
