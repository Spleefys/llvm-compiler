#include "codegen.h"
#include "node.h"

static Type* typeOf(CodeGenContext& context, const NIdentifier& type) {
  return Type::getInt32Ty(context.getContext());
}

Value* NInteger::codeGen(CodeGenContext& context) {
  return ConstantInt::get(Type::getInt32Ty(context.getContext()), value, true);
}

Value* NString::codeGen(CodeGenContext& context) {
  return ConstantInt::get(Type::getInt8Ty(context.getContext()), value, true);
}

Value* NIdentifier::codeGen(CodeGenContext& context) {
  if (context.locals().find(name) == context.locals().end()) {
    std::cerr << "undeclared variable " << name << std::endl;
    return NULL;
  }

  LoadInst* ld =
      new LoadInst(context.locals()[name], "", false, context.currentBlock());
  ld->setAlignment(4);

  return ld;
}

Value* NIdentifier::store(Node* rhs, CodeGenContext& context) {
  StoreInst* st = new StoreInst(rhs->codeGen(context), context.locals()[name],
                                false, context.currentBlock());

#if 0
    // normal alignment in the future (no)
    // context.locals()[lhs->getName()];
#else
  st->setAlignment(4);
#endif

  return st;
}

Value* NAssignment::codeGen(CodeGenContext& context) {
  NIdentifier* lvalue = dynamic_cast<NIdentifier*>(&lhs);

  if (context.locals().find(lvalue->name) == context.locals().end()) {
    AllocaInst* alloc =
        new AllocaInst(typeOf(context, *lvalue), 0, lvalue->name.c_str(),
                       context.currentBlock());
#if 0
    // normal alignment in the future (no)
    if (this->type->getName() == "int")
      alloc->setAlignment(4);
    else if (this->type->getName() == "char") {
      alloc->setAlignment(1);
    }
#else
    alloc->setAlignment(4);
#endif
    context.locals()[lvalue->name] = alloc;
  }

  return lvalue->store(&rhs, context);
}

Value* NMath::codeGen(CodeGenContext& context) {
  Instruction::BinaryOps instr;

  Value* l = lhs.codeGen(context);
  Value* r = rhs.codeGen(context);

  IRBuilder<> builder(context.currentBlock());

  if (op == std::string("+"))
    return builder.CreateAdd(l, r, "tmp");
  else if (op == std::string("-"))
    return builder.CreateSub(l, r, "tmp");
  else if (op == std::string("*"))
    return builder.CreateMul(l, r, "tmp");
  else if (op == std::string("/"))
    return builder.CreateSDiv(l, r, "tmp");
}

Value* NIf::codeGen(CodeGenContext& context) {
  BasicBlock* then = BasicBlock::Create(context.getContext(), "",
                                        context.currentBlock()->getParent());

  BasicBlock* cond_false = BasicBlock::Create(
      context.getContext(), "", context.currentBlock()->getParent());

  IRBuilder<> builder(context.currentBlock());

  Value* comparison = cond.codeGen(context);
  Value* zero =
      ConstantInt::get(Type::getInt32Ty(context.getContext()), 0, true);
  Value* cellValIsZero = builder.CreateICmpNE(comparison, zero);

  builder.CreateCondBr(cellValIsZero, then, cond_false);

  std::map<std::string, Value*> currLocals = context.locals();

  context.pushBlock(then);  // start then
  context.setLocals(currLocals);

  block.codeGen(context);

  IRBuilder<> builder2(context.currentBlock());
  builder2.CreateBr(cond_false);

  context.popBlock();  // end then

  context.popBlock();

  context.pushBlock(cond_false);
  context.setLocals(currLocals);

  // if (else_block) {
  //   context.pushBlock(cond_false);
  //   else_block->codeGen(context);
  //   context.popBlock();
  // }
}

Value* NCond::codeGen(CodeGenContext& context) {
  Instruction::BinaryOps instr;

  Value* l = lhs.codeGen(context);
  Value* r = rhs.codeGen(context);

  IRBuilder<> builder(context.currentBlock());

  if (op == std::string("=="))
    return builder.CreateIntCast(builder.CreateICmpEQ(l, r, "tmp"),
                                 builder.getInt32Ty(), false, "");
  else if (op == std::string("!="))
    return builder.CreateIntCast(builder.CreateICmpNE(l, r, "tmp"),
                                 builder.getInt32Ty(), false, "");
  else if (op == std::string("<"))
    return builder.CreateIntCast(builder.CreateICmpSLT(l, r, "tmp"),
                                 builder.getInt32Ty(), false, "");
  else if (op == std::string("<="))
    return builder.CreateIntCast(builder.CreateICmpSLE(l, r, "tmp"),
                                 builder.getInt32Ty(), false, "");
  else if (op == std::string(">"))
    return builder.CreateIntCast(builder.CreateICmpSGT(l, r, "tmp"),
                                 builder.getInt32Ty(), false, "");
  else if (op == std::string(">="))
    return builder.CreateIntCast(builder.CreateICmpSGE(l, r, "tmp"),
                                 builder.getInt32Ty(), false, "");

  return NULL;
}

Value* NBlock::codeGen(CodeGenContext& context) {
  // NodeList::const_iterator it;
  Value* last = NULL;
  // for (it = statements.begin(); it != statements.end(); it++) {
  //   last = (**it).codeGen(context);
  // }
  for (Node* a : statements) {
    last = a->codeGen(context);
  }

  return last;
}

Value* NLoop::codeGen(CodeGenContext& context) {
  std::map<std::string, Value*> currLocals = context.locals();

  BasicBlock* loopHeader = BasicBlock::Create(
      context.getContext(), "header_loop", context.currentBlock()->getParent());

  BasicBlock* loopBody = BasicBlock::Create(
      context.getContext(), "loop_body", context.currentBlock()->getParent());

  BasicBlock* loopAfter = BasicBlock::Create(
      context.getContext(), "loop_after", context.currentBlock()->getParent());

  IRBuilder<> builder(context.currentBlock());

  builder.CreateBr(loopHeader);

  // loopHeader

  context.popBlock();  // конец базового ьлока, с которым
                       // мы залетели в функцию
  context.pushBlock(loopHeader);
  context.setLocals(currLocals);

  IRBuilder<> builderHeader(context.currentBlock());

  Value* comparison = cond.codeGen(context);
  Value* zero =
      ConstantInt::get(Type::getInt32Ty(context.getContext()), 0, true);
  Value* cellValIsZero = builderHeader.CreateICmpNE(comparison, zero);

  builderHeader.CreateCondBr(cellValIsZero, loopBody, loopAfter);

  // end loopHeader

  // loopBody

  context.pushBlock(loopBody);  // start then
  context.setLocals(currLocals);

  block.codeGen(context);

  IRBuilder<> builderBody(context.currentBlock());
  builderBody.CreateBr(loopHeader);

  context.popBlock();  // end then

  // end loopBody

  // loopAfter

  context.popBlock();

  context.pushBlock(loopAfter);  // продолжение базового блока,
  context.setLocals(currLocals);  // с которым мы залетели в функцию
}

Value* NCallArgs::codeGen(CodeGenContext& context) {
  // NodeList::const_iterator it;
  Value* last = NULL;
  // for (it = statements.begin(); it != statements.end(); it++) {
  //   last = (**it).codeGen(context);
  // }
  for (Node* a : call_args) {
    last = a->codeGen(context);
  }

  return last;
}

Value* NMethodCall::codeGen(CodeGenContext& context) {
  IRBuilder<> builder(context.currentBlock());

  Value* outInt = arguments.codeGen(context);

  std::vector<Type*> putsArgs;
  putsArgs.push_back(builder.getInt32Ty());

  ArrayRef<Type*> argsRef(putsArgs);

  FunctionType* putsType =
      FunctionType::get(builder.getInt32Ty(), argsRef, false);
  Constant* putsFunc =
      context.getModule()->getOrInsertFunction("print", putsType);

  builder.CreateCall(putsFunc, outInt);
}

Value* NReturn::codeGen(CodeGenContext& context) {
  ReturnInst::Create(context.getModule()->getContext(), ret->codeGen(context),
                     context.currentBlock());
}