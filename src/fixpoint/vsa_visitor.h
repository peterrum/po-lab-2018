#ifndef PROJECT_VISITOR_H
#define PROJECT_VISITOR_H

#include "../abstract_domain/AbstractDomain.h"
#include "../util/util.h"
#include "branch_conditions.h"
#include "state.h"
#include "worklist.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Operator.h"
#include <queue>
#include <unordered_map>

using namespace llvm;

namespace pcpo {

class VsaVisitor : public InstVisitor<VsaVisitor, void> {

public:
  VsaVisitor(WorkList &q, Function &function)
      : worklist(q), newState(), bcs(programPoints) {
    dt.runOnFunction(function);
    DT = &dt.getDomTree();

    for (auto &bb : function)
      if (DT->getNode(&bb)->getLevel() > 0) {
        DEBUG_OUTPUT(bb.getName()
                     << "  "
                     << DT->getNode(&bb)->getIDom()->getBlock()->getName()
                     << "\n");
      }
  };

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

  /// visit SwitchInstruction:
  /// before calling visitTerminatorInst, it evaluates branch conditions
  void visitSwitchInst(SwitchInst &I);

  /// visit LoadInstruction:
  /// set variable explicitly top
  void visitLoadInst(LoadInst &I);

  /// visit PHINode:
  /// visitBasicBlock has already created lub of states of preceeding bbs
  /// here we only add the
  void visitPHINode(PHINode &I);

  /*void visitIndirectBrInst(IndirectBrInst &I);
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
  void visitSub(BinaryOperator &I);
  void visitMul(BinaryOperator &I);
  void visitURem(BinaryOperator &I);
  void visitSRem(BinaryOperator &I);
  void visitUDiv(BinaryOperator &I);
  void visitSDiv(BinaryOperator &I);
  void visitAnd(BinaryOperator &I);
  void visitOr(BinaryOperator &I);
  void visitXor(BinaryOperator &I);

  void visitShl(Instruction &I);
  void visitLShr(Instruction &I);
  void visitAShr(Instruction &I);

  /// if not specific get the whole class
  // void visitCastInst(CastInst &I);
  void visitBinaryOperator(BinaryOperator &I);
  // void visitCmpInst(CmpInst &I);
  void visitUnaryInstruction(UnaryInstruction &I);

  /// default
  void visitInstruction(Instruction &I);

  /// print state of all basic blocks
  void print();

  /// return the program points
  std::map<BasicBlock *, State> &getProgramPoints();

private:
  /// push directly reachable basic blocks onto worklist
  void pushSuccessors(TerminatorInst &I);

  WorkList &worklist;
  mutable DominatorTree *DT = nullptr;
  DominatorTreeWrapperPass dt;
  State newState;
  std::map<BasicBlock *, State> programPoints;
  BranchConditions bcs;
};
}

#endif // PROJECT_VISITOR_H
