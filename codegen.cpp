#include "codegen.h"
#include "node.h"

Value* NInteger::codeGen(CodeGenContext& context) {
  return ConstantInt::get(Type::getInt32Ty(context.getContext()), value, true);
}

Value* NString::codeGen(CodeGenContext& context) {
  return ConstantInt::get(Type::getInt8Ty(context.getContext()), value[1],
                          true);
}

Value* NArray::codeGen(CodeGenContext& context) {}

Value* NArray::getElementPtrInst(CodeGenContext& context,
                                 const std::string& identifier, int i) {
  std::vector<llvm::Value*> vect;

  vect.push_back(ConstantInt::get(Type::getInt32Ty(context.getContext()), 0));
  vect.push_back(ConstantInt::get(Type::getInt32Ty(context.getContext()), i));

  GetElementPtrInst* elemPtr = GetElementPtrInst::CreateInBounds(
      context.locals()[identifier], vect, "", context.currentBlock());

  return elemPtr;
}

Value* NArray::store(Node* rhs, const std::string& identifier,
                     CodeGenContext& context) {
  NArray* vec = dynamic_cast<NArray*>(rhs);

  StoreInst* st;
  int i = 0;
  for (Node* a : vec->data) {
    st = new StoreInst(a->codeGen(context),
                       getElementPtrInst(context, identifier, i), false,
                       context.currentBlock());
    st->setAlignment(4);
    i++;
  }

  return st;
}

Value* NArrid::codeGen(CodeGenContext& context) {
  NIdentifier* identifier = dynamic_cast<NIdentifier*>(&id);
  LoadInst* ld = new LoadInst(getElementPtrInst(context, identifier->name), "",
                              false, context.currentBlock());
  ld->setAlignment(4);

  return ld;
}

Value* NArrid::getElementPtrInst(CodeGenContext& context,
                                 const std::string& identifier) {
  std::vector<llvm::Value*> vect;

  vect.push_back(ConstantInt::get(Type::getInt32Ty(context.getContext()), 0));
  vect.push_back(idx.codeGen(context));

  GetElementPtrInst* elemPtr = GetElementPtrInst::CreateInBounds(
      context.locals()[identifier], vect, "", context.currentBlock());

  return elemPtr;
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
  st->setAlignment(4);
  return st;
}

Value* NAssignment::codeGen(CodeGenContext& context) {
  NIdentifier* lvalue = dynamic_cast<NIdentifier*>(&lhs);

  if (context.locals().find(lvalue->name) == context.locals().end()) {
    std::string typee = typeid(rhs).name();
    typee = typee.substr(typee.find('N') + 1);
    AllocaInst* alloc;

    if (strcmp(typee.c_str(), "Integer") == 0 ||
        strcmp(typee.c_str(), "Arrid") == 0 ||
        strcmp(typee.c_str(), "Math") == 0) {
      alloc = new AllocaInst(Type::getInt32Ty(context.getContext()), 0,
                             lvalue->name.c_str(), context.currentBlock());

      alloc->setAlignment(4);
      context.locals()[lvalue->name] = alloc;
      context.locals_type()[lvalue->name] = "Integer";

    } else if (strcmp(typee.c_str(), "String") == 0) {
      alloc = new AllocaInst(IntegerType::get(context.getContext(), 8), 0,
                             lvalue->name.c_str(), context.currentBlock());
      context.locals()[lvalue->name] = alloc;
      context.locals_type()[lvalue->name] = "String";

    } else if (strcmp(typee.c_str(), "Identifier") == 0) {
      NIdentifier* rvalue = dynamic_cast<NIdentifier*>(&rhs);
      AllocaInst* alloc;
      std::string ty = context.locals_type()[rvalue->name];

      if (strcmp(ty.c_str(), "Integer") == 0) {
        alloc = new AllocaInst(Type::getInt32Ty(context.getContext()), 0,
                               lvalue->name.c_str(), context.currentBlock());
        alloc->setAlignment(4);

      } else if (strcmp(ty.c_str(), "String") == 0) {
        alloc = new AllocaInst(IntegerType::get(context.getContext(), 8), 0,
                               lvalue->name.c_str(), context.currentBlock());
      }
      context.locals()[lvalue->name] = alloc;
      context.locals_type()[lvalue->name] = ty;
    } else if (strcmp(typee.c_str(), "Array") == 0) {
      NArray* vec = dynamic_cast<NArray*>(&rhs);

      ArrayType* arrayType = ArrayType::get(
          Type::getInt32Ty(context.getContext()), vec->data.size());

      alloc = new AllocaInst(arrayType, 0, (lvalue->name + ".addr").c_str(),
                             context.currentBlock());

      context.locals()[lvalue->name] = alloc;
      context.locals_type()[lvalue->name] = "AInteger";

      alloc->setAlignment(4);

      return vec->store(&rhs, lvalue->name, context);
    }
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
  Value* last = NULL;
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

Value* NMethodCall::codeGen(CodeGenContext& context) {
  NIdentifier* ident = dynamic_cast<NIdentifier*>(&id);
  std::string meth_name = ident->name;
  if (strcmp(meth_name.c_str(), "print") == 0) {
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
  } else if (strcmp(meth_name.c_str(), "echoc") == 0) {
    IRBuilder<> builder(context.currentBlock());

    Value* outInt = arguments.codeGen(context);

    std::vector<Type*> putsArgs;
    putsArgs.push_back(builder.getInt8Ty());

    ArrayRef<Type*> argsRef(putsArgs);

    FunctionType* putsType =
        FunctionType::get(builder.getInt32Ty(), argsRef, false);
    Constant* putsFunc =
        context.getModule()->getOrInsertFunction("echoc", putsType);

    builder.CreateCall(putsFunc, outInt);

  } else if (strcmp(meth_name.c_str(), "echo") == 0) {
    NString* message = dynamic_cast<NString*>(&arguments);
    IRBuilder<> builder(context.currentBlock());
    Value* outString = builder.CreateGlobalStringPtr(message->value);
    std::vector<Type*> putsArgs;
    putsArgs.push_back(builder.getInt8Ty()->getPointerTo());
    ArrayRef<Type*> argsRef(putsArgs);
    FunctionType* putsType =
        FunctionType::get(builder.getInt32Ty(), argsRef, false);
    Constant* putsFunc =
        context.getModule()->getOrInsertFunction("echo", putsType);
    builder.CreateCall(putsFunc, outString);
  }
}

Value* NReturn::codeGen(CodeGenContext& context) {
  ReturnInst::Create(context.getModule()->getContext(), ret->codeGen(context),
                     context.currentBlock());
}