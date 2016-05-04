// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "template.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// Return a reference to the given declaration.
//
// FIXME: Specialize the reference based on whether it's a variable
// or function? Also, handle all of the other things that can be
// referred to (e.g., overload sets, parameters, etc).
Expr&
make_reference(Context& cxt, Decl& d)
{
  // TODO: What other kinds of objects do we have here...
  //
  // TODO: Dispatch.
  if (Variable_decl* v = as<Variable_decl>(&d))
    return cxt.make_reference(*v);
  if (Object_parm* p = as<Object_parm>(&d))
    return cxt.make_reference(*p);
  if (Function_decl* f = as<Function_decl>(&d))
    return cxt.make_reference(*f);
//  if (Coroutine_decl *c = as<Coroutine_decl>(&d))
//    return cxt.make_reference(*c);

  // If it's a template name, then it must almost certainly
  // refer to a function template.
  #if 0
  if (Template_decl* t = as<Template_decl>(&d)) {
    Decl& p = t->parameterized_declaration();
    if (is<Function_decl>(&p))
      return cxt.make_reference(*t);
    throw Type_error("'{}' cannot be used as an expression");
  }
  #endif

  // Here are some things that lookup can find that are not
  // valid expressions.
  //
  // TODO: Diagnose the error and point to the declaration.
  if (Type_decl* t = as<Type_decl>(&d))
    throw Type_error("'{}' is not an object or function", t->name());

  banjo_unhandled_case(d);
}


// Perform unqualified lookup.
Expr&
make_reference(Context& cxt, Simple_id& id)
{
  Decl_list decls = unqualified_lookup(cxt, id);
  if (decls.size() == 1)
    return make_reference(cxt, decls.front());

  // TODO: Return a reference to an overload set.
  banjo_unhandled_case(id);
}


Expr&
make_reference(Context& cxt, Template_id& id)
{
  lingo_unreachable();
  
  // FIXME: Validate that this is actually a referrable entity.
  // Basically, we're going to perform the same analysis as we
  // do above on the resolved declaration (is it a var, fn, etc.?).
  //
  // FIXME: This needs to refer to a *saved* implicit instantiation
  // and not an arbitrarily created declaration. When the arguments
  // are dependent, this could be the same as the primary template
  // declaration -- or it could be something else altogether.

  // Template_decl& tmp = id.declaration();
  // Term_list& args = id.arguments();
  // Decl& d = specialize_template(cxt, tmp, args);
  // return make_reference(cxt, d);
}


Expr&
make_reference(Context& cxt, Concept_id& id)
{
  Builder build(cxt);

  // FIXME: There are a lot of questions to ask here... Presumably,
  // I must ensure that this resoles to a legitimate check, and the
  // arguments should match in kind (and type?). What if they don't.
  //
  // Also, if the arguments are non-dependent, should I fold the
  // constant?
  //
  // If the arguments are dependent, should I enter into a constrained
  // scope?
  //
  // As mentioned... lots of interesting things to do here.
  return build.make_check(id.declaration(), id.arguments());
}


Expr&
make_reference(Context& cxt, Name& n)
{
  struct fn
  {
    Context& cxt;
    Expr& operator()(Name& n)        { banjo_unhandled_case(n); }
    Expr& operator()(Simple_id& n)   { return make_reference(cxt, n); }
    Expr& operator()(Template_id& n) { return make_reference(cxt, n); }
    Expr& operator()(Concept_id& n)  { return make_reference(cxt, n); }
  };
  return apply(n, fn{cxt});
}


// -------------------------------------------------------------------------- //
// Member references


// FIXME: Can we actually have variables in a member? Probably, if it's a
// static member variable. Same goes for member functions.
Expr&
make_member_reference(Context& cxt, Expr& obj, Decl& decl)
{
  // References to non-static members.
  if (Field_decl* v = as<Field_decl>(&decl))
    return cxt.make_member_reference(obj, *v);
  if (Method_decl* f = as<Method_decl>(&decl))
    return cxt.make_member_reference(obj, *f);

  // References to static members.
  if (Variable_decl* v = as<Variable_decl>(&decl))
    return cxt.make_reference(*v);
  if (Function_decl* f = as<Function_decl>(&decl))
    return cxt.make_reference(*f);

  if (is<Type_decl>(&decl)) {
    error(cxt, "'{}' does not name a member variable or member function", decl.name());
    throw Type_error("invalid reference");
  }

  lingo_unhandled(decl);
}



// Perform qualified lookup to resolve the declaration referred to by obj.n.
Expr&
make_member_reference(Context& cxt, Expr& obj, Simple_id& name)
{
  Type& type = obj.type();
  Decl_list decls = qualified_lookup(cxt, type, name);
  if (decls.size() == 1)
    return make_member_reference(cxt, obj, decls.front());
  lingo_unreachable();
}


Expr&
make_member_reference(Context& cxt, Expr& obj, Name& n)
{
  struct fn
  {
    Context& cxt;
    Expr&    obj;
    Expr& operator()(Name& n)      { lingo_unhandled(n); }
    Expr& operator()(Simple_id& n) { return make_member_reference(cxt, obj, n); }
  };
  return apply(n, fn{cxt, obj});
}


} // namespace banjo
