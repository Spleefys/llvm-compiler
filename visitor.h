#ifndef VISITOR_H
#define VISITOR_H

class Node;
class NExpression;
class NStatement;
class NInteger;
class NIdentifier;
class NArray;
class NArrid;
class NString;
class NBlock;
class NVariableDeclaration;
class NMethodCall;
class NAssignment;
class NIf;
class NLoop;
class NCond;
class NMath;
class NReturn;

class Visitor {
 public:
  virtual void VisitInteger(NInteger &integer) = 0;
  virtual void VisitIdentifier(NIdentifier &ident) = 0;
  virtual void VisitString(NString &str) = 0;
  virtual void VisitArray(NArray &arr) = 0;
  virtual void VisitArrid(NArrid &arrid) = 0;
  virtual void VisitBlock(NBlock &block) = 0;
  virtual void VisitVariableDeclaration(NVariableDeclaration &vd) = 0;
  virtual void VisitMethodCall(NMethodCall &mcall) = 0;
  virtual void VisitAssignment(NAssignment &assign) = 0;
  virtual void VisitIf(NIf &ifstmt) = 0;
  virtual void VisitLoop(NLoop &loop) = 0;
  virtual void VisitCond(NCond &cond) = 0;
  virtual void VisitMath(NMath &math) = 0;
  virtual void VisitReturn(NReturn &ret) = 0;
};

#endif
