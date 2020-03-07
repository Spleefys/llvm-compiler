#ifndef CODEGEN_H
#define CODEGEN_H

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <signal.h>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include "node.h"

using namespace llvm;

class CodeGenBlock {
 private:
  BasicBlock *block;
  std::map<std::string, Value *> locals;
  std::map<std::string, std::string> locals_type;

 public:
  CodeGenBlock() {}
  void setBlock(BasicBlock *block) { this->block = block; }
  void setLocals(std::map<std::string, Value *> &locals) {
    this->locals = locals;
  }
  void setLocalsType(std::map<std::string, std::string> &locals_type) {
    this->locals_type = locals_type;
  }
  BasicBlock *getBlock() { return block; }
  std::map<std::string, Value *> &getLocals() { return locals; }
  std::map<std::string, std::string> &getLocalsType() { return locals_type; }
  ~CodeGenBlock(){};
};

class CodeGenContext {
 private:
  Module *module;
  LLVMContext llvmContext;
  std::stack<CodeGenBlock *> blocks;
  Function *mainFunction;

 public:
  CodeGenContext(/* args */) {
    module = new Module("PHP Compiler", llvmContext);
  }

  Module *getModule() { return module; }

  LLVMContext &getContext() { return llvmContext; }

  std::map<std::string, Value *> &locals() { return blocks.top()->getLocals(); }
  std::map<std::string, std::string> &locals_type() {
    return blocks.top()->getLocalsType();
  }

  void setLocals(std::map<std::string, Value *> &locals) {
    blocks.top()->setLocals(locals);
  }

  void setLocalsType(std::map<std::string, std::string> &locals_type) {
    blocks.top()->setLocalsType(locals_type);
  }

  BasicBlock *currentBlock() { return blocks.top()->getBlock(); }

  void pushBlock(BasicBlock *block) {
    blocks.push(new CodeGenBlock());
    blocks.top()->setBlock(block);
  }

  void popBlock() {
    CodeGenBlock *top = blocks.top();
    blocks.pop();
    delete top;
  }

  static void sighandler(int signo) { exit(0); }

  void generateCode(NBlock *root) {
    /* Create the top level interpreter function to call as entry */
    std::vector<Type *> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(llvmContext),
                                            makeArrayRef(argTypes), false);
    mainFunction =
        Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock =
        BasicBlock::Create(llvmContext, "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root->codeGen(*this); /* emit bytecode for the toplevel block */
    ReturnInst::Create(llvmContext, bblock);
    popBlock();

    module->print(errs(), nullptr);
  }

  GenericValue runCode() {
    signal(SIGSEGV, sighandler);
    ExecutionEngine *ee =
        EngineBuilder(std::unique_ptr<Module>(module)).create();
    ee->finalizeObject();
    std::vector<GenericValue> noargs;
    GenericValue v = ee->runFunction(mainFunction, noargs);
    return v;
  }

  ~CodeGenContext() {}
};

#endif