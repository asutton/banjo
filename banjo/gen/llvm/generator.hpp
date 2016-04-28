// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_GENERATOR_HPP
#define BANJO_GENERATOR_HPP

// An LLVM code generator based on the LLVM IR builder.

#include <banjo/language.hpp>
#include <banjo/ast.hpp>

#include <lingo/environment.hpp>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

#include <stack>


namespace banjo
{

namespace ll
{


// The three different kinds of contexts in which
// declarations can appear.
enum {
  invalid_cxt,
  global_cxt,
  function_cxt,
  type_cxt,
};


// Used to maintain a mapping of Beaker declarations to their corresponding
// LLVM declarations. This is used to track the names of globals and
// parameters.
using Symbol_env = Environment<Decl const*, llvm::Value*>;
using Symbol_stack = Stack<Symbol_env>;


// Like the symbol environment, except that all
// type annotations are global.
using Type_env = Environment<Decl const*, llvm::Type*>;


struct Generator
{
  Generator();

  llvm::Module* operator()(Stmt const&);

  String get_name(Decl const&);

  llvm::Type* get_type(Type const&);
  llvm::Type* get_type(Void_type const&);
  llvm::Type* get_type(Boolean_type const&);
  llvm::Type* get_type(Integer_type const&);
  llvm::Type* get_type(Float_type const&);
  llvm::Type* get_type(Function_type const&);
  llvm::Type* get_type(Auto_type const&);

  llvm::Value* gen(Expr const&);
  llvm::Value* gen(Boolean_expr const&);
  llvm::Value* gen(Integer_expr const&);
  llvm::Value* gen(Real_expr const&);
  llvm::Value* gen(Add_expr const&);
  llvm::Value* gen(Sub_expr const&);
  llvm::Value* gen(Mul_expr const&);
  llvm::Value* gen(Div_expr const&);
  llvm::Value* gen(Rem_expr const&);
  llvm::Value* gen(Neg_expr const&);
  llvm::Value* gen(Pos_expr const&);
  llvm::Value* gen(Eq_expr const&);
  llvm::Value* gen(Ne_expr const&);
  llvm::Value* gen(Lt_expr const&);
  llvm::Value* gen(Gt_expr const&);
  llvm::Value* gen(Le_expr const&);
  llvm::Value* gen(Ge_expr const&);
  llvm::Value* gen(Cmp_expr const&);
  llvm::Value* gen(And_expr const&);
  llvm::Value* gen(Or_expr const&);
  llvm::Value* gen(Not_expr const&);
  llvm::Value* gen(Call_expr const&);

  void gen(Stmt const&);
  void gen(Empty_stmt const&);
  void gen(Translation_stmt const&);
  void gen(Member_stmt const&);
  void gen(Compound_stmt const&);
  void gen(Return_stmt const&);
  void gen(If_then_stmt const&);
  void gen(If_else_stmt const&);
  void gen(While_stmt const&);
  void gen(Break_stmt const&);
  void gen(Continue_stmt const&);
  void gen(Expression_stmt const&);
  void gen(Declaration_stmt const&);
  void gen(Stmt_list const&);


  void gen(Decl const&);
  void gen(Variable_decl const&);
  void gen_local_variable(Variable_decl const&);
  void gen_global_variable(Variable_decl const&);
  void gen(Function_decl const&);
  void gen_function_definition(Def const&);
  void gen_function_definition(Function_def const&);
  void gen(Type_decl const&);
  void gen(Object_parm const&);

  // Name bindings
  void declare(Decl const&, llvm::Value*);
  llvm::Value* lookup(Decl const&);

  // The context and default IR builder.
  llvm::LLVMContext cxt;
  llvm::IRBuilder<> build;

  // The current module.
  llvm::Module*     mod;

  // Information about the current function.
  llvm::Function*   fn;
  llvm::Value*      ret;
  llvm::BasicBlock* entry; // Function entry
  llvm::BasicBlock* exit;  // Function exit
  llvm::BasicBlock* top;   // Loop top
  llvm::BasicBlock* bot;   // Loop bottom

  // Environment.
  int           declcxt; // The current declaration context
  Symbol_stack  stack;   // Local symbol names
  Type_env      types;   // Declared types

  struct Enter_context;
  struct Enter_loop;
};


inline
Generator::Generator()
  : cxt(), build(cxt), mod(nullptr), declcxt(invalid_cxt)
{ }


inline void 
Generator::declare(Decl const& d, llvm::Value* v)
{
  stack.top().bind(&d, v);
}


inline llvm::Value*
Generator::lookup(Decl const& d)
{
  return stack.top().get(&d).second;
}


// An RAII class used to manage the registration and
// removal of name-to-value bindings for code generation.
struct Generator::Enter_context
{
  Enter_context(Generator& g, int c)
    : gen(g), prev(g.declcxt)
  {
    gen.declcxt = c;
    gen.stack.push();
  }

  ~Enter_context()
  {
    gen.declcxt = prev;
    gen.stack.pop();
  }

  Generator& gen;
  int        prev;
};



// An RAII class that manages the top and bottom
// blocks of loops. These are the current jump
// targets for the break and continue statements.
struct Generator::Enter_loop
{
  Enter_loop(Generator& g)
    : gen(g), top(gen.top), bot(gen.bot)
  {
  }

  ~Enter_loop()
  {
    gen.top = top;
    gen.bot = bot;
  }

  Generator& gen;
  llvm::BasicBlock* top;  // Pevious loop top
  llvm::BasicBlock* bot;  // Previos loop bottom
};



} // namespace ll

} // namespace banjo


#endif
