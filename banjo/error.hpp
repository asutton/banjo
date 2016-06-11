// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_ERROR_HPP
#define BANJO_ERROR_HPP

#include "prelude.hpp"
#include "language.hpp"

#include <cstdlib>
#include <cstring>


namespace banjo
{

struct Context;


namespace detail
{

// -------------------------------------------------------------------------- //
// Format-able messages

// A message argument stores the (mostly) concrete information that
// will be rendered into a final diagnostic. This is a discriminated
// union containing the kinds of arguments that can be rendered into
// strings.
//
// TODO: This facility should most definitely move into lingo.
//
// TODO: Build a visitor for this.
//
// TODO: Support formatting flags, allowing formatting to be communicated
// through to a rendering device.
struct Message_arg
{
  enum Kind
  {
    token_arg,
    term_arg,
    cstr_arg,
    int_arg,
    float_arg
  } k;

  union Rep 
  {
    Rep() { }
    ~Rep() { }
    
    Token       k;
    Term const* t;
    char const* s;
    long        z;
    double      f;
  } u;

  Message_arg(Message_arg const&);
  Message_arg& operator=(Message_arg const&);

  Message_arg(Message_arg&&);
  Message_arg& operator=(Message_arg&&);

  Message_arg(Token const& k) : k(token_arg) { u.k = k; }
  Message_arg(Term const& t) : k(term_arg) { u.t = &t; }
  Message_arg(char const* s) : k(cstr_arg) { u.s = strdup(s); }
  Message_arg(int n) : k(int_arg) { u.z = n; }
  Message_arg(std::size_t n) : k(int_arg) { u.z = n; }
  Message_arg(Integer const& n) : k(int_arg) { u.z = n.gets(); }
  Message_arg(double d) : k(float_arg) { u.f = d; }

  ~Message_arg();

  Kind kind() const { return k; }

  Token const& token() const { return u.k; }
  Term const& term() const   { return *u.t; }
  char const* str() const    { return u.s; }
  long integer() const       { return u.z; }
  double real() const        { return u.f; }

  void partial_copy(Rep const& r);
  void partial_move(Rep&& r);
  void partial_destroy(Rep& r);
};


inline void
Message_arg::partial_copy(Rep const& r)
{
  switch (k) {
    case token_arg:
      new (&u.k) Token(r.k); 
      break;
    case term_arg:
      u.t = r.t; 
      break;
    case cstr_arg:
      u.s = strdup(r.s);
      break;
    case int_arg:
      u.z = r.z;
      break;
    case float_arg:
      u.f = r.f;
  }
}


// This is the same as partial copy, except for the handling of the
// string representation.
inline void
Message_arg::partial_move(Rep&& r)
{
  switch (k) {
    case token_arg:
      new (&u.k) Token(std::move(r.k));
      break;
    case term_arg:
      u.t = r.t; 
      break;
    case cstr_arg:
      u.s = r.s;
      r.s = nullptr;
      break;
    case int_arg:
      u.z = r.z;
      break;
    case float_arg:
      u.f = r.f;
  }
}


inline void
Message_arg::partial_destroy(Rep& r)
{
  switch (k) {
    case token_arg:
      u.k.~Token();
      break;
    case cstr_arg:
      free(const_cast<char*>(u.s));
      break;
    default:
      // All others are trivially destructible.
      break;
  }
}


inline
Message_arg::Message_arg(Message_arg const& x)
  : k(x.k)
{
  partial_copy(x.u);
}


inline Message_arg& 
Message_arg::operator=(Message_arg const& x)
{
  partial_destroy(u);
  k = x.k; 
  partial_copy(x.u);
  return *this;
}


inline
Message_arg::Message_arg(Message_arg&& x)
  : k(x.k)
{
  partial_move(std::move(x.u));
}


inline Message_arg& 
Message_arg::operator=(Message_arg&& x)
{
  partial_destroy(u);
  k = x.k; 
  partial_move(std::move(x.u));
  return *this;
}


inline
Message_arg::~Message_arg()
{
  partial_destroy(u);
}


using Arg_vec = std::vector<Message_arg>;


// Construct a vector of message arguments from a pack of function
// arguments.
//
// TODO: Support a range of wrappers on function arguments in order to
// push formatting information into the render. Examples might be bold,
// highlight, color, selecting debug output vs. pretty printing, etc.

inline void
record_args(Arg_vec& vec)
{ }


template<typename T, typename... Args>
inline void
record_args(Arg_vec& vec, T const& t, Args const&... args)
{ 
  vec.emplace_back(t);
  record_args(vec, args...);
}


} // namespace detail


// A message contains the structure of a string that will be rendered into
// a diagnostic.
//
// TODO: Add methods to make this more consumable by users.
//
// TODO: This render function current dumps messages to an ostream. This
// isn't ideal; we really want to dump to a render device, which could
// very well be an ostream.
//
// TODO: Allow messages to be nested. There are probably a couple different
// forms of nesting: notes, explanations, and causes are probably good
// examples. A note provides additional information, an explanation provides
// context, and a cause describes the source of the error.
//
// TODO: Allow messages to be loaded from files? This would allow/make it 
// easy for compilers to provide a set of templates that explain errors, and 
// could be parameterized by types in the user's program.
struct Message
{
  using Arg = detail::Message_arg;
  using Arg_list = detail::Arg_vec;
  using Msg_list = std::vector<Message>;

  using Render_fn = void (*)(std::ostream&, Arg const&);

  template<typename... Args>
  Message(Location loc, char const* fmt, Args const&... args)
    : loc_(loc), fmt_(fmt), args_()
  {
    detail::record_args(args_, args...);
  }

  template<typename... Args>
  Message(char const* fmt, Args const&... args)
    : loc_(), fmt_(fmt), args_()
  {
    detail::record_args(args_, args...);
  }

  void render(std::ostream&, Render_fn) const;

  void add_note(Message const& m) { notes_.push_back(m); }
  void add_note(Message&& m) { notes_.push_back(std::move(m)); }

  Location loc_;   // The location of the error.
  String   fmt_;   // The format string
  Arg_list args_;  // The argument list
  Msg_list notes_; // Additional information for the message
};


// Used to indicate formatting failures. These are hard errors.
struct Formatting_error : std::runtime_error
{
  Formatting_error(char const* s)
    : std::runtime_error(s)
  { }
};


void dump(Message const&);
void dump(std::ostream&, Message const&);


// -------------------------------------------------------------------------- //
// Exceptions

// The compiler-error class represents a runtime error that contains
// a compiler diagnostic. This overrides the what() function to provide
// a textual representation of that diagnostic.
//
// This class is designed to operate as a fall-back for cases where a
// compiler does not handle certain errors at recovery/progress points.
//
// Note that compiler errors must never escape main(). 
struct Compiler_error : std::runtime_error
{
  Compiler_error()
    : std::runtime_error(""), msg("compiler error")
  { }

  Compiler_error(Message const& m)
    : std::runtime_error(""), msg(m)
  { }

  Compiler_error(Message&& m)
    : std::runtime_error(""), msg(std::move(m))
  { }

  virtual const char* what() const noexcept;

  Context* cxt;       // A pointer to a context for rendering.
  Message  msg;       // The error message
  mutable String buf; // Contains the rendered 'what' text
};


// Represents a translation failure resulting from an internal
// logic error such as a failed precondition or unhandled case.
// See the macros below for simplied usage.
struct Internal_error : Compiler_error
{
  using Compiler_error::Compiler_error;
};


// Throws an internal error to indicate an unhandled case for
// the dynamic type of x.
#define banjo_unhandled_case(x) \
  throw banjo::Internal_error("{}:{}: unhandled case '{}'", __FILE__, __LINE__, lingo::type_str(x))


// Throws an internal error to indicate an unhandled case for
// the dynamic type of x.
#define banjo_unimplemented(s) \
  throw banjo::Internal_error("{}:{}: unimplemented: '{}'", __FILE__, __LINE__, s)

# // Fixes syntax highlighting in Atom

// Represents an error that occurs during translation. Translation
// errors occurring in certain contexts are recoverable.
struct Translation_error : Compiler_error
{
  using Compiler_error::Compiler_error;
};


// Represents a syntactic error.
struct Syntax_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents a lookup error. Lookup errors occur when lookup
// fails to find a declaration or fails to find a declaration
// of the right kind.
struct Lookup_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represets a type error. Type errors occur when one type (or kind
// of type) is expected, but another type is given.
struct Type_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represets an error resulting from an invalid declaration of a name.
struct Declaration_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents an template argument deduction error.
struct Deduction_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents an failure to unify multiple deductions of a template
// parameter. This is a distinct kind of deduction failure.
struct Unification_error : Deduction_error
{
  using Deduction_error::Deduction_error;
};


// Represents an error occcurring in the substitution of template
// arguments for parameters.
struct Substitution_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents an error resulting from the evaluation of a constant
// expression.
struct Evaluation_error : Translation_error
{
  using Translation_error::Translation_error;
};


// Represents an error caused by exceeding an implementation limit.
struct Limitation_error : Translation_error
{
  using Translation_error::Translation_error;
};


} // namespace banjo


#endif
