// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_VALUE_HPP
#define BANJO_VALUE_HPP

#include "language.hpp"

#include <cstring>


namespace banjo
{

struct Function_decl;


struct Value;


enum Value_kind
{
  error_value,
  integer_value,
  float_value,
  reference_value,
  array_value,
  tuple_value,
};


// An empty representation of an error state.
struct Error_value { };


// Representation of fundamental value categories.
using Integer_value = int64_t;


// Represents a floating point object.
using Float_value = double;


// Represents a memory address.
//
// TODO: This isn't quite an address; it's the location of an object
// in memory. A "pure" address is simply an integer value. However,
// I'm not sure we want the abstract machine to allow loads and stores
// of uninitialized (i.e., non-existent) memory.
using Reference_value = Decl const*;


// The common structure of array and tuple values.
//
// FIXME: Memory allocated to this array is never freed. Stop leaking
// memory. Either make this a regular type with move semantics or use a 
// shared pointer for for the underlying structure.
//
// TODO: Consider making this vector<Value>. Note that vector may not be
// defined for an incomplete type (should be fixed in C++17?).
struct Aggregate_value
{
  Aggregate_value(std::size_t n);
  Aggregate_value(char const*);
  Aggregate_value(char const*, std::size_t n);

  std::size_t size() const { return len; }

  Value const& operator[](std::size_t) const;
  Value&       operator[](std::size_t);

  // Iterators
  Value const* begin() const;
  Value const* end() const;

  Value* begin();
  Value* end();

  std::size_t len;
  Value*      data;
};


// An array value is a sequence of values of the
// same kind.
struct Array_value : Aggregate_value
{
  using Aggregate_value::Aggregate_value;

  std::string get_as_string() const;
};


// A tuple value is a sequence of values of different kind.
struct Tuple_value : Aggregate_value
{
  using Aggregate_value::Aggregate_value;
};


// The underlying representation of the value variant.
union Value_rep
{
  Value_rep() : err_() { }
  Value_rep(Integer_value z) : int_(z) { }
  Value_rep(Float_value fp) : float_(fp) { }
  Value_rep(Reference_value r) : ref_(r) { }
  Value_rep(Array_value a) : arr_(a) { }
  Value_rep(Tuple_value t) : tup_(t) { }
  ~Value_rep() { }

  Error_value     err_;
  Integer_value   int_;
  Float_value     float_;
  Reference_value ref_;
  Array_value     arr_;
  Tuple_value     tup_;
};


// Represents a compile time value.
struct Value
{
  struct Visitor;
  struct Mutator;

  Value()
    : k(error_value), r()
  { }

  // TODO: handle signed and unsigned

  // Need this constructor because the conversion from
  // int to int64_t or double is ambiguous.
  Value(int n)
    : k(integer_value), r((int64_t)n)
  { }

  // Need this constructor because the conversion from
  // unsigned long to int64_t or double is ambiguous.
  Value(unsigned long n)
    : k(integer_value), r((int64_t)n)
  { }

  Value(Integer_value n)
    : k(integer_value), r(n)
  { }

  Value(Float_value fp)
    : k(float_value), r(fp)
  { }

  Value(Reference_value ref)
    : k(reference_value), r(ref)
  { }

  Value(Array_value a)
    : k(array_value), r(a)
  { }

  Value(Tuple_value a)
    : k(tuple_value), r(a)
  { }

  Value(Value* v);

  ~Value() { }

  void accept(Visitor&) const;
  void accept(Mutator&);

  Value_kind kind() const { return k; }
  bool is_error() const;
  bool is_integer() const;
  bool is_float() const;
  bool is_reference() const;
  bool is_array() const;
  bool is_tuple() const;

  Error_value     get_error() const;
  Integer_value   get_integer() const;
  Float_value     get_float() const;
  Reference_value get_reference() const;
  Array_value     get_array() const;
  Tuple_value     get_tuple() const;
  bool            get_boolean() const;

  Value_kind k;
  Value_rep r;
};


// The non-modifying visitor.
struct Value::Visitor
{
  virtual void visit(Error_value const&) = 0;
  virtual void visit(Integer_value const&) = 0;
  virtual void visit(Float_value const&) = 0;
  virtual void visit(Reference_value const&) = 0;
  virtual void visit(Array_value const&) = 0;
  virtual void visit(Tuple_value const&) = 0;
};


// The modifying visitor
struct Value::Mutator
{
  virtual void visit(Error_value&) = 0;
  virtual void visit(Integer_value&) = 0;
  virtual void visit(Float_value&) = 0;
  virtual void visit(Reference_value&) = 0;
  virtual void visit(Array_value&) = 0;
  virtual void visit(Tuple_value&) = 0;
};


// Returns true if the value is an error.
inline bool
Value::is_error() const
{
  return k == error_value;
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


// Returns true if k is a reference.
inline bool
Value::is_reference() const
{
  return k == reference_value;
}


// Returns true if the value is an array.
inline bool
Value::is_array() const
{
  return k == array_value;
}


// Returns true if the value is a tuple.
inline bool
Value::is_tuple() const
{
  return k == tuple_value;
}


// Returns the error value.
inline Error_value
Value::get_error() const
{
  assert(is_error());
  return r.err_;
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
inline Array_value
Value::get_array() const
{
  assert(is_array());
  return r.arr_;
}


// Returns the array value.
inline Tuple_value
Value::get_tuple() const
{
  assert(is_tuple());
  return r.tup_;
}


// Returns an boolean interpretaion of an integer value.
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
    case integer_value: return v.visit(r.int_);
    case float_value: return v.visit(r.float_);
    case reference_value: return v.visit(r.ref_);
    case array_value: return v.visit(r.arr_);
    case tuple_value: return v.visit(r.tup_);
  }
}


inline void
Value::accept(Mutator& v)
{
  switch (k) {
    case error_value: return v.visit(r.err_);
    case integer_value: return v.visit(r.int_);
    case float_value: return v.visit(r.float_);
    case reference_value: return v.visit(r.ref_);
    case array_value: return v.visit(r.arr_);
    case tuple_value: return v.visit(r.tup_);
  }
}


// -------------------------------------------------------------------------- //
// Aggregate implementations
//
// These must appear after the definition of Value because they require
// it to be a complete type.

inline
Aggregate_value::Aggregate_value(std::size_t n)
  : len(n), data(new Value[n])
{ }


inline
Aggregate_value::Aggregate_value(char const* s)
  : Aggregate_value(std::strlen(s))
{ }


inline
Aggregate_value::Aggregate_value(char const* s, std::size_t n)
  : Aggregate_value(n)
{
  std::copy(s, s + n, data);
}


inline Value const&
Aggregate_value::operator[](std::size_t n) const
{
  return data[n];
}


inline Value&
Aggregate_value::operator[](std::size_t n)
{
  return data[n];
}


inline Value const*
Aggregate_value::begin() const
{ 
  return data; 
}


inline Value const*
Aggregate_value::end() const  
{ 
  return data + len; 
}


inline Value*
Aggregate_value::begin()
{ 
  return data; 
}


inline Value*
Aggregate_value::end()  
{ 
  return data + len; 
}


// -------------------------------------------------------------------------- //
// Generic visitors

template<typename F, typename T>
struct Generic_value_visitor : Value::Visitor, lingo::Generic_visitor<F, T>
{
  Generic_value_visitor(F fn)
    : lingo::Generic_visitor<F, T>(fn)
  { }

  void visit(Error_value const& v) { this->invoke(v); };
  void visit(Integer_value const& v) { this->invoke(v); };
  void visit(Float_value const& v) { this->invoke(v); };
  void visit(Reference_value const& v) { this->invoke(v); };
  void visit(Array_value const& v) { this->invoke(v); };
  void visit(Tuple_value const& v) { this->invoke(v); };
};


template<typename F, typename T>
struct Generic_value_mutator : Value::Mutator, lingo::Generic_mutator<F, T>
{
  Generic_value_mutator(F fn)
    : lingo::Generic_mutator<F, T>(fn)
  { }

  void visit(Error_value& v)     { this->invoke(v); };
  void visit(Integer_value& v)   { this->invoke(v); };
  void visit(Float_value& v)     { this->invoke(v); };
  void visit(Reference_value& v) { this->invoke(v); };
  void visit(Array_value& v)     { this->invoke(v); };
  void visit(Tuple_value& v)     { this->invoke(v); };
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
