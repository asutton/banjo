// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_ERROR_HPP
#define BANJO_ERROR_HPP

#include "prelude.hpp"

namespace banjo
{

// The compiler-error class represents a runtime error that contains
// a compiler diagnostic. This overrides the what() function to provide
// a textual represntation of that diagnostic.
//
// NOTE: Do not let compiler errors escape main(). The rendering of
// of a diagnostic message requires that input buffers be in scope,
// which may not be guaranteed at the point of termination.
//
// FIXME: For constructors taking strings, do I need (or want) to
// give some more specific kind of error (like fatal?).
struct Compiler_error : std::runtime_error
{
  Compiler_error()
    : Compiler_error(error_diag, String("compiler error"))
  { }

  Compiler_error(Diagnostic d)
    : std::runtime_error(""), diag(d)
  { }

  Compiler_error(Diagnostic_kind k, String const& s)
    : Compiler_error(Diagnostic(k, Location(), s))
  { }

  Compiler_error(Diagnostic_kind k, Location loc, String const& s)
    : Compiler_error(Diagnostic(k, loc, s))
  { }

  Compiler_error(String const& s)
    : Compiler_error(Diagnostic(error_diag, Location(), s))
  { }

  Compiler_error(char const* s)
    : Compiler_error(Diagnostic(error_diag, Location(), s))
  { }

  template<typename... Args>
  Compiler_error(char const* s, Args const&... args)
    : Compiler_error(error_diag, format(s, args...))
  { }

  template<typename... Args>
  Compiler_error(Location loc, char const* s, Args const&... args)
    : Compiler_error(error_diag, loc, format(s, args...))
  { }

  template<typename... Args>
  Compiler_error(Context& cxt, String const& s)
    : Compiler_error(error_diag, location(cxt), s)
  { }

  template<typename... Args>
  Compiler_error(Context& cxt, char const* s, Args const&... args)
    : Compiler_error(error_diag, location(cxt), format(s, args...))
  { }

  virtual const char* what() const noexcept;

  // Helper functions.
  Location location(Context const&);


  Diagnostic diag;    // The diagnostic
  mutable String buf; // Guarantees ownership of 'what' text
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
