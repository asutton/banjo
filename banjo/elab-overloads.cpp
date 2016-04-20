// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "parser.hpp"
#include "printer.hpp"
#include "ast.hpp"

#include <iostream>


namespace banjo
{

void check_declarations(Context& cxt, Decl const&, Decl const&);
void check_declarations(Context& cxt, Object_decl const&, Object_decl const&);
void check_declarations(Context& cxt, Function_decl const&, Function_decl const&);
void check_declarations(Context& cxt, Type_decl const&, Type_decl const&);


void
Parser::elaborate_overloads(Stmt_list& ss)
{
  for (Stmt& s : ss)
    elaborate_overloads(s);
}


void
Parser::elaborate_overloads(Stmt& s)
{
  if (Declaration_stmt* s1 = as<Declaration_stmt>(&s))
    elaborate_overloads(s1->declaration());
}


void
Parser::elaborate_overloads(Decl& decl)
{
  Name& name = decl.name();
  Overload_set& ovl = *current_scope().lookup(name);

  // Find the position of the declation within the overload
  // set. We only need to compare it with declarations "down stream"
  // since we will have validated all preceeding declarations.
  auto iter = std::find_if(ovl.begin(), ovl.end(), [&decl](Decl& d) {
    return &decl == &d;
  });

  // Check each downstream declaration in turn, trapping declaration
  // errors so we can diagnose as many as possible.
  bool ok = true;
  for (++iter ; iter != ovl.end(); ++iter) {
    try {
      check_declarations(cxt, decl, *iter);
    } catch (...) {
      ok = false;
    }
  }

  // If we got an error, rethrow it.
  if (!ok)
    throw Declaration_error();
}


// Given declarations d1 and d2, a declaration error occurs when:
//
//    - d2 changes the meaning of the name declared by d1, or if not that, then
//      - d1 and d2 are both objects, or
//      - d1 and d2 are functions that cannot be overloaded, or
//      - d1 and d2 are types having different kinds.
//
// Note that d1 preceeds d2 in lexical order.
//
// TODO: Would it make sense to poison the declaration so that it's not
// analyzed in subsequent passes? We could essentially replace the existing
// overload set with one containing a poisoned declaration. Any expression
// that uses that name would have an invalid type. We could then use this
// to list the places where the error affects use.
void
check_declarations(Context& cxt, Decl const& d1, Decl const& d2)
{
  struct fn
  {
    Context& cxt;
    Decl const& d2;
    void operator()(Decl const& d)           { lingo_unhandled(d); }
    void operator()(Object_decl const& d1)   { return check_declarations(cxt, d1, cast_as(d1, d2)); }
    void operator()(Function_decl const& d1) { return check_declarations(cxt, d1, cast_as(d1, d2)); }
    void operator()(Type_decl const& d1)     { return check_declarations(cxt, d1, cast_as(d1, d2)); }
  };

  if (typeid(d1) != typeid(d2)) {
    // TODO: Get the source location right.
    error(cxt, "declaration changes the meaning of '{}'", d1.name());
    note("'{}' previously declared as:", d1.name());
    
    // TODO: Don't print the definition. It's not germaine to
    // the error. If we have source locations, I wonder if we
    // can just point at the line.
    note("{}", d1);
    throw Declaration_error();
  }
  apply(d1, fn{cxt, d2});
}


void
check_declarations(Context& cxt, Object_decl const& d1, Object_decl const& d2)
{
  struct fn
  {
    char const* operator()(Decl const& d)          { lingo_unhandled(d); }
    char const* operator()(Variable_decl const& d) { return "variable"; }
    char const* operator()(Field_decl const& d)    { return "member variable"; }
    char const* operator()(Object_parm const& d)   { return "parameter"; }
  };
  error(cxt, "redeclaration of {} with the same name", apply(d1, fn{}));
  throw Declaration_error();
}


void
check_declarations(Context& cxt, Function_decl const& d1, Function_decl const& d2)
{
  // FIXME: Actually check overloading rules.
}


void
check_declarations(Context& cxt, Type_decl const& d1, Type_decl const& d2)
{
  Type const& t1 = d1.type();
  Type const& t2 = d2.type();
  if (is_different(t1, t2)) {
    // TODO: Get the source location right.
    error(cxt, "declaration of '{}' as a different kind of type", d1.name());
    note("'{}' previously declared as:", d1.name());
    
    // TODO: Don't print the definition. It's not germaine to
    // the error. If we have source locations, I wonder if we
    // can just point at the line.
    note("{}", d1);
    throw Declaration_error();
  }
}


} // namespace banjo
