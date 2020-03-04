#ifndef NODE_H
#define NODE_H

#include <llvm/IR/Value.h>
#include <iostream>
#include <vector>
#include "visitor.h"

typedef std::vector<Node *> NodeList;
class CodeGenContext;

class Node {
 public:
  virtual ~Node() {}
  virtual void accept(Visitor &v) = 0;
  virtual llvm::Value *codeGen(CodeGenContext &context) { return NULL; };
};

class NInteger : public Node {
 public:
  long long value;
  NInteger(long long value) : value(value) {}
  void accept(Visitor &v) override { v.VisitInteger(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NIdentifier : public Node {
 public:
  std::string name;
  NIdentifier(const std::string &name) : name(name) {}
  void accept(Visitor &v) override { v.VisitIdentifier(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
  llvm::Value *store(Node *rhs, CodeGenContext &context);
};

class NString : public Node {
 public:
  std::string value;
  NString(std::string value) : value(value) {}
  void accept(Visitor &visitor) override { visitor.VisitString(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NArray : public Node {
 public:
  NodeList data;
  NArray() {}
  void accept(Visitor &visitor) override { visitor.VisitArray(*this); }
  // virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NArrid : public Node {
 public:
  Node &id;
  Node &idx;
  NArrid(Node &id, Node &idx) : id(id), idx(idx) {}
  void accept(Visitor &visitor) override { visitor.VisitArrid(*this); }
  // virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NMethodCall : public Node {
 public:
  Node &id;
  Node &arguments;
  NMethodCall(Node &id, Node &arguments) : id(id), arguments(arguments) {}
  void accept(Visitor &v) override { v.VisitMethodCall(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NAssignment : public Node {
 public:
  Node &lhs;
  Node &rhs;
  NAssignment(Node &lhs, Node &rhs) : lhs(lhs), rhs(rhs) {}
  void accept(Visitor &v) override { v.VisitAssignment(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NMath : public Node {
 public:
  Node &lhs;
  Node &rhs;
  std::string op;
  NMath(Node &lhs, Node &rhs, const std::string &op)
      : lhs(lhs), rhs(rhs), op(op) {}
  void accept(Visitor &v) override { v.VisitMath(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NBlock : public Node {
 public:
  NodeList statements;
  NBlock() {}
  void accept(Visitor &v) override { v.VisitBlock(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NIf : public Node {
 public:
  Node &cond;
  Node &block;
  Node *else_block;

  NIf(Node &cond, Node &block, Node *else_block = nullptr)
      : cond(cond), block(block), else_block(else_block) {}
  void accept(Visitor &v) override { v.VisitIf(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NLoop : public Node {
 public:
  Node &cond;
  Node &block;
  NLoop(Node &cond, Node &block) : cond(cond), block(block) {}
  void accept(Visitor &v) override { v.VisitLoop(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NCond : public Node {
 public:
  Node &lhs;
  Node &rhs;
  std::string op;

  NCond(Node &lhs, Node &rhs, const std::string &op)
      : lhs(lhs), rhs(rhs), op(op) {}

  void accept(Visitor &v) override { v.VisitCond(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NCallArgs : public Node {
 public:
  NodeList call_args;
  NCallArgs() {}
  void accept(Visitor &v) override { v.VisitCallArgs(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

class NReturn : public Node {
 public:
  Node *ret;
  NReturn(Node *ret = nullptr) : ret(ret) {}
  void accept(Visitor &v) override { v.VisitReturn(*this); }
  virtual llvm::Value *codeGen(CodeGenContext &context);
};

#endif