// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_ELABORATION_HPP
#define BANJO_ELABORATION_HPP

#include "language.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "ast.hpp"


namespace banjo
{

struct Parser;


// This defines the set of functions available to all elaborators.
// Derived classes should overload only those that they need.
struct Basic_elaborator
{
  Basic_elaborator(Parser& p)
    : cxt(p.cxt), parser(p)
  { }

  // Translation
  void start_translation_unit(Translation_unit&) { }
  void finish_translation_unit(Translation_unit&) { }

  // Statements
  void on_statement(Stmt&) { }
  void start_compound_statement(Compound_stmt&) { }
  void finish_compound_statement(Compound_stmt&) { }
  void on_empty_statement(Empty_stmt&) { }
  void on_return_statement(Return_stmt&) { }
  void on_yield_statement(Yield_stmt&) { }
  void on_if_statement(If_then_stmt&) { }
  void on_if_statement(If_else_stmt&) { }
  void on_while_statement(While_stmt&) { }
  void on_break_statement(Break_stmt&) { }
  void on_continue_statement(Continue_stmt&) { }
  void on_declaration_statement(Declaration_stmt&) { }
  void on_expression_statement(Expression_stmt&) { }

  // Declarations
  void on_declaration(Decl&) { }

  void on_variable_declaration(Variable_decl&) { }

  void start_function_declaration(Function_decl&) { }
  void finish_function_declaration(Function_decl&) { }
  void on_parameter(Object_parm&) { }
  void on_function_body(Function_def&) { }
  void on_function_body(Expression_def&) { }
  
  void start_class_declaration(Class_decl&) { }
  void finish_class_declaration(Class_decl&) { }
  void on_super_declaration(Super_decl&) { }
  
  // Types
  void on_type(Type&) { }

  // Expressions
  void on_expression(Expr&) { }

  Context& cxt;
  Parser&  parser;
};


// An elaborator is a grammar-directed recursive visitor. It is 
// parameterized by a function object that contains the semantics 
// required by the visitor. The purpose of this tool is to make it
// easier to write simple elaboration/type checking passes over a
// program.
//
// Again, note that this is a *grammer* directed visitor. It is not
// based on the abstract syntax of the language.
template<typename F>
struct Elaborator
{
  using Self = Elaborator;

  Elaborator(Parser& p, F f)
    : cxt(p.cxt), parser(p), vis(f)
  { }

  void operator()(Translation_unit& s) { translation_unit(s); }

  void translation_unit(Translation_unit&);

  void statement(Stmt&);
  void statement_seq(Stmt_list&);
  void compound_statement(Compound_stmt&);
  void return_statement(Return_stmt&);
  void yield_statement(Yield_stmt&);
  void if_statement(If_then_stmt&);
  void if_statement(If_else_stmt&);
  void while_statement(While_stmt&);
  void break_statement(Break_stmt&);
  void continue_statement(Continue_stmt&);
  void declaration_statement(Declaration_stmt&);
  void expression_statement(Expression_stmt&);

  void declaration(Decl&);

  void variable_declaration(Variable_decl&);

  void function_declaration(Function_decl&);
  void parameter_list(Function_decl&);
  void parameter(Decl&);
  void parameter(Object_parm&);
  void function_body(Def&);
  void function_body(Function_def&);
  void function_body(Expression_def&);

  void class_declaration(Class_decl&);
  void class_body(Class_def&);
  void super_declaration(Super_decl&);


  // Types
  Type& types(Type&);

  // Expressions
  Expr& expression(Expr&);

  Parser&  parser;
  Context& cxt;
  F        vis;
};


template<typename F>
inline void
Elaborator<F>::translation_unit(Translation_unit& tu)
{
  Enter_scope scope(cxt, tu);
  vis.start_translation_unit(tu);

  // FIXME: This should be toplevel-stmt-seq
  statement_seq(tu.statements());
  
  vis.finish_translation_unut(tu);
}


template<typename F>
inline void
Elaborator<F>::statement(Stmt& s)
{
  struct fn
  {
    Self& elab;
    void operator()(Stmt& s)             { lingo_unhandled(s); }
    void operator()(Compound_stmt& s)    { elab.compound_statement(s); }
    void operator()(Return_stmt& s)      { elab.return_statement(s); }
    void operator()(Yield_stmt& s)       { elab.yield_statement(s); }
    void operator()(If_then_stmt& s)     { elab.if_statement(s); }
    void operator()(If_else_stmt& s)     { elab.if_statement(s); }
    void operator()(While_stmt& s)       { elab.while_statement(s); }
    void operator()(Break_stmt& s)       { elab.break_statement(s); }
    void operator()(Continue_stmt& s)    { elab.continue_statement(s); }
    void operator()(Declaration_stmt& s) { elab.declaration_statement(s); }
    void operator()(Expression_stmt& s)  { elab.expression_statement(s); }
  };
  vis.on_statement(s);
  apply(s, fn{*this});
}


template<typename F>
inline void
Elaborator<F>::statement_seq(Stmt_list& ss)
{
  for (Stmt& s : ss)
    statement(s);
}


template<typename F>
inline void
Elaborator<F>::compound_statement(Compound_stmt& s)
{
  Enter_scope scoppe(cxt, s);
  vis.start_compound_statement(s);
  statement_seq(s);
  vis.finish_compound_statement(s);
}


template<typename F>
inline void
Elaborator<F>::return_statement(Return_stmt& s)
{
  vis.on_return_statement(s);
}


template<typename F>
inline void
Elaborator<F>::yield_statement(Yield_stmt& s)
{
  vis.on_yield_statement(s);
}


template<typename F>
inline void
Elaborator<F>::if_statement(If_then_stmt& s)
{
  vis.on_if_statement(s);
}


template<typename F>
inline void
Elaborator<F>::if_statement(If_else_stmt& s)
{
  vis.on_if_statement(s);
}


template<typename F>
inline void
Elaborator<F>::while_statement(While_stmt& s)
{
  vis.on_while_statement(s);
}


template<typename F>
inline void
Elaborator<F>::declaration_statement(Declaration_stmt& s)
{
  vis.on_declaration_stmt(s);
}


template<typename F>
inline void
Elaborator<F>::expression_statement(Expression_stmt& s)
{
  vis.on_expression_stmt(s);
}



template<typename F>
inline void
Elaborator<F>::declaration(Decl& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Decl& d)          { lingo_unhandled(d); }
    void operator()(Variable_decl& d) { elab.variable_declaration(d); }
    void operator()(Function_decl& d) { elab.function_declaration(d); }
    void operator()(Class_decl& d)    { elab.class_declaration(d); }
    void operator()(Super_decl& d)    { elab.super_declaration(d); }
    void operator()(Field_decl& d)    { elab.field_declaration(d); }
    void operator()(Method_decl& d)   { elab.method_declaration(d); }
  };
  vis.on_declaration(d);
  apply(d, fn{(*this)});
}


template<typename F>
inline void
Elaborator<F>::variable_declaration(Variable_decl& d)
{
  vis.on_variable_declaration(d);
}


template<typename F>
inline void
Elaborator<F>::function_declaration(Function_decl& d)
{
  Enter_scope scope(cxt, d);
  vis.start_function_declaration(d);
  parameter_list(d);
  function_body(d);
  vis.finish_function_declaration(d);
}


template<typename F>
inline void
Elaborator<F>::parameter_list(Function_decl& d)
{
  for (Decl& d : d.parameters())
    parameter(d);
}


template<typename F>
inline void
Elaborator<F>::parameter(Decl& p)
{
  struct fn
  {
    Self& elab;
    void operator()(Decl& p)        { lingo_unhandled(p); }
    void operator()(Object_parm& p) { elab.parameter(p); }
  };
  apply(p, fn{*this});
}


template<typename F>
inline void
Elaborator<F>::parameter(Object_parm& d)
{
  vis.on_parameter(d);
}


template<typename F>
inline void
Elaborator<F>::function_body(Def& d)
{
  struct fn
  {
    Self& elab;
    void operator()(Def& d)            { lingo_unhandled(d); }
    void operator()(Function_def& d)   { elab.function_body(d); }
    void operator()(Expression_def& d) { elab.function_body(d); }
  };
}


template<typename F>
inline void
Elaborator<F>::function_body(Function_def& d)
{
  vis.on_on_function_body(d);
  statement(d.statement());
}


template<typename F>
inline void
Elaborator<F>::function_body(Expression_def& d)
{
  vis.on_function_body(d);
  expression(d.expression());
}


template<typename F>
inline void
Elaborator<F>::class_declaration(Class_decl& d)
{
  Enter_scope scope(cxt, d);
  vis.start_class_declaration(d);
  class_body(d.definition());
  vis.finish_class_declaration(d);
}

template<typename F>
inline void
Elaborator<F>::super_declaration(Super_decl& d)
{

}


} // nammespace banjo


#endif
