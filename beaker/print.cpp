// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "print.hpp"

#include <iostream>


namespace beaker
{

void
Printer::id(Name const* n)
{
  if (Qualified_id const* q = as<Qualified_id>(n))
    qualified_id(q);
  else
    unqualified_id(n);
}


void
Printer::unqualified_id(Name const* n)
{
  struct fn
  {
    Printer& p;
    void operator()(Simple_id const* n)      { p.unqualified_id(n); }
    void operator()(Global_id const* n)      { }
    void operator()(Placeholder_id const* n) { }
    void operator()(Operator_id const* n)    { p.operator_id(n); }
    void operator()(Conversion_id const* n)  { p.conversion_id(n); }
    void operator()(Literal_id const* n)     { p.literal_id(n); }
    void operator()(Destructor_id const* n)  { p.destructor_id(n); }
    void operator()(Template_id const* n)    { p.template_id(n); }
    void operator()(Qualified_id const* n)   { lingo_unreachable(); }
  };
  apply(n, fn{*this});
}


void
Printer::unqualified_id(Simple_id const* n)
{
  os << *n->symbol();
}


void
Printer::destructor_id(Destructor_id const*)
{
  os << '~';
  os << "<|type|>";
}


void
Printer::operator_id(Operator_id const*)
{
  os << "operator";
}


void
Printer::conversion_id(Conversion_id const*)
{
  os << "operator";
}


void
Printer::literal_id(Literal_id const*)
{
  os << "operator";
}


void
Printer::template_id(Template_id const*)
{
  os << "<|template-decl|>" << '<' << '>';
}


void
Printer::qualified_id(Qualified_id const* n)
{
  nested_name_specifier(n);
  while (is<Qualified_id>(n->name()))
    n = cast<Qualified_id>(n->name());
  unqualified_id(n->name());
}


void
Printer::nested_name_specifier(Qualified_id const* n)
{
  unqualified_id(n->context()->name());
  os << "::";
  if (Qualified_id const* q = as<Qualified_id>(n->name()))
    nested_name_specifier(q);
}

// -------------------------------------------------------------------------- //
// Printing of types

void
Printer::type(Type const* t)
{
  struct fn
  {
    Printer& p;
    void operator()(Void_type const* t)      { p.simple_type(t); }
    void operator()(Boolean_type const* t)   { p.simple_type(t); }
    void operator()(Integer_type const* t)   { p.simple_type(t); }
    void operator()(Float_type const* t)     { p.simple_type(t); }
    void operator()(Auto_type const* t)      { p.simple_type(t); }
    void operator()(Decltype_type const* t)  { p.simple_type(t); }
    void operator()(Declauto_type const* t)  { p.simple_type(t); }
    void operator()(Qualified_type const* t) { lingo_unreachable(); }
    void operator()(Pointer_type const* t)   { lingo_unreachable(); }
    void operator()(Reference_type const* t) { lingo_unreachable(); }
    void operator()(Array_type const* t)     { lingo_unreachable(); }
    void operator()(Sequence_type const* t)  { lingo_unreachable(); }
    void operator()(Class_type const* t)     { lingo_unreachable(); }
    void operator()(Union_type const* t)     { lingo_unreachable(); }
    void operator()(Enum_type const* t)      { lingo_unreachable(); }
  };
  apply(t, fn{*this});
}


void
Printer::simple_type(Void_type const* t)
{
  os << "void";
}


void
Printer::simple_type(Boolean_type const* t)
{
  os << "bool";
}


void
Printer::simple_type(Integer_type const* t)
{
  if (t->is_unsigned())
    os << 'u';
  os << "int" << t->precision();
}


void
Printer::simple_type(Float_type const* t)
{
  os << "float" << t->precision();
}


void
Printer::simple_type(Auto_type const* t)
{
  os << "auto";
}


// TODO: Implement me.
void
Printer::simple_type(Decltype_type const* t)
{
  os << "decltype(<|expr|>)";
}


void
Printer::simple_type(Declauto_type const* t)
{
  os << "decltype(auto)";
}


// -------------------------------------------------------------------------- //
// Declarations

void
Printer::declaration(Decl const* d)
{
  struct fn
  {
    Printer& p;
    void operator()(Variable_decl const* d)  { p.variable_declaration(d); }
    void operator()(Constant_decl const* d)  { p.constant_declaration(d); }
    void operator()(Function_decl const* d)  { p.function_declaration(d); }
    void operator()(Parameter_decl const* d) { p.parameter_declaration(d); }
    void operator()(Class_decl const* d)     { p.class_declaration(d); }
    void operator()(Union_decl const* d)     { p.union_declaration(d); }
    void operator()(Enum_decl const* d)      { p.enum_declaration(d); }
    void operator()(Namespace_decl const* d) { p.namespace_declaration(d); }
    void operator()(Template_decl const* d)  { p.template_declaration(d); }
  };
  apply(d, fn{*this});
}


// FIXME: Print the initializer.
void
Printer::variable_declaration(Variable_decl const* d)
{
  os << "var " << *d->type() << ' ' << *d->name() << ';';
}


// FIXME: Print the initializer.
void
Printer::constant_declaration(Constant_decl const* d)
{
  os << "const " << *d->type() << ' ' << *d->name() << ';';
}


// FIXME: Implement me.
void
Printer::function_declaration(Function_decl const* d)
{
  os << "def " << *d->name() << ';';
}


// FIXME: Print the default argument.
void
Printer::parameter_declaration(Parameter_decl const* d)
{
  os << *d->type() << ' ' << *d->name() << ';';
}


void
Printer::class_declaration(Class_decl const* d)
{
  lingo_unreachable();
}


void
Printer::union_declaration(Union_decl const* d)
{
  lingo_unreachable();
}


void
Printer::enum_declaration(Enum_decl const* d)
{
  lingo_unreachable();
}


// FIXME: Handle the global namespace and anonymous namespaces.
//
// FIXME: Print the list of members.
void
Printer::namespace_declaration(Namespace_decl const* d)
{
  os << "namespace " << *d->name() << " {";
  os << "}";
}


void
Printer::template_declaration(Template_decl const* d)
{
  lingo_unreachable();
}



// -------------------------------------------------------------------------- //
// Streaming

std::ostream&
operator<<(std::ostream& os, Name const& n)
{
  Printer print(os);
  print(&n);
  return os;
}


std::ostream&
operator<<(std::ostream& os, Type const& t)
{
  Printer print(os);
  print(&t);
  return os;
}


std::ostream&
operator<<(std::ostream& os, Decl const& d)
{
  Printer print(os);
  print(&d);
  return os;
}

} // namespace beaker
