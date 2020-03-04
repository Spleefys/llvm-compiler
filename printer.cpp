#include "printer.h"
#include <iostream>
#include <typeindex>
#include <typeinfo>
#include "node.h"

using namespace std;

void Printer::VisitInteger(NInteger &integer) {
  os << "Visit Integer " << integer.value;
}
void Printer::VisitIdentifier(NIdentifier &ident) {
  if (ident.name[0] == '$') {
    os << "Visit VarIdentifier " << ident.name.substr(1);
  } else {
    os << "Visit FunIdentifier " << ident.name;
  }
}
void Printer::VisitString(NString &str) { os << "Visit String " << str.value; }
void Printer::VisitBlock(NBlock &block) {
  os << "Visit Block\n";
  for (Node *stmt : block.statements) stmt->accept(*this);
  os << "End Of Block\n";
}
void Printer::VisitArray(NArray &arr) {
  os << "Visit Array\n";
  for (Node *a : arr.data) {
    a->accept(*this);
  }
}
void Printer::VisitArrid(NArrid &arrid) {
  os << "Visit Array ID:\n";
  arrid.id.accept(*this);
  arrid.idx.accept(*this);
}
void Printer::VisitVariableDeclaration(NVariableDeclaration &vd) {
  os << "Visit Variable Declaration\n";
}
void Printer::VisitMethodCall(NMethodCall &mcall) {
  os << "Visit MethodCall\n";
  mcall.id.accept(*this);
  os << " ( ";
  mcall.arguments.accept(*this);
  os << " )\n";
  os << "End Of MethodCall\n";
}
void Printer::VisitAssignment(NAssignment &assign) {
  os << "Visit Assignment (";
  assign.lhs.accept(*this);
  os << " = ";
  assign.rhs.accept(*this);
  os << ")"
     << "\n"
     << "End Of Assignment\n";
}
void Printer::VisitIf(NIf &ifstmt) {
  os << "Visit If Statement\n";
  ifstmt.cond.accept(*this);
  ifstmt.block.accept(*this);
  if (ifstmt.else_block != nullptr) {
    os << "Visit ElseStatement\n";
    ifstmt.else_block->accept(*this);
  }
  os << "End Of If Statement\n";
}
void Printer::VisitLoop(NLoop &loop) {
  os << "Visit Loop Statement\n";
  loop.cond.accept(*this);
  loop.block.accept(*this);
  os << "End Of Loop Statement\n";
}
void Printer::VisitCond(NCond &cond) {
  os << "Visit Condition (";
  cond.lhs.accept(*this);
  os << " " << cond.op << " ";
  cond.rhs.accept(*this);
  os << ") End of Condition\n";
}
void Printer::VisitMath(NMath &math) {
  os << "Visit Math (";
  math.lhs.accept(*this);
  os << " " << math.op << " ";
  math.rhs.accept(*this);
  os << ")"
     << " End Of Math";
}
void Printer::VisitCallArgs(NCallArgs &args) {
  os << "Visit Call Arguments: ( ";
  for (Node *args : args.call_args) {
    args->accept(*this);
    os << " ";
  }
  os << ")";
}
void Printer::VisitReturn(NReturn &ret) {
  os << "Visit Return Statement\n";
  if (ret.ret != nullptr) {
    ret.ret->accept(*this);
  }
  os << "\nEnd of Return\n";
}