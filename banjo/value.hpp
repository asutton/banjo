// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_VALUE_HPP
#define BANJO_VALUE_HPP

// This module defines the compile-time representation of values within 
// the abstract machine.

#include "language.hpp"

#include <cstring>


namespace banjo
{

struct Value;


// An enumeration of value kinds. Each value kind determines the way
// in which the vale is represented in memory.
enum Value_kind
{
  error_value,     // An object containing an error.
  void_value,      // An object representing a void expression
  integer_value,   // An object containing an integer value
  float_value,     // An object containing floating point value
  aggregate_value, // An object containing other objects
  reference_value, // A reference to an object
};


// An empty representation of an error state.
struct Error_value { };


// Represents values of the expression void().
struct Void_value { };


// Representation of fundamental value categories.
using Integer_value = int64_t;


// Represents a floating point object.
using Float_value = double;


// An aggregation of other values.
//
// TODO: Represent compile-time string literal differently?
//
// TODO: We need to be careful to not let reference values refer
// to objects that are no longer live. In other words, we have to
// avoid dangling references at all costs within the evaluator.
struct Aggregate_value : std::vector<Value>
{
  Aggregate_value(std::size_t n);
  Aggregate_value(char const*);
  Aggregate_value(char const*, std::size_t n);

  String get_string() const;
};


// Represents a handle to another value as a pointer to that value.
//
// TODO: See comments on aggregate values. We may need a more complex
// reference representation in order to avoid dangling references.
using Reference_value = Value*;


// The underlying representation of the value variant.
union Value_rep
{
  Value_rep() { }
  Value_rep(Error_value x) : err_(x) { }
  Value_rep(Void_value x) : void_(x) { }
  Value_rep(Integer_value z) : int_(z) { }
  Value_rep(Float_value fp) : float_(fp) { }
  Value_rep(Aggregate_value const& a) : agg_(a) { }
  Value_rep(Aggregate_value&& a) : agg_(std::move(a)) { }
  Value_rep(Reference_value r) : ref_(r) { }
  ~Value_rep() { }

  void initialize(Value_rep const&, Value_kind);
  void initialize(Value_rep&&, Value_kind);
  void destroy(Value_kind);

  Error_value     err_;
  Void_value      void_;
  Integer_value   int_;
  Float_value     float_;
  Aggregate_value agg_;
  Reference_value ref_;
};


// Represents a compile time value as a variant of objects of varying shape.
struct Value
{
  struct Visitor;
  struct Mutator;

  Value()
    : k(error_value), r(Error_value{})
  { }

  // Copy semantics
  Value(Value const&);
  Value& operator=(Value const&);

  // Move semantics
  Value(Value&&);
  Value& operator=(Value&&);

  Value(Void_value v)
    : k(void_value), r(v)
  { }

  // Need this constructor because the conversion from int to int64_t or 
  // double is ambiguous.
  Value(int n)
    : k(integer_value), r((int64_t)n)
  { }

  // Need this constructor because the conversion from unsigned long to 
  // int64_t or double is ambiguous.
  Value(unsigned long n)
    : k(integer_value), r((int64_t)n)
  { }

  Value(Integer_value n)
    : k(integer_value), r(n)
  { }

  Value(Float_value fp)
    : k(float_value), r(fp)
  { }

  Value(Aggregate_value const& a)
    : k(aggregate_value), r(a)
  { }

  Value(Aggregate_value&& a)
    : k(aggregate_value), r(std::move(a))
  { }

  Value(Reference_value ref)
    : k(reference_value), r(ref)
  { }

  ~Value();

  void accept(Visitor&) const;
  void accept(Mutator&);

  Value_kind kind() const { return k; }
  bool is_error() const;
  bool is_void() const;
  bool is_integer() const;
  bool is_float() const;
  bool is_reference() const;
  bool is_aggregate() const;
  bool is_tuple() const;

  Error_value            get_error() const;
  Void_value             get_void() const;
  Integer_value          get_integer() const;
  bool                   get_boolean() const;
  Float_value            get_float() const;
  Aggregate_value const& get_aggregate() const;  
  Reference_value        get_reference() const;

  Value_kind k;
  Value_rep r;
};


// The non-modifying visitor.
struct Value::Visitor
{
  virtual void visit(Error_value const&) = 0;
  virtual void visit(Void_value const&) = 0;
  virtual void visit(Integer_value const&) = 0;
  virtual void visit(Float_value const&) = 0;
  virtual void visit(Reference_value const&) = 0;
  virtual void visit(Aggregate_value const&) = 0;
};


// The modifying visitor
struct Value::Mutator
{
  virtual void visit(Error_value&) = 0;
  virtual void visit(Void_value&) = 0;
  virtual void visit(Integer_value&) = 0;
  virtual void visit(Float_value&) = 0;
  virtual void visit(Reference_value&) = 0;
  virtual void visit(Aggregate_value&) = 0;
};


inline
Value::Value(Value const& x)
  : k(x.k)
{
  r.initialize(x.r, k);
}


inline Value&
Value::operator=(Value const& x)
{
  if (this != &x) {
    r.destroy(k);
    k = x.k;
    r.initialize(x.r, k);
  }
  return *this;
}


inline
Value::Value(Value&& x)
  : k(x.k)
{
  r.initialize(std::move(x.r), k);
}


inline Value&
Value::operator=(Value&& x)
{
  if (this != &x) {
    r.destroy(k);
    k = x.k;
    r.initialize(std::move(x.r), k);
  }
  return *this;
}


inline
Value::~Value()
{
  r.destroy(k);
}


// Returns true if the value is an error.
inline bool
Value::is_error() const
{
  return k == error_value;
}


// Returns true if the value is a void.
inline bool
Value::is_void() const
{
  return k == void_value;
}


// Returns true if the value is an integer.
inline bool
Value::is_integer() const
{
  return k == integer_value;
}


// Returns true if the value is an floating point.
inline bool
Value::is_float() const
{
  return k == float_value;
}


// Returns true if the value is an aggregate.
inline bool
Value::is_aggregate() const
{
  return k == aggregate_value;
}


// Returns true if k is a reference.
inline bool
Value::is_reference() const
{
  return k == reference_value;
}


// Returns the error value.
inline Error_value
Value::get_error() const
{
  assert(is_error());
  return r.err_;
}


// Returns the void value.
inline Void_value
Value::get_void() const
{
  assert(is_void());
  return r.void_;
}


// Returns the integer value.
inline Integer_value
Value::get_integer() const
{
  assert(is_integer());
  return r.int_;
}


// Returns the floating point value.
inline Float_value
Value::get_float() const
{
  assert(is_float());
  return r.float_;
}


// Get a pointer to the referred to value.
inline Reference_value
Value::get_reference() const
{
  assert(is_reference());
  return r.ref_;
}


// Returns the array value.
inline Aggregate_value const&
Value::get_aggregate() const
{
  assert(is_aggregate());
  return r.agg_;
}


// Returns an boolean interpretation of an integer value.
inline bool
Value::get_boolean() const
{
  return (bool)get_integer();
}


inline void
Value::accept(Visitor& v) const
{
  switch (k) {
    case error_value: return v.visit(r.err_);
    case void_value: return v.visit(r.void_);
    case integer_value: return v.visit(r.int_);
    case float_value: return v.visit(r.float_);
    case reference_value: return v.visit(r.ref_);
    case aggregate_value: return v.visit(r.agg_);
    default: lingo_unreachable();
  }
}


inline void
Value::accept(Mutator& v)
{
  switch (k) {
    case error_value: return v.visit(r.err_);
    case void_value: return v.visit(r.void_);
    case integer_value: return v.visit(r.int_);
    case float_value: return v.visit(r.float_);
    case reference_value: return v.visit(r.ref_);
    case aggregate_value: return v.visit(r.agg_);
    default: lingo_unreachable();
  }
}


// -------------------------------------------------------------------------- //
// Value_rep implementation


// Copy initialize this object from another representation with shape k.
inline void
Value_rep::initialize(Value_rep const& x, Value_kind k)
{
  switch (k) {
    case error_value:
      new (&err_) Error_value(x.err_); 
      break;

    case void_value:
      new (&void_) Void_value(x.void_); 
      break;

    case integer_value:
      new (&int_) Integer_value(x.int_); 
      break;

    case float_value:
      new (&float_) Float_value(x.float_); 
      break;

    case aggregate_value:
      new (&agg_) Aggregate_value(x.agg_); 
      break;

    case reference_value:
      new (&ref_) Reference_value(x.ref_); 
      break;

    default: 
      lingo_unreachable();
  }
}


// Move initialize self from another representation with shape k.
inline void
Value_rep::initialize(Value_rep&& x, Value_kind k)
{
  switch (k) {
    case error_value:
      new (&err_) Error_value(std::move(x.err_)); 
      break;

    case void_value:
      new (&void_) Void_value(std::move(x.void_)); 
      break;

    case integer_value:
      new (&int_) Integer_value(std::move(x.int_)); 
      break;

    case float_value:
      new (&float_) Float_value(std::move(x.float_)); 
      break;

    case aggregate_value:
      new (&agg_) Aggregate_value(std::move(x.agg_)); 
      break;

    case reference_value:
      new (&ref_) Reference_value(std::move(x.ref_)); 
      break;

    default: 
      lingo_unreachable();
  }
}


// Destroy this value representation.
inline void
Value_rep::destroy(Value_kind k)
{
  switch (k) {
    case error_value:
      err_.~Error_value();
      break;

    case void_value:
      void_.~Void_value();
      break;

    case integer_value:
      int_.~Integer_value();
      break;

    case float_value:
      float_.~Float_value();
      break;

    case aggregate_value:
      agg_.~Aggregate_value();
      break;

    case reference_value:
      ref_.~Reference_value();
      break;

    default: 
      lingo_unreachable();
  }
}



// -------------------------------------------------------------------------- //
// Aggregate implementation
//
// These must appear after the definition of Value because they require
// it to be a complete type.


// Initialize the aggregate with n error values.
//
// TODO: These should be indeterminate values.
inline
Aggregate_value::Aggregate_value(std::size_t n)
  : std::vector<Value>(n)
{ }


// FIXME
inline
Aggregate_value::Aggregate_value(char const* s)
  : Aggregate_value(std::strlen(s) + 1)
{
  auto iter = copy(s, s + size(), begin());
  *iter = Value(0);
}


inline
Aggregate_value::Aggregate_value(char const* s, std::size_t n)
  : Aggregate_value(n + 1)
{
  auto iter = std::copy(s, s + n, begin());
  *iter = Value(0);
}



// -------------------------------------------------------------------------- //
// Generic visitors

template<typename F, typename T>
struct Generic_value_visitor : Value::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_value_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }

  void visit(Error_value const& v)     { this->invoke(v); };
  void visit(Void_value const& v)      { this->invoke(v); };
  void visit(Integer_value const& v)   { this->invoke(v); };
  void visit(Float_value const& v)     { this->invoke(v); };
  void visit(Aggregate_value const& v) { this->invoke(v); };
  void visit(Reference_value const& v) { this->invoke(v); };
};


template<typename F, typename T>
struct Generic_value_mutator : Value::Mutator, lingo::Generic_mutator<F, T>
{
  Generic_value_mutator(F fn)
    : lingo::Generic_mutator<F, T>(fn)
  { }

  void visit(Error_value& v)     { this->invoke(v); };
  void visit(Void_value& v)      { this->invoke(v); };
  void visit(Integer_value& v)   { this->invoke(v); };
  void visit(Float_value& v)     { this->invoke(v); };
  void visit(Aggregate_value& v) { this->invoke(v); };
  void visit(Reference_value& v) { this->invoke(v); };
};


template<typename F, typename T = typename std::result_of<F(Error_value const&)>::type>
inline decltype(auto)
apply(Value const& v, F fn)
{
  Generic_value_visitor<F, T> vis(fn);
  return accept(v, vis);
}


template<typename F, typename T = typename std::result_of<F(Error_value&)>::type>
inline decltype(auto)
apply(Value& v, F fn)
{
  Generic_value_mutator<F, T> vis(fn);
  return accept(v, vis);
}


// -------------------------------------------------------------------------- //
// Intrinsic behaviors

void zero_initialize(Value&);


// -------------------------------------------------------------------------- //
// Other types and functions


// A sequence of values.
using Value_list = std::vector<Value>;


// -------------------------------------------------------------------------- //
// Printing
//
// TODO: Move this to the print module?

std::ostream& operator<<(std::ostream&, Value const&);


} // namepace banjo

#endif
