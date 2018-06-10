#include "vsa_visitor.h"
#include "llvm/Support/raw_os_ostream.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitBasicBlock(BasicBlock &BB) {
  /// create new empty BOTTOM state
  newState = State();
  DEBUG_OUTPUT("visitBasicBlock: entered " << BB.getName());

  /// bb has no predecessors: return empty state which is not bottom!
  if (pred_size(&BB) == 0) {
    // mark state such that it is not bottom any more
    newState.setNotBottom();
    return;
  }

  /// least upper bound with all predecessors
  for (auto pred : predecessors(&BB)) {
    DEBUG_OUTPUT("visitBasicBlock: pred " << pred->getName() << " found");
    auto incoming = programPoints.find(pred);
    if (incoming != programPoints.end()) {
      if (incoming->second.isBottom()) // case 1: lub(x, bottom) = x
        continue;
      // else state is not bottom
      DEBUG_OUTPUT("visitBasicBlock: state for " << pred->getName()
                                                 << " found");

      incoming->second.applyCondition(&BB);
      newState.leastUpperBound(incoming->second);
      incoming->second.unApplyCondition();
    }
  }

  /// visited and still bottom: something is wrong...
  /// none of the preceeding basic blocks has been visited!?
  assert(!newState.isBottom() &&
         "VsaVisitor::visitBasicBlock: newState is still bottom!");
}

void VsaVisitor::visitTerminatorInst(TerminatorInst &I) {
  DEBUG_OUTPUT("visitTerminationInst: entered");
  auto currentBB = I.getParent();
  auto oldState = programPoints.find(currentBB);
  if (oldState != programPoints.end()) {
    DEBUG_OUTPUT("visitTerminationInst: old state found");

    assert(!oldState->second.isBottom() && "Pruning with bottom!");

    // From the merge of states, there are values in the map that are in
    // reality only defined for certain paths.
    // All values actually defined are also defined after the first pass.
    // Therefore remove all values that were not defined in the previous state
    newState.prune(oldState->second);

    /// compute lub in place after this old state is updated
    if (!oldState->second.leastUpperBound(newState)) {
      /// new state was old state: do not push successors
      DEBUG_OUTPUT("visitTerminationInst: state has not been changed");
      DEBUG_OUTPUT("visitTerminationInst: new state equals old state in "
                   << currentBB->getName());
      return;
    }
    /// copy stuff regarding branch conditions (TODO: remove)
    oldState->second.transferBranchConditions(newState);
    oldState->second.transferBottomness(newState);
  } else {
    DEBUG_OUTPUT("visitTerminationInst: old state not found");
    programPoints[currentBB] = newState;
  }

  DEBUG_OUTPUT(
      "visitTerminationInst: state has been changed -> push successors");
  DEBUG_OUTPUT("visitTerminationInst: new state in bb "
               << currentBB->getName());

  /// push currently reachable successors
  pushSuccessors(I);
}

void VsaVisitor::visitBranchInst(BranchInst &I) {
  auto cond = I.getOperand(0);

  DEBUG_OUTPUT("CONDITIONAL BRANCHES: TEST");
  if (ICmpInst::classof(cond)) {
    auto cmpInst = reinterpret_cast<ICmpInst *>(cond);
    auto op0 = cmpInst->getOperand(0);
    auto op1 = cmpInst->getOperand(1);
    auto ad0 = newState.getAbstractValue(op0);
    auto ad1 = newState.getAbstractValue(op1);

    DEBUG_OUTPUT("CONDITIONAL BRANCHES: ");
    DEBUG_OUTPUT("     " << *ad0);
    DEBUG_OUTPUT("     " << *ad1);
    
    /// left argument (l)
    if (Instruction::classof(op0)) {
      /// perform comparison
      auto temp = ad0->icmp(cmpInst->getPredicate(),
                            cmpInst->getType()->getIntegerBitWidth(), *ad1);

      DEBUG_OUTPUT("T-l: " << *temp.first);
      DEBUG_OUTPUT("F-l: " << *temp.second);

      /// true
      newState.putBranchConditions(I.getSuccessor(0), op0, temp.first);
      /// false
      newState.putBranchConditions(I.getSuccessor(1), op0, temp.second);
    }

    /// right argument (r)
    if (Instruction::classof(op1)) {
      /// perform comparison with inverted predicate
      auto temp = ad1->icmp(cmpInst->getInversePredicate(),
                            cmpInst->getType()->getIntegerBitWidth(), *ad0);

      DEBUG_OUTPUT("T-r: " << *temp.first);
      DEBUG_OUTPUT("F-r: " << *temp.second);

      /// true
      newState.putBranchConditions(I.getSuccessor(0), op1, temp.first);
      /// false
      newState.putBranchConditions(I.getSuccessor(1), op1, temp.second);
    }
  }

  /// continue as it were a simple terminator
  this->visitTerminatorInst(I);
}

void VsaVisitor::visitLoadInst(LoadInst &I) {
  // not strictly necessary (non-present vars are T ) but good for clearity
  newState.put(I, AD_TYPE::create_top());
}

void VsaVisitor::visitPHINode(PHINode &I) {
  /// bottom
  auto bs = AD_TYPE::create_bottom();
  for (Use &val : I.incoming_values()) {
    // newState.getAbstractValue(val)->printOut();

    /// if the basic block where a value comes from is bottom,
    /// the corresponding alternative in the phi node is never taken
    /// the next 20 lines handle all the cases for that

    /// Check if basic block containing use is bottom
    if (Instruction::classof(val)) {
      auto incomingBlock = reinterpret_cast<Instruction *>(&val)->getParent();

      /// block has not been visited yet -> implicit bottom
      if (programPoints.find(incomingBlock) == programPoints.end())
        continue;

      /// explicit bottom
      if (programPoints[incomingBlock].isBottom())
        continue;
    }

    /// if state of basic block was not bottom, include abstract value
    /// in appropriate block in lub for phi
    bs = bs->leastUpperBound(*newState.getAbstractValue(val));
  }

  // bs->printOut();

  newState.put(I, bs);
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I) {
  STD_OUTPUT("visited binary instruction");
}

void VsaVisitor::visitAdd(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  // TODO: meaning of arguments?
  newState.put(I,
               ad0->add(I.getType()->getIntegerBitWidth(), *ad1, false, false));
}

void VsaVisitor::visitMul(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  // TODO: meaning of arguments?
  newState.put(I,
               ad0->mul(I.getType()->getIntegerBitWidth(), *ad1, false, false));
}

void VsaVisitor::visitUnaryInstruction(UnaryInstruction &I) {
  // todo
}

void VsaVisitor::visitInstruction(Instruction &I) {
  STD_OUTPUT("visitInstruction: " << I.getOpcodeName());
}

void VsaVisitor::pushSuccessors(TerminatorInst &I) {
  // put all currently reachable successors into the worklist
  for (auto bb : I.successors()) {
    if (!newState.isBasicBlockReachable(bb))
      continue; // do not put it on the worklist now
    DEBUG_OUTPUT("\t-" << bb->getName());
    worklist.push(bb);
  }
}

void VsaVisitor::print() {
  for (auto &pp : programPoints) {
    DEBUG_OUTPUT("VsaVisitor::print():" << pp.first->getName());
    pp.second.print();
  }
}
}
