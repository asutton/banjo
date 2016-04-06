// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "context.hpp"
#include "ast.hpp"
#include "builder.hpp"
#include "scope.hpp"
#include "token.hpp"

#include <lingo/io.hpp>


namespace banjo
{

Context::Context()
  : Builder(*this), syms(), id(0), tparms {-1, -1}, pholds {-1, -1}
  , diags(false)
{
  // Initialize the color system. This is a process-level
  // configuration. Perhaps we we should only initialize
  // colors if the default output terminal is actually the
  // output terminal.
  init_colors();

  // Initialize all the tokens.
  init_tokens(syms);

  // Initialize the global namepace.
  global = &get_global_namespace();
}


// -------------------------------------------------------------------------- //
// Scope management

// Enter the given scope. Unless `s` is the scope of the global
// namespace, `s` must be linked through its enclosing scopes
// to the global namespace.
//
// Do not call this function directly. Use Context::Scope_sentinel
// to enter a new scope, and guarantee cleanup and scope exit.
void
Context::set_scope(Scope& s)
{
  scope = &s;
}


// Returns a new general purpose scope.
Scope&
Context::make_scope()
{
  return *new Scope(current_scope());
}


// Create a new initializer scopee.
Initializer_scope&
Context::make_initializer_scope(Decl& d)
{
  return *new Initializer_scope(current_scope(), d);
}


// Create a new function scope.
Function_scope&
Context::make_function_scope(Decl& d)
{
  return *new Function_scope(current_scope(), d);
}


// Create a new function parameter scope.
Function_parameter_scope&
Context::make_function_parameter_scope()
{
  return *new Function_parameter_scope(current_scope());
}


Template_scope&
Context::make_template_scope()
{
  return *new Template_scope(current_scope());
}


// Create a new template parameter scope.
Template_parameter_scope&
Context::make_template_parameter_scope()
{
  return *new Template_parameter_scope(current_scope());
}


// Create a new block scope.
Block_scope&
Context::make_block_scope()
{
  return *new Block_scope(current_scope());
}


// Create a new requires scope.
Requires_scope&
Context::make_requires_scope()
{
  return *new Requires_scope(current_scope());
}


// Create a new concept scope.
Concept_scope&
Context::make_concept_scope(Decl& d)
{
  return *new Concept_scope(current_scope(), d);
}


// Create a new constrained scope.
Constrained_scope&
Context::make_constrained_scope(Expr& e)
{
  return *new Constrained_scope(current_scope(), e);
}


// Returns the current scope.
Scope&
Context::current_scope()
{
  return *scope;
}


// Returns the current template scope or nullptr if not
// currntly in the declaration of a template.
Template_scope*
Context::current_template_scope()
{
  Scope* p = scope;
  while (p) {
    if (Template_scope* t = as<Template_scope>(p))
      return t;
    p = p->enclosing_scope();
  }
  return nullptr;
}


// Returns the current template parameters, or nullptr if not
// in a current template. The resulting is empty only if the template
// parameters have not been established (e.g., parsed).
Decl_list*
Context::current_template_parameters()
{
  if (Template_scope* t = current_template_scope())
    return &t->parms;
  else
    return nullptr;
}


// Returns the current template constraints, or nullptr if the
// current template if no constraint is yet associated.
Expr*
Context::current_template_constraints()
{
  if (Template_scope* t = current_template_scope())
    return t->cons;
  else
    return nullptr;
}


// Returns the current template scope or nullptr if not
// currntly in the declaration of a template.
Requires_scope*
Context::current_requires_scope()
{
  Scope* p = scope;
  while (p) {
    if (Requires_scope* t = as<Requires_scope>(p))
      return t;
    p = p->enclosing_scope();
  }
  return nullptr;
}

// Find the innermost declaration context. This is the first
// scope associatd with a declaration. Note that we are guaranteed
// to have a current context since the outermost scope is the
// global namespace.
Decl&
Context::current_context()
{
  Scope* p = &current_scope();
  while (!p->context())
    p = p->enclosing_scope();
  return *p->context();
}


// Returns the current template declaration, or nullptr if not
// in a template declaration.
//
// NOTE: As it currently stands, the "current template" is not
// fully established until its declaration is actually declared.
// Consider:
//
//    template<typename T>
//    def f() -> int { ... }
//                  ^
// The point of declaration for f is indicated by the caret. This
// is when when the current template is valid. The following functions
// can provide information about templat parameters and constraints
// prior to the point of declaration:
//
// - current_template_scope
// - current_template_parameters
// - current_template_constraints
Template_decl*
Context::current_template()
{
  Scope* p = &current_scope();
  while (!p->context()) {
    if (Template_decl* t = as<Template_decl>(p->context()))
      return t;
    p = p->enclosing_scope();
  }
  return nullptr;
}



// -------------------------------------------------------------------------- //
// Enter scope

// Enter a new, general purpose scope. This is primarily used during
// the first pass of a parse, when names are only be associated with
// the kind of declaration and not a more specific type.
Enter_scope::Enter_scope(Context& cxt)
  : cxt(cxt), prev(&cxt.current_scope()), alloc(&cxt.make_scope())
{
  cxt.set_scope(*alloc);
}


// Enter the scope associated with a namespace definition.
Enter_scope::Enter_scope(Context& c, Namespace_decl& ns)
  : cxt(c), prev(&c.current_scope()), alloc(nullptr)
{
  cxt.set_scope(*ns.scope());
}


// Enter the given scope. This assumes ownership of the given
// scope and deletes it when the class goes out of scope.
Enter_scope::Enter_scope(Context& c, Scope& s)
  : cxt(c), prev(&c.current_scope()), alloc(&s)
{
  cxt.set_scope(*alloc);
}


// Restore the previous scope and delete any allocated scopes.
Enter_scope::~Enter_scope()
{
  cxt.set_scope(*prev);
  delete alloc;
}


} // namespace banjo
