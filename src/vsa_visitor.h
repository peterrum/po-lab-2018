#ifndef PROJECT_VISITOR_H
#define PROJECT_VISITOR_H

#include "AbstractDomain.h"
#include "BoundedSet.h"
#include "branch_conditions.h"
#include "state.h"
#include "util.h"
#include "worklist.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/InstrTypes.h"
#include <queue>
#include <unordered_map>

using namespace llvm;

namespace pcpo {

class VsaVisitor : public InstVisitor<VsaVisitor, void> {

public:
  VsaVisitor(WorkList &q) : worklist(q), newState(), bcs(programPoints){};

  /// create lub of states of preceeding basic blocks and use it as newState;
  /// the visitor automatically visits all instructions of this basic block
  void visitBasicBlock(BasicBlock &BB);

  /// visit TerminatorInstruction:
  /// compare lub(oldState, newState) with oldState of basic block; if state
  /// has changed: update state and push direct successors basic block into
  /// the working list
  void visitTerminatorInst(TerminatorInst &I);

  /// visit BranchInstruction:
  /// before calling visitTerminatorInst, it evaluates branch conditions
  void visitBranchInst(BranchInst &I);

  /// visit LoadInstruction:
  /// set variable explicitly top
  void visitLoadInst(LoadInst &I);

  /// visit PHINode:
  /// visitBasicBlock has already created lub of states of preceeding bbs
  /// here we only add the
  void visitPHINode(PHINode &I);

  /*void visitSwitchInst(SwitchInst &I);
  void visitIndirectBrInst(IndirectBrInst &I);
  void visitResumeInst(ResumeInst &I);
  void visitICmpInst(ICmpInst &I);
  void visitAtomicCmpXchgInst(AtomicCmpXchgInst &I);
  void visitTruncInst(TruncInst &I);
  void visitZExtInst(ZExtInst &I);
  void visitSExtInst(SExtInst &I);
  void visitBitCastInst(BitCastInst &I);
  void visitSelectInst(SelectInst &I);
  void visitVAArgInst(VAArgInst &I);
  void visitExtractElementInst(ExtractElementInst &I);
  void visitExtractValueInst(ExtractValueInst &I);*/

  /// Call and Invoke
  // void visitCallInst(CallInst &I);
  // void visitInvokeInst(InvokeInst &I);

  /// BinaryOperators
  void visitAdd(BinaryOperator &I);
  void visitMul(BinaryOperator &I);

  /// if not specific get the whole class
  // void visitCastInst(CastInst &I);
  void visitBinaryOperator(BinaryOperator &I);
  // void visitCmpInst(CmpInst &I);
  void visitUnaryInstruction(UnaryInstruction &I);

  /// default
  void visitInstruction(Instruction &I);

  /// print state of all basic blocks
  void print();

private:
  /// push directly reachable basic blocks onto worklist
  void pushSuccessors(TerminatorInst &I);

  WorkList &worklist;
  State newState;
  std::map<BasicBlock *, State> programPoints;
  BranchConditions bcs;
};
}

#endif // PROJECT_VISITOR_H
