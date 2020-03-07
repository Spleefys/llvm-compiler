#ifndef PRINTER_H
#define PRINTER_H

#include <sstream>
#include "visitor.h"

class Printer : Visitor {
  std::ostream &os;

 public:
  Printer(std::ostream &ostream) : os(ostream) {}
  void VisitInteger(NInteger &integer);
  void VisitIdentifier(NIdentifier &ident);
  void VisitString(NString &str);
  void VisitArray(NArray &arr);
  void VisitArrid(NArrid &arrid);
  void VisitBlock(NBlock &block);
  void VisitVariableDeclaration(NVariableDeclaration &vd);
  void VisitMethodCall(NMethodCall &mcall);
  void VisitAssignment(NAssignment &assign);
  void VisitIf(NIf &ifstmt);
  void VisitLoop(NLoop &loop);
  void VisitCond(NCond &cond);
  void VisitMath(NMath &math);
  void VisitReturn(NReturn &ret);
};

#endif