// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_CONVERSION_HPP
#define BANJO_CONVERSION_HPP

#include "prelude.hpp"
#include "ast.hpp"


namespace banjo
{

struct Context;
struct Type;
struct Expr;
struct Conv;


using Qualifier_list = std::vector<int>;


enum Conversion_category
{
  identity_rank,
  value_transformation,
  qualification_adjustment,
  value_promotion,
  value_conversion
};


// The conversion rank.
enum Conversion_rank
{
  exact_rank,
  promotion_rank,
  conversion_rank
};


// A standard conversion sequence is a list of the conversions
// applied to an operand.
struct Standard_conversion_seq
{
  Standard_conversion_seq()
    : xform(nullptr), conv(nullptr), adjust(nullptr)
  { }

  // Returns the conversion that defines a value transformation
  // or nullptr if no such transformation is applied.
  Conv const* transformation() const { return xform; }
  Conv*       transformation()       { return xform; }
  void        transformation(Conv& e);

  // Returns the conversion that defines a value conversion
  // or promotion. Returns null if no such conversion is
  // applied.
  Conv const* conversion() const { return conv; }
  Conv*       conversion()       { return conv; }
  void        conversion(Conv& e);

  // Returns the conversion that applies a qualification adjustment,
  // or nullptr if no such adjustment is applied.
  Conv const* adjustment() const { return adjust; }
  Conv*       adjustment()       { return adjust; }
  void        adjustment(Conv& e);

  Conv* xform;
  Conv* conv;
  Conv* adjust;
};


inline void
Standard_conversion_seq::transformation(Conv& e)
{
  lingo_assert(!xform);
  xform = &e;
}


inline void
Standard_conversion_seq::conversion(Conv& e)
{
  lingo_assert(!conv);
  conv = &e;
}


inline void
Standard_conversion_seq::adjustment(Conv& e)
{
  lingo_assert(!adjust);
  adjust = &e;
}


// A user-defined conversion sequence enumerates the conversion
// needed to convert a user-defined type to a destination
// type. This is a standard conversion sequence followed by
// a user-defined conversion (a conversion-function call or
// constructor invokation), followed by another standard
// conversion sequence.
//
// TODO: Finish implementing me.
struct User_conversion_seq
{
  Standard_conversion_seq before;
  Conv*                   user;
  Standard_conversion_seq after;
};


// An ellipsis conversion sequence represents the conversion
// of an argument into the ellipsis parameter of a variadic
// function.
//
// This is currently empty.
struct Ellipsis_conversion_seq
{
};


// Kinds of conversion sequence
enum Conversion_seq_kind
{
  std_conv_seq,
  user_conv_seq,
  ellipsis_conv_seq
};


// Internal representation of a conversion sequence.
union Conversion_seq_rep
{
  Conversion_seq_rep(Standard_conversion_seq s) : sc(s) { }
  Conversion_seq_rep(User_conversion_seq s) : uc(s) { }
  Conversion_seq_rep(Ellipsis_conversion_seq s) : ec(s) { }

  Standard_conversion_seq sc;
  User_conversion_seq     uc;
  Ellipsis_conversion_seq ec;
};


// A conversion sequence is one of:
//
//    - a standard conversion sequence
//    - a user-defined conversion sequence
//    - an ellipsis conversion sequence
//
// TODO: What's the interface here?
//
// TODO: Integrate knowledge of reference binding since that affects
// how conversions are ordered. Also support aggregate initialization,
// since those rules might also affect ordering.
struct Conversion_seq
{
  Conversion_seq(Standard_conversion_seq s)
    : k(std_conv_seq), r(s)
  { }

  Conversion_seq(Ellipsis_conversion_seq s)
    : k(ellipsis_conv_seq), r(s)
  { }

  Conversion_seq_kind kind() const { return k; }

  // TODO: Assert the kind.
  Standard_conversion_seq standard_conversions() const { return r.sc; }
  User_conversion_seq     user_conversions() const     { return r.uc; }
  Ellipsis_conversion_seq ellipsis_conversions() const { return r.ec; }

  Conversion_seq_kind k;
  Conversion_seq_rep  r;
};


// The results obtainable by a comparison of conversions and
// conversion sequences.
enum Conversion_comp
{
  worse_conv = -1,
  indistinct_conv = 0,
  better_conv = 1
};


// FIXME: All of these should take a context.

Expr&     standard_conversion(Expr const&, Type const&);
Expr_pair arithmetic_conversion(Expr const&, Expr const&);
Expr&     contextual_conversion_to_bool(Context& cxt, Expr&);
Expr&     dependent_conversion(Context& cxt, Expr&, Type&);

Conversion_seq get_conversion_sequence(Expr const&);

Conversion_comp compare(Conversion_seq const&, Conversion_seq const&);
Conversion_comp compare(Standard_conversion_seq const&, Standard_conversion_seq const&);

bool is_similar(Type const&, Type const&);
Qualifier_list get_qualification_signature(Type const&);


bool is_tuple_equiv_to_array(Tuple_type& t1, Array_type& t2);



} // namespace banjo


#endif
