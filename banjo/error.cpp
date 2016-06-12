// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "error.hpp"
#include "context.hpp"

#include <lingo/io.hpp>

#include <iostream>
#include <sstream>


namespace banjo
{

// Returns true if we can match '{}' in the character range.
//
// TODO: Modify this to return formatting information contained within
// the braces.
static bool
is_placeholder(char const* f, char const* l)
{
  if (f + 1 == l)
    return false;
  char const* i = f + 1;
  if (*f == '{' && *i == '}')
    return true;
  return false;
}


void
Message::render(std::ostream& os, Render_fn fn) const
{
  auto iter = args_.begin();
  char const* f = fmt_.data();
  char const* l = f + fmt_.size();
  while (f != l) {
    if (is_placeholder(f, l)) {
      if (iter == args_.end())
        throw Formatting_error("too few arguments");
      fn(os, *iter);
      f += 2; // TODO: If we have formatting flags, this will be greater
      ++iter;
    }
    ++f;
  }
  if (iter != args_.end())
    throw Formatting_error("message formatting: too many arguments");
}


// -------------------------------------------------------------------------- //
// Message dump
//
// This is a simplified rendering of the error messages that can be used
// as a basis for more appropriate representations. This simply prints
// to std::error.

static void
dump_arg(std::ostream& os, Message::Arg const& a)
{
  switch (a.kind()) {
    case Message::Arg::token_arg:
      os << a.token();
      break;

    // TODO: Render something meaningful for the term. Perhaps an inline
    // sexpr via the debugging facility.
    case Message::Arg::term_arg:
      os << "<term>";
      break;
    
    case Message::Arg::cstr_arg:
      os << a.str();
      break;
    
    case Message::Arg::int_arg:
      os << a.integer();
      break;
    
    case Message::Arg::float_arg:
      os << a.real();
      break;
  }
}


void
dump(std::ostream& os, Message const& m)
{
  m.render(os, dump_arg);
}


void 
dump(Message const& m)
{
  dump(std::cerr, m);
}


// -------------------------------------------------------------------------- //
// Exception formatting

// FIXME: Any rendering of a diagnostic potentially counts an an error. 
// We need to update the error count so that drivers can exit correctly.
char const*
Compiler_error::what() const noexcept
{
  std::stringstream ss;
  ss.iword(lingo::ios_color_flag) = std::cerr.iword(lingo::ios_color_flag);
  
  // If a context has been provided, render it that way. Otherwise, just
  // take a guess at how it should be formatted.
  //
  // FIXME: If the program is terminating, cxt will not be valid and
  // we can't actually use this. There's 
  if (cxt)
    cxt->emit_error(msg);
  else
    dump(msg);

  buf = ss.str();
  return buf.c_str();
}


} // namespace banjo
