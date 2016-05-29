// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "expression.hpp"
#include "ast.hpp"
#include "template.hpp"
#include "context.hpp"
#include "lookup.hpp"
#include "type.hpp"
#include "printer.hpp"

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Helper functions
//
// The following functions determine the type and value category of the
// id-expressions, depending on the kind of declaration named.


// Returns the expression category for an id referring to a declared object.
Category
get_category(Object_decl const& d)
{
  if (d.is_consume_reference())
    return cref_expr;
  else
    return nref_expr;
}


// The identifier is not an object or function.
static Expr&
make_error_ref(Context& cxt, Decl& d)
{
  // Here are some things that lookup can find that are not
  // valid expressions.
  //
  // TODO: Diagnose the error and point to the declaration.
  if (Type_decl* t = as<Type_decl>(&d))
    throw Type_error("'{}' is not an object or function", t->name());

  lingo_unhandled(d);
}


// Returns a reference to the declared object. A name denoting a variable
// or function parameter is a reference expression whose type is that
// of the declared variable or parameter. If the declaration is declared
// with the consume reference specifier, then the expression is a consumable
// reference.
static Expr&
make_object_ref(Context& cxt, Object_decl& d)
{
  return cxt.make_reference(get_category(d), d.type(), d);
}


// Returns a reference to the declared constant. A name denoting a value
// is a value expression with the declared type of the value.
static Expr&
make_value_ref(Context& cxt, Value_decl& d)
{
  return cxt.make_reference(val_expr, d.type(), d);
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
  return cxt.make_reference(nref_expr, d.type(), d);
}


// Return a reference to the given declaration based on the kind
// of the resolved declaration.
static Expr&
make_decl_ref(Context& cxt, Decl& decl)
{
  struct fn
  {
    Context& cxt;
    Expr& operator()(Decl& d)          { lingo_unhandled(d); }
    Expr& operator()(Object_decl& d)   { return make_object_ref(cxt, d); }
    Expr& operator()(Value_decl& d)    { return make_value_ref(cxt, d); }
    Expr& operator()(Function_decl& d) { return make_function_ref(cxt, d); }
  };
  return apply(decl, fn{cxt});
}


// Return a reference to an overload set.
static Expr&
make_ovl_ref(Context& cxt, Name& n, Decl_list&& ds)
{
  return cxt.make_reference(n, std::move(ds));
}


// Returns an expression referring to a class field. A field expression
// is a reference value expression whose type is that of the declared 
// field. If the field is declared with the consume specifier, then the 
// expression computes a consumable value.
static Expr&
make_field_ref(Context& cxt, Expr& e, Field_decl& d)
{
  return cxt.make_reference(get_category(d), d.type(), e, d);
}


// Returns an expression referring to a class method. A method expression
// is a reference value expression whose type is that of the function.
static Expr&
make_method_ref(Context& cxt, Expr& e, Method_decl& d)
{
  return cxt.make_reference(nref_expr, d.type(), e, d);
}


// Note that member references to static data members are not
// actually member expressions; we can discard the computed object
// and simply refer to the core value.
static Expr&
make_member_decl_ref(Context& cxt, Expr& obj, Decl& decl)
{
  struct fn
  {
    Context& cxt;
    Expr& obj;

    Expr& operator()(Decl& d)          { return make_error_ref(cxt, d); }

    // Non-members.
    Expr& operator()(Object_decl& d)   { return make_object_ref(cxt, d); }
    Expr& operator()(Value_decl& d)    { return make_value_ref(cxt, d); }
    Expr& operator()(Function_decl& d) { return make_function_ref(cxt, d); }
    Expr& operator()(Macro_decl& d)    { return make_function_ref(cxt, d); }
    
    // Members.
    Expr& operator()(Field_decl& d)    { return make_field_ref(cxt, obj, d); }
    Expr& operator()(Method_decl& d)   { return make_method_ref(cxt, obj, d); }
  };
  return apply(decl, fn{cxt, obj});
}


// Construct a reference to an unresolved (overloaded) member.
static Expr&
make_member_ovl_ref(Context& cxt, Expr& obj, Name& name, Decl_list&& ds)
{
  return cxt.make_reference(obj, name, std::move(ds));
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
    return make_decl_ref(cxt, decls.front());
  else
    return make_ovl_ref(cxt, id, std::move(decls));
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
    Expr& operator()(Name& n)        { banjo_unhandled_case(n); }
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
    return make_member_decl_ref(cxt, obj, decls.front());
  else
    return make_member_ovl_ref(cxt, obj, name, std::move(decls));
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
