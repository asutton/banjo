// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "type.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Helper functions
//
// The following functions determine the type and value category of the
// id-expressions, depending on the kind of declaration named.


// The identifier is not an object or function.
static Expr&
make_error_ref(Context& cxt, Decl& d)
{
  // Here are some things that lookup can find that are not
  // valid expressions.
  //
  // TODO: Diagnose the error and point to the declaration.
  if (Type_decl* t = as<Type_decl>(&d)) {
    error(cxt, "'{}' is not an object, reference, or function", t->name());
    throw Type_error();
  }

  lingo_unhandled(d);
}


// Returns a reference to the declared variable. In the general, the
// type will be a reference to the declared variable. However, if the
// declaration has meta-type, the result is an object.
//
// TODO: Handle meta variables.
static Expr&
make_variable_ref(Context& cxt, Variable_decl& d)
{
  Type& t = cxt.get_reference_type(d.type());
  return cxt.make_reference(t, d);
}


// Returns a reference to a declared function. A name denoting a function
// declaration is a reference value whose type is the type of the function.
//
// TODO: Can I declare a consumed function? 
//
//    def f(n : int) -> int;
//    consume f := fn; ???
//
// No... that wouldn't work. But this would:
//
//    consume f := consume fn;
//
// And it would have approximately the same meaning as this:
//
//    var x : int;
//    consume x := consume x;
static Expr&
make_function_ref(Context& cxt, Function_decl& d)
{
  Type& t = cxt.get_reference_type(d.type());
  return cxt.make_reference(t, d);
}


// Return a reference to the given declaration based on the kind
// of the resolved declaration.
static Expr&
make_resolved_ref(Context& cxt, Decl& decl)
{
  struct fn
  {
    Context& cxt;
    Expr& operator()(Decl& d)          { lingo_unhandled(d); }
    Expr& operator()(Variable_decl& d) { return make_variable_ref(cxt, d); }
    Expr& operator()(Function_decl& d) { return make_function_ref(cxt, d); }
  };
  return apply(decl, fn{cxt});
}


// Return a reference to an overload set.
static Expr&
make_overload_ref(Context& cxt, Name& n, Decl_list&& ds)
{
  return cxt.make_reference(n, std::move(ds));
}


// Returns an expression referring to a class field. A field expression
// is a reference value expression whose type is that of the declared 
// field. If the field is declared with the consume specifier, then the 
// expression computes a consumable value.
static Expr&
make_mem_variable_ref(Context& cxt, Expr& e, Mem_variable_decl& d)
{
  lingo_unreachable();

  // Type& t = cxt.get_reference_type(d.type());
  // return cxt.make_reference(t, d);
}


// Returns an expression referring to a class method. A method expression
// is a reference value expression whose type is that of the function.
static Expr&
make_mem_function_ref(Context& cxt, Expr& e, Mem_function_decl& d)
{
  lingo_unreachable();

  // return cxt.make_reference(nref_expr, d.type(), e, d);
}


// Note that member references to static data members are not
// actually member expressions; we can discard the computed object
// and simply refer to the core value.
static Expr&
make_resolved_mem_ref(Context& cxt, Expr& obj, Decl& decl)
{
  struct fn
  {
    Context& cxt;
    Expr& obj;

    Expr& operator()(Decl& d) { return make_error_ref(cxt, d); }

    // Non-members.
    Expr& operator()(Variable_decl& d) { return make_variable_ref(cxt, d); }
    Expr& operator()(Function_decl& d) { return make_function_ref(cxt, d); }
    
    // Members.
    Expr& operator()(Mem_variable_decl& d) { return make_mem_variable_ref(cxt, obj, d); }
    Expr& operator()(Mem_function_decl& d) { return make_mem_function_ref(cxt, obj, d); }
  };
  return apply(decl, fn{cxt, obj});
}


// Construct a reference to an unresolved (overloaded) member.
static Expr&
make_mem_overload_ref(Context& cxt, Expr& obj, Name& name, Decl_list&& ds)
{
  lingo_unreachable();

  // return cxt.make_reference(obj, name, std::move(ds));
}


// -------------------------------------------------------------------------- //
// Non-member references

// Perform unqualified lookup and return a kind of id-expression referring
// to the found declaration(s).
//
// TODO: Allow an lookup to fail, indicating that we could not find
// a name. In function calls, this could be used to perform class
// lookup for member functions.
Expr&
make_reference(Context& cxt, Simple_id& id)
{
  Decl_list decls = unqualified_lookup(cxt, id);
  if (decls.size() == 1)
    return make_resolved_ref(cxt, decls.front());
  else
    return make_overload_ref(cxt, id, std::move(decls));
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
  lingo_unreachable();

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
}


// Returns a reference to the given name.
//
// TODO: Support qualified names.
Expr&
make_reference(Context& cxt, Name& n)
{
  struct fn
  {
    Context& cxt;
    Expr& operator()(Name& n)        { lingo_unhandled(n); }
    Expr& operator()(Simple_id& n)   { return make_reference(cxt, n); }
    Expr& operator()(Template_id& n) { return make_reference(cxt, n); }
    Expr& operator()(Concept_id& n)  { return make_reference(cxt, n); }
  };
  return apply(n, fn{cxt});
}


// -------------------------------------------------------------------------- //
// Member references


// Perform qualified lookup to resolve the declaration referred to by obj.n.
//
// FIXME: This needs to perform class lookup, not qualified lookup.
static Expr&
make_member_ref(Context& cxt, Expr& obj, Simple_id& name)
{
  Type& type = obj.type();
  Decl_list decls = qualified_lookup(cxt, type, name);
  if (decls.empty()) {
    error(cxt, "no matching declaration for '{}'", name);
    throw Lookup_error();
  }
  if (decls.size() == 1)
    return make_resolved_mem_ref(cxt, obj, decls.front());
  else
    return make_mem_overload_ref(cxt, obj, name, std::move(decls));
}


// Return an expression that refers to a name scoped to object.
Expr&
make_member_reference(Context& cxt, Expr& obj, Name& n)
{
  struct fn
  {
    Context& cxt;
    Expr&    obj;
    Expr& operator()(Name& n)      { lingo_unhandled(n); }
    Expr& operator()(Simple_id& n) { return make_member_ref(cxt, obj, n); }
  };
  return apply(n, fn{cxt, obj});
}


} // namespace banjo
