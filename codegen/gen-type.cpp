// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved


namespace banjo
{

namespace ll
{

// -------------------------------------------------------------------------- //
// Mapping of types
//
// The type generator transforms a beaker type into its corresponding
// LLVM type.


llvm::Type*
Generator::gen_type(Type const& t)
{
  struct fn
  {
    Generator& g;

    llvm::Type* operator()(Type const& t)          { lingo_unhandled(t); }
    llvm::Type* operator()(Void_type const& t)     { return g.gen_type(t); }
    llvm::Type* operator()(Boolean_type const& t)  { return g.gen_type(t); }
    llvm::Type* operator()(Integer_type const& t)  { return g.gen_type(t); }
    llvm::Type* operator()(Byte_type const& t)     { return g.gen_type(t); }
    llvm::Type* operator()(Float_type const& t)    { return g.gen_type(t); }
    llvm::Type* operator()(Function_type const& t) { return g.gen_type(t); }
    llvm::Type* operator()(Class_type const& t)    { return g.gen_type(t); }
    llvm::Type* operator()(Array_type const& t)    { return g.gen_type(t); }
    llvm::Type* operator()(Tuple_type const& t)    { return g.gen_type(t); }
    llvm::Type* operator()(Pointer_type const& t)  { return g.gen_type(t); }
   };
  return apply(t, fn{*this});
}


llvm::Type*
Generator::gen_type(Void_type const&)
{
  return build.getVoidTy();
}


// Return the 1 bit integer type.
llvm::Type*
Generator::gen_type(Boolean_type const&)
{
  return build.getInt1Ty();
}


// FIXME: This isn't realistic.
llvm::Type*
Generator::gen_type(Integer_type const& t)
{
  return build.getInt32Ty();
}

llvm::Type*
Generator::gen_type(Byte_type const& t)
{
  return build.getInt8Ty();
}


// FIXME: This isn't realistic.
llvm::Type*
Generator::gen_type(Float_type const&)
{
  return build.getDoubleTy();
}


// Return a function type.
llvm::Type*
Generator::gen_type(Function_type const& t)
{
  std::vector<llvm::Type*> parms;
  parms.reserve(t.parameter_types().size());
  for (Type const& pt : t.parameter_types())
    parms.push_back(gen_type(pt));
  llvm::Type* ret = gen_type(t.return_type());
  return llvm::FunctionType::get(ret, parms, false);
}


// Return an array type.
//
// FIXME: The complete array type's extent should already have been
// evaluated. 
llvm::Type*
Generator::gen_type(Array_type const& t) 
{
  llvm::Type* t1 = gen_type(t.element_type());
  Value v = evaluate(banjo, t.extent());
  return llvm::ArrayType::get(t1, v.get_integer());
}


// Return a tuple type.
llvm::Type*
Generator::gen_type(Tuple_type const& t)
{
  Type_list const& ts = t.element_types();

  // Handle the empty case specially.
  if (ts.empty())
    return llvm::StructType::get(cxt);

  // Generate a literal struct type.
  std::vector<llvm::Type*> types;
  types.reserve(t.element_types().size());
  for (Type const& t1 : t.element_types())
    types.push_back(gen_type(t1));
  return llvm::StructType::get(cxt, types);
}


// Return a tuple type.
llvm::Type*
Generator::gen_type(Pointer_type const& t)
{
  llvm::Type* inner = gen_type(t.type());
  return llvm::PointerType::getUnqual(inner);
}


llvm::Type*
Generator::gen_type(Class_type const& t)
{
  auto const* bind = types.lookup(&t.declaration());
  if (!bind)
  {
    gen(as<Class_decl>(t.declaration()));
    bind = types.lookup(&t.declaration());
  }
  return bind->second;
}


} // namespace ll

} // namespace banjo
