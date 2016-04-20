// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#include "generator.hpp"

#include <banjo/ast.hpp>

#include <llvm/IR/Type.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include <iostream>


namespace banjo
{

// -------------------------------------------------------------------------- //
// Generation of names

// FIXME: Actually mangle names according to some language linkage.
String
Generator::get_name(Decl const& d)
{
  Name const& n = d.name();
  if (Simple_id const* id = as<Simple_id>(&n))
    return id->symbol().spelling();
  lingo_unhandled(n);
}


// -------------------------------------------------------------------------- //
// Mapping of types
//
// The type generator transforms a beaker type into its correspondiong
// LLVM type.


llvm::Type*
Generator::get_type(Type const& t)
{
  struct fn
  {
    Generator& g;
    llvm::Type* operator()(Type const& t)          { lingo_unhandled(t); }
    llvm::Type* operator()(Boolean_type const& t)  { return g.get_type(t); }
    llvm::Type* operator()(Integer_type const& t)  { return g.get_type(t); }
    llvm::Type* operator()(Float_type const& t)    { return g.get_type(t); }
    llvm::Type* operator()(Function_type const& t) { return g.get_type(t); }
  };
  return apply(t, fn{*this});
}


// Return the 1 bit integer type.
llvm::Type*
Generator::get_type(Boolean_type const&)
{
  return build.getInt1Ty();
}


// FIXME: This isn't realistic.
llvm::Type*
Generator::get_type(Integer_type const& t)
{
  return build.getInt32Ty();
}


// FIXME: This isn't realistic.
llvm::Type*
Generator::get_type(Float_type const&)
{
  return build.getDoubleTy();
}


// Return a function type.
llvm::Type*
Generator::get_type(Function_type const& t)
{
  std::vector<llvm::Type*> parms;
  parms.reserve(t.parameter_types().size());
  for (Type const& pt : t.parameter_types())
    parms.push_back(get_type(pt));
  llvm::Type* ret = get_type(t.return_type());
  return llvm::FunctionType::get(ret, parms, false);
}


#if 0
// -------------------------------------------------------------------------- //
// Code generation for expressions
//
// An expression is transformed into a sequence instructions whose
// intermediate results are saved in registers.

llvm::Value*
Generator::gen(Expr const* e)
{
  struct Fn
  {
    Generator& g;
    llvm::Value* operator()(Literal_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Id_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Decl_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Lambda_expr const* e) const { lingo_unreachable(); }
    llvm::Value* operator()(Add_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Sub_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Mul_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Div_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Rem_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Neg_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Pos_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Eq_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Ne_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Lt_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Gt_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Le_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Ge_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(And_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Or_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Not_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Call_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Dot_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Field_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Method_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Index_expr const* e) const { return g.gen(e); }
    llvm::Value* operator()(Value_conv const* e) const { return g.gen(e); }
    llvm::Value* operator()(Promote_conv const* e) const { return g.gen(e); }
    llvm::Value* operator()(Block_conv const* e) const { return g.gen(e); }
    llvm::Value* operator()(Base_conv const* e) const { return g.gen(e); }
    llvm::Value* operator()(Init const* e) const { lingo_unreachable(); }
  };

  return apply(e, Fn{*this});
}


namespace
{
struct Gen_init_fn
{
  Generator& g;
  llvm::Value* ptr;

  template<typename T>
  void operator()(T const*) { lingo_unreachable(); }

  void operator()(Default_init const* e) { g.gen_init(ptr, e); }
  void operator()(Trivial_init const* e) { g.gen_init(ptr, e); }
  void operator()(Copy_init const* e) { g.gen_init(ptr, e); }
  void operator()(Reference_init const* e) { g.gen_init(ptr, e); }
};


} // namespace


void
Generator::gen_init(llvm::Value* ptr, Expr const* e)
{
  apply(e, Gen_init_fn{*this, ptr});
}


// Return the value corresponding to a literal expression.
llvm::Value*
Generator::gen(Literal_expr const* e)
{
  // TODO: Write better type queries.
  //
  // TODO: Write a better interface for values.
  Value v = evaluate(e);
  Type const* t = e->type();
  if (t == get_boolean_type())
    return build.getInt1(v.get_integer());
  if (t == get_character_type())
    return build.getInt8(v.get_integer());
  if (t == get_integer_type())
    return build.getInt32(v.get_integer());

  // FIXME: How should we generate array literals? Are
  // these global constants or are they local alloca
  // objects. Does it depend on context?

  // A string literal produces a new global string constant.
  // and returns a pointer to an array of N characters.
  if (is_string(t)) {
    Array_value a = v.get_array();
    String s = a.get_string();

    // FIXME: This does not unify equivalent strings.
    // Maybe we needt maintain a mapping in order to
    // avoid redunancies.
    auto iter = strings.find(s);
    if (iter == strings.end()) {
      llvm::Value* v = build.CreateGlobalString(s);
      iter = strings.emplace(s, v).first;
    }
    return iter->second;
  }

  else
    throw std::runtime_error("cannot generate function literal");
}


llvm::Value*
Generator::gen(Id_expr const* e)
{
  lingo_unreachable();
}


// Returns the value associated with the declaration.
llvm::Value*
Generator::gen(Decl_expr const* e)
{
  auto const* bind = stack.lookup(e->declaration());
  llvm::Value* result = bind->second;

  // Fetch the value from a reference declaration.
  Decl const* decl = bind->first;

  if (is_reference(decl))
    return build.CreateLoad(result);
  return result;
}


llvm::Value*
Generator::gen(Add_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateAdd(l, r);
}


llvm::Value*
Generator::gen(Sub_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateSub(l, r);
}


llvm::Value*
Generator::gen(Mul_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateMul(l, r);
}


llvm::Value*
Generator::gen(Div_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateSDiv(l, r);
}


// FIXME: decide on unsigned or signed remainder
// based on types of expressions
llvm::Value*
Generator::gen(Rem_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateURem(l, r);
}


llvm::Value*
Generator::gen(Neg_expr const* e)
{
  llvm::Value* zero = build.getInt32(0);
  llvm::Value* val = gen(e->operand());
  return build.CreateSub(zero, val);
}


llvm::Value*
Generator::gen(Pos_expr const* e)
{
  return gen(e->operand());
}


llvm::Value*
Generator::gen(Eq_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateICmpEQ(l, r);
}


llvm::Value*
Generator::gen(Ne_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateICmpNE(l, r);
}


llvm::Value*
Generator::gen(Lt_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateICmpSLT(l, r);
}


llvm::Value*
Generator::gen(Gt_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateICmpSGT(l, r);
}


llvm::Value*
Generator::gen(Le_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateICmpSLE(l, r);
}


llvm::Value*
Generator::gen(Ge_expr const* e)
{
  llvm::Value* l = gen(e->left());
  llvm::Value* r = gen(e->right());
  return build.CreateICmpSGE(l, r);
}


llvm::Value*
Generator::gen(And_expr const* e)
{
  llvm::BasicBlock* head_block = build.GetInsertBlock();
  llvm::BasicBlock* tail_block = llvm::BasicBlock::Create(cxt, "", fn, head_block->getNextNode());
  llvm::BasicBlock* then_block = llvm::BasicBlock::Create(cxt, "", fn, tail_block);

  // Generate code for the left operand.
  llvm::Value* left = gen(e->left());
  build.CreateCondBr(left, then_block, tail_block);
  build.SetInsertPoint(then_block);

  // Generate code for the right operand.
  llvm::Value* right = gen(e->right());
  build.CreateBr(tail_block);
  build.SetInsertPoint(tail_block);

  llvm::PHINode* phi_inst = build.CreatePHI(build.getInt1Ty(), 2);
  phi_inst->addIncoming(build.getFalse(), head_block);
  phi_inst->addIncoming(right, then_block);
  return phi_inst;
}


llvm::Value*
Generator::gen(Or_expr const* e)
{
  llvm::BasicBlock* head_block = build.GetInsertBlock();
  llvm::BasicBlock* tail_block = llvm::BasicBlock::Create(cxt, "", fn, head_block->getNextNode());
  llvm::BasicBlock* then_block = llvm::BasicBlock::Create(cxt, "", fn, tail_block);

  // Generate code for the left operand.
  llvm::Value* left = gen(e->left());
  build.CreateCondBr(left, tail_block, then_block);
  build.SetInsertPoint(then_block);

  // Generate code for the right operand.
  llvm::Value* right = gen(e->right());
  build.CreateBr(tail_block);
  build.SetInsertPoint(tail_block);

  llvm::PHINode* phi_inst = build.CreatePHI(build.getInt1Ty(), 2);
  phi_inst->addIncoming(build.getTrue(), head_block);
  phi_inst->addIncoming(right, then_block);
  return phi_inst;
}


// Logical not is a simple XOR with the value true
// 1 xor 1 = 0
// 0 xor 1 = 1
llvm::Value*
Generator::gen(Not_expr const* e)
{
  llvm::Value* one = build.getTrue();
  llvm::Value* operand = gen(e->operand());
  return build.CreateXor(one, operand);
}


namespace
{

// Returns a method declaration if e is a virtual call
// to that method. Otherwise, returns nullptr.
inline Method_decl const*
calls_virtual_method(Call_expr const* e)
{
  if (Decl_expr const* d = as<Decl_expr>(e->target()))
    if (Method_decl const* m = as<Method_decl>(d->declaration()))
      if (m->is_polymorphic())
        return m;
  return nullptr;
}


} // namespace


llvm::Value*
Generator::gen(Call_expr const* e)
{
  // Generate the code for a virtual function call.
  //
  //    f(x, args...)
  //
  // If x is a polymorphic record type, then we want to
  // transform that to:
  //
  //    x.vptr[m](x, args...);
  //
  // where n is the offset of the f in the virtual table
  // of x's type.
  //
  // TODO: Consider representing virtual calls separately
  // within the AST. That would help simplify the code
  // generation a bit.
  //
  // TODO: If x refers to a variable of non-reference type,
  // then we can call the method directly, since x's dynamic
  // type is known.
  llvm::Value* fn;
  if (Method_decl const* m = calls_virtual_method(e)) {
    Expr_seq const& args = e->arguments();

    // Get (and load) the virtual function pointer.
    llvm::Value* vptr = gen_vptr(args.front());
    llvm::Value* a[] = {
      build.getInt32(0),
      build.getInt32(m->vtable_entry())
    };
    llvm::Value* vfpp = build.CreateInBoundsGEP(vptr, a);
    fn = build.CreateLoad(vfpp);
  } else {
    fn = gen(e->target());
  }

  std::vector<llvm::Value*> args;
  for (Expr const* a : e->arguments())
    args.push_back(gen(a));
  return build.CreateCall(fn, args);
}


// NOTE: The IR builder will automatically compact
// nested member expressions into a single GEP
// instruction. We don't have to do anything more
// complex than this.
llvm::Value*
Generator::gen(Dot_expr const* e)
{
  lingo_unreachable();
}


llvm::Value*
Generator::gen(Field_expr const* e)
{
  llvm::Value* obj = gen(e->container());

  // Build the GEP index array.
  Field_path const& p = e->path();
  std::vector<llvm::Value*> args(p.size() + 1);
  args[0] = build.getInt32(0);
  std::transform(p.begin(), p.end(), ++args.begin(), [&](int n) {
    return build.getInt32(n);
  });

  return build.CreateGEP(obj, args);
}


// Just generate the base object. This will be used
// as the argument for the method call.
llvm::Value*
Generator::gen(Method_expr const* e)
{
  return gen(e->container());
}


llvm::Value*
Generator::gen(Index_expr const* e)
{
  llvm::Value* arr = gen(e->array());
  llvm::Value* ix = gen(e->index());
  std::vector<llvm::Value*> args {
    build.getInt32(0), // 0th element from base
    ix                 // requested index
  };
  return build.CreateGEP(arr, args);
}


llvm::Value*
Generator::gen(Value_conv const* e)
{

  llvm::Value* v = gen(e->source());
  return build.CreateLoad(v);
}

llvm::Value*
Generator::gen(Promote_conv const* e)
{
  llvm::Value* v = gen(e->source());
  const Type * t = e->target();

  if(is<Integer_type>(t)) {
    const Integer_type * t2 = dynamic_cast<const Integer_type*>(t);
    return build.CreateIntCast(v, get_type(t2), t2->is_signed());
  }
  else if (is<Float_type>(t) || is<Double_type>(t)) {
    return build.CreateFPCast(v, get_type(t));
  }

  else
    return v;

}


llvm::Value*
Generator::gen(Block_conv const* e)
{
  // Generate the array value.
  llvm::Value* a = gen(e->source());

  // Decay the array pointer to an array into
  // a pointer to the first object. This effectively
  // returns a pointer to the first object in the
  // array.
  llvm::Value *zero = build.getInt32(0);
  llvm::Value *args[] = { zero, zero };
  return build.CreateInBoundsGEP(a, args);
}


// Build a GEP to the base class sub-object. Note that
// for derivation from the first base class, a bit-cast
// would be appropriate.
llvm::Value*
Generator::gen(Base_conv const* e)
{
  llvm::Value* a = gen(e->source());
  std::vector<llvm::Value*> args(e->path().size(), build.getInt32(0));
  return build.CreateGEP(a, args);
}

// TODO: Return the value or store it?
void
Generator::gen_init(llvm::Value* ptr, Default_init const* e)
{
  Type const* t = e->type();
  llvm::Type* type = get_type(t);
  llvm::Value* init = nullptr;

  // Scalar types should get a 0 value in the
  // appropriate type.
  if (is_scalar(t))
    init = llvm::ConstantInt::get(type, 0);

  // Aggregate types are zero initialized.
  //
  // NOTE: This isn't actually correct. Aggregate types
  // should be memberwise default initialized.
  if (is_aggregate(t))
    init = llvm::ConstantAggregateZero::get(type);

  if (init != nullptr) {
    build.CreateStore(init, ptr);
    return;
  }

  throw std::runtime_error("unhahndled default initializer");
}

void
Generator::gen_init(llvm::Value* ptr, Trivial_init const* e)
{
  return;
}

// TODO: Return the value or store it?
void
Generator::gen_init(llvm::Value* ptr, Copy_init const* e)
{
  llvm::Value* init = gen(e->value());
  build.CreateStore(init, ptr);
}


void
Generator::gen_init(llvm::Value* ptr, Reference_init const* e)
{
  llvm::Value* init = gen(e->object());
  build.CreateStore(init, ptr);
}


// -------------------------------------------------------------------------- //
// Code generation for statements
//
// The statement generator is responsible for
// the generation of statements at block scope.

void
Generator::gen(Stmt const* s)
{
  struct Fn
  {
    Generator& g;
    void operator()(Empty_stmt const* s) { g.gen(s); }
    void operator()(Block_stmt const* s) { g.gen(s); }
    void operator()(Assign_stmt const* s) { g.gen(s); }
    void operator()(Return_stmt const* s) { g.gen(s); }
    void operator()(If_then_stmt const* s) { g.gen(s); }
    void operator()(If_else_stmt const* s) { g.gen(s); }
    void operator()(While_stmt const* s) { g.gen(s); }
    void operator()(Break_stmt const* s) { g.gen(s); }
    void operator()(Continue_stmt const* s) { g.gen(s); }
    void operator()(Expression_stmt const* s) { g.gen(s); }
    void operator()(Declaration_stmt const* s) { g.gen(s); }
  };
  apply(s, Fn{*this});
}


void
Generator::gen(Empty_stmt const* s)
{
  // Do nothing.
}


// Generate code for a sequence of statements.
// Note that this does not correspond to a basic
// block since we don't need any terminators
// in the following program.
//
//    {
//      { ; }
//    }
//
// We only need new blocks for specific control
// flow concepts.
void
Generator::gen(Block_stmt const* s)
{
  for (Stmt const* s1 : s->statements())
    gen(s1);
}


void
Generator::gen(Assign_stmt const* s)
{
  llvm::Value* lhs = gen(s->object());
  llvm::Value* rhs = gen(s->value());
  build.CreateStore(rhs, lhs);
}


void
Generator::gen(Return_stmt const* s)
{
  llvm::Value* v = gen(s->value());
  build.CreateStore(v, ret);
  build.CreateBr(exit);
}


void
Generator::gen(If_then_stmt const* s)
{
  llvm::Value* cond = gen(s->condition());

  llvm::BasicBlock* then = llvm::BasicBlock::Create(cxt, "if.then", fn);
  llvm::BasicBlock* done = llvm::BasicBlock::Create(cxt, "if.done", fn);
  build.CreateCondBr(cond, then, done);

  // Emit the 'then' block
  build.SetInsertPoint(then);
  gen(s->body());
  then = build.GetInsertBlock();
  if (!then->getTerminator())
    build.CreateBr(done);

  // Emit the merge point.
  build.SetInsertPoint(done);
}


void
Generator::gen(If_else_stmt const* s)
{
  llvm::Value* cond = gen(s->condition());

  llvm::BasicBlock* then = llvm::BasicBlock::Create(cxt, "if.then", fn);
  llvm::BasicBlock* other = llvm::BasicBlock::Create(cxt, "if.else", fn);
  llvm::BasicBlock* done = llvm::BasicBlock::Create(cxt, "if.done", fn);
  build.CreateCondBr(cond, then, other);

  // Emit the then block.
  build.SetInsertPoint(then);
  gen(s->true_branch());
  then = build.GetInsertBlock();
  if (!then->getTerminator())
    build.CreateBr(done);

  // Emit the else block.
  build.SetInsertPoint(other);
  gen(s->false_branch());
  other = build.GetInsertBlock();
  if (!other->getTerminator())
    build.CreateBr(done);

  // Emit the done block.
  build.SetInsertPoint(done);
}


void
Generator::gen(While_stmt const* s)
{
  // Save the current loop information, to be restored
  // on scope exit.
  Loop_sentinel loop(*this);

  // Create the new loop blocks.
  top = llvm::BasicBlock::Create(cxt, "while.top", fn);
  bottom = llvm::BasicBlock::Create(cxt, "while.bottom", fn);
  llvm::BasicBlock* body = llvm::BasicBlock::Create(cxt, "while.body", fn, bottom);
  build.CreateBr(top);

  // Emit the condition test.
  build.SetInsertPoint(top);
  llvm::Value* cond = gen(s->condition());
  build.CreateCondBr(cond, body, bottom);

  // Emit the loop body.
  build.SetInsertPoint(body);
  gen(s->body());
  body = build.GetInsertBlock();
  if (!body->getTerminator())
    build.CreateBr(top);

  // Emit the bottom block.
  build.SetInsertPoint(bottom);
}


// Branch to the bottom of the current loop.
void
Generator::gen(Break_stmt const* s)
{
  build.CreateBr(bottom);
}


// Branch to the top of the current loop.
void
Generator::gen(Continue_stmt const* s)
{
  build.CreateBr(top);
}


void
Generator::gen(Expression_stmt const* s)
{
  gen(s->expression());
}


void
Generator::gen(Declaration_stmt const* s)
{
  gen(s->declaration());
}


// -------------------------------------------------------------------------- //
// Code generation for declarations
//
// TODO: We can't generate all of the code for a module
// in a single pass. We probably need to break this up
// into a number of smaller declaration generators. For
// example, generators that:
//
//    - produce declarations
//    - produce global initializers
//    - produce global destructors
//    - other stuff
//
// In, it might not be worthwhile to have a number
// of sub-generators that refer to the top-level
// generator.

void
Generator::gen(Decl const* d)
{
  struct Fn
  {
    Generator& g;
    void operator()(Super_decl const* d)  { return g.gen(d); }
    void operator()(Variable_decl const* d) { return g.gen(d); }
    void operator()(Function_decl const* d) { return g.gen(d); }
    void operator()(Parameter_decl const* d) { return g.gen(d); }
    void operator()(Record_decl const* d) { return g.gen(d); }
    void operator()(Field_decl const* d) { return g.gen(d); }
    void operator()(Method_decl const* d) { return g.gen(d); }
    void operator()(Module_decl const* d) { return g.gen(d); }
  };
  return apply(d, Fn{*this});
}

void
Generator::gen(Super_decl const* d)
{
  // TODO Do something here...
}

void
Generator::gen_local(Variable_decl const* d)
{
  // Create the alloca instruction at the beginning of
  // the function. Not at the point where we get it.
  llvm::BasicBlock& b = fn->getEntryBlock();
  llvm::IRBuilder<> tmp(&b, b.begin());
  llvm::Type* type = get_type(d->type());
  String name = d->name()->spelling();
  llvm::Value* ptr = tmp.CreateAlloca(type, nullptr, name);

  // Save the decl binding.
  stack.top().bind(d, ptr);

  // Generate the initializer.
  gen_init(ptr, d->init());

  // If the variable has polymorphic record type,
  // initialize its vref to the appropriate table.
  //
  // FIXME: This is totally broken if it doesn't
  // happen before initialization. Also, note that
  // polymorphic types cannot be zero-initialized.
  // Only member-wise initialized.
  if (Record_type const* rt = as<Record_type>(d->type())) {
    Record_decl const* rec = rt->declaration();
    llvm::Value* vtbl = vtables.find(rec)->second;
    llvm::Value* vref = gen_vref(rec, ptr);
    build.CreateStore(vtbl, vref);
  }
}


void
Generator::gen_global(Variable_decl const* d)
{
  String      name = get_name(d);
  llvm::Type* type = get_type(d->type());

  // Try to generate a constant initializer.
  //
  // FIXME: If the initializer can be reduced to a value,
  // then generate that constant. If not, we need dynamic
  // initialization of global variables.
  llvm::Constant* init = nullptr;
  if (!d->is_foreign())
    init = llvm::Constant::getNullValue(type);


  // Note that the aggregate 0 only applies to aggregate
  // types. We can't apply it to initializers for scalars.

  // Build the global variable, automatically adding
  // it to the module.
  llvm::GlobalVariable* var = new llvm::GlobalVariable(
    *mod,                                  // owning module
    type,                                  // type
    false,                                 // is constant
    llvm::GlobalVariable::ExternalLinkage, // linkage,
    init,                                  // initializer
    name                                   // name
  );

  // Create a binding for the new variable.
  stack.top().bind(d, var);
}


// Generate code for a variable declaration. Note that
// code generation depends heavily on context. Globals
// and locals are very different.
//
// TODO: If we add class/record types, then we also
// need to handle member variables as well. Maybe.
void
Generator::gen(Variable_decl const* d)
{
  if (is_global_variable(d))
    return gen_global(d);
  else
    return gen_local(d);
}


void
Generator::gen(Function_decl const* d)
{

  String name = get_name(d);
  llvm::Type* type = get_type(d->type());

  // Build the function.
  llvm::FunctionType* ftype = llvm::cast<llvm::FunctionType>(type);
  fn = llvm::Function::Create(
    ftype,                           // function type
    llvm::Function::ExternalLinkage, // linkage
    name,                            // name
    mod);                            // owning module

  // Create a new binding for the variable.
  stack.top().bind(d, fn);

  // If the declaration is not defined, then don't
  // do any of this stuff...
  if (!d->body())
    return;

  // Establish a new binding environment for declarations
  // related to this function.
  Symbol_sentinel scope(*this);

  // Build the argument list. Note that
  {
    auto ai = fn->arg_begin();
    auto pi = d->parameters().begin();
    while (ai != fn->arg_end()) {
      Decl const* p = *pi;
      llvm::Argument* a = &*ai;
      a->setName(p->name()->spelling());

      // Create an initial name binding for the function
      // parameter. Note that we're going to overwrite
      // this when we create locals for each parameter.
      stack.top().bind(p, a);

      ++ai;
      ++pi;
    }
  }

  // Build the entry and exit blocks for the function.
  entry = llvm::BasicBlock::Create(cxt, "entry", fn);
  exit = llvm::BasicBlock::Create(cxt, "exit");
  build.SetInsertPoint(entry);

  // Build the return value.
  ret = build.CreateAlloca(fn->getReturnType());

  // Generate a local variable for each of the variables.
  for (Decl const* p : d->parameters())
    gen(p);
  gen(d->body());
  entry = build.GetInsertBlock();
  if (!entry->getTerminator())
    build.CreateBr(exit);

  // Insert the exit block and generate the actual
  // return statement,
  fn->getBasicBlockList().push_back(exit);
  build.SetInsertPoint(exit);
  build.CreateRet(build.CreateLoad(ret));

  // Reset stateful info.
  ret = nullptr;
  fn = nullptr;
}


void
Generator::gen(Parameter_decl const* d)
{
  llvm::Type* t = get_type(d->type());
  llvm::Value* a = stack.top().get(d).second;
  llvm::Value* v = build.CreateAlloca(t);
  stack.top().rebind(d, v);
  build.CreateStore(a, v);
}


// Generate a new struct type.
void
Generator::gen(Record_decl const* d)
{
  // If we've already created a type, don't do
  // anything else.
  if (types.lookup(d))
    return;

  std::vector<llvm::Type*> ts;
  ts.reserve(16);

  // If d is the root of a polymorphic type hierarchy,
  // then generate a vptr as its first sub-object. This is
  // represented as an i8* since we haven't generated the
  // the table yet.
  if (Decl const* vr = d->vref())
    ts.push_back(get_type(vr->type()));

  // Add the base class sub-object before fields.
  //
  // TODO: Implement the empty base optimization.
  if (Type const* b = d->base())
    ts.push_back(get_type(b));

  // Construct the type over only the fields. If the record
  // is empty, generate a struct with exactly one  byte so that
  // we never have a type with 0 size.
  if (d->fields().empty()) {
    ts.push_back(build.getInt8Ty());
  } else {
    for (Decl const* f : d->fields())
      ts.push_back(get_type(f->type()));
  }

  // This will automatically be added to the module,
  // but if it's not used, then it won't be generated.
  llvm::Type* t = llvm::StructType::create(cxt, ts, d->name()->spelling());
  types.bind(d, t);

  // Now, generate code for all other members.
  for (Decl const* m : d->members())
    gen(m);

  // Finally, generate the vtable.
  if (d->is_polymorphic())
    gen_vtable(d);
}


void
Generator::gen(Field_decl const* d)
{
  // NOTE: We should never actually get here.
  lingo_unreachable();
}



// Just call out to the function generator. Name
// mangling is handled in get_name().
void
Generator::gen(Method_decl const* d)
{
  gen(cast<Function_decl>(d));
}


void
Generator::gen(Module_decl const* d)
{
  // Establish the global binding environment.
  Symbol_sentinel scope(*this);

  // Initialize the module.
  //
  // TODO: Make the output name the ".ll" version of the
  // the input name. Although this might also depend on
  // whether we're generating IR or object code?
  assert(!mod);
  mod = new llvm::Module("a.ll", cxt);

  // Generate all top-level declarations.
  for (Decl const* d1 : d->declarations())
    gen(d1);

  // TODO: Make a second pass to generate global
  // constructors for initializers.
}


llvm::Value*
Generator::gen_vtable(Record_decl const* d)
{
  Decl_seq const& vtbl = *d->vtable();

  // Build the vtable type. This is just an array
  // of character pointers. The call expression re-casts
  // to the appropriate static type.
  //
  // TODO: The type is unnamed. Does this actually matter?
  std::vector<llvm::Type*> types;
  std::vector<llvm::Constant*> values;
  for (Decl const* d : vtbl) {
    llvm::Type* t = llvm::PointerType::getUnqual(get_type(d->type()));
    types.push_back(t);

    llvm::Value* v = stack.lookup(d)->second;
    llvm::Function* f = llvm::cast<llvm::Function>(v);
    llvm::Constant* p = llvm::ConstantExpr::getBitCast(f, t);
    values.push_back(p);
  }

  // Build the type and initializer.
  String base = mangle(d);
  String vtn = "_VT_" + base;
  String vttn = "_VTT_" + base;
  llvm::StructType* vtt = llvm::StructType::create(cxt, types, vttn);
  llvm::Constant* vti = llvm::ConstantStruct::get(vtt, values);

  // Generate the vtable global.
  llvm::GlobalVariable* ret = new llvm::GlobalVariable(
    *mod,                                  // owning module
    vtt,                                   // type
    true,                                  // is constant
    llvm::GlobalVariable::ExternalLinkage, // linkage,
    vti,                                   // initializer
    vtn                                    // name
  );
  vtables.emplace(d, ret);
  return ret;
}


// Generate an expression that accesses the virtual
// table within the object. Here, expr is the first
// argument of the function call.
llvm::Value*
Generator::gen_vptr(Expr const* e)
{
  Decl_expr const* d = cast<Decl_expr>(e);
  llvm::Value* obj = gen(d);

  Record_type const* t = cast<Record_type>(d->type()->nonref());
  Record_decl const* r = t->declaration();
  return gen_vptr(r, obj);
}


// Returns the vtable pointer.
llvm::Value*
Generator::gen_vptr(Record_decl const* r, llvm::Value* obj)
{
  llvm::Value* ref = gen_vref(r, obj);
  return build.CreateLoad(ref);
}


// Returns a reference to the vptr that's suitable for
// storing a value.
llvm::Value*
Generator::gen_vref(Record_decl const* r, llvm::Value* obj)
{
  std::vector<llvm::Value*> args { build.getInt32(0) };
  Record_decl const* p = r;
  while (!p->vref()) {
    args.push_back(build.getInt32(0));
    p = p->base_declaration();
  }
  args.push_back(build.getInt32(0));

  llvm::Value* ref = build.CreateInBoundsGEP(obj, args);
  llvm::Value* vtbl = vtables.find(r)->second;
  llvm::Type* type = llvm::PointerType::getUnqual(vtbl->getType());
  return build.CreateBitCast(ref, type);
}


llvm::Module*
Generator::operator()(Decl const* d)
{
  assert(is<Module_decl>(d));
  gen(d);
  return mod;
}

#endif

} // namespace
