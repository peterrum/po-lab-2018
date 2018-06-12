#include "vsa_visitor.h"
#include "llvm/Support/raw_os_ostream.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitBasicBlock(BasicBlock &BB) {
  /// create new empty BOTTOM state
  newState = State();
  DEBUG_OUTPUT("visitBasicBlock: entered " << BB.getName());

  /// bb has no predecessors: return empty state which is not bottom!
  /// and insert values s.t. arg -> T
  int numPreds = std::distance(pred_begin(&BB), pred_end(&BB));
  if (numPreds == 0) {
    // mark state such that it is not bottom any more
    newState.markVisited();

    for (auto &arg : BB.getParent()->args()) {
      // put top for all arguments
      if(arg.getType()->isIntegerTy())
        newState.put(arg, AD_TYPE::create_top(arg.getType()->getIntegerBitWidth()));
    }

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

      bcs.applyCondition(pred, &BB);
      newState.leastUpperBound(incoming->second);
      bcs.unApplyCondition(pred);
    }
  }

  /// prune state with predominator
  auto predomState = programPoints[DT->getNode(&BB)->getIDom()->getBlock()];
  newState.prune(predomState);

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
    } // else: state has changed
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
      bcs.putBranchConditions(I.getParent(), I.getSuccessor(0), op0,
                              temp.first);
      /// false
      bcs.putBranchConditions(I.getParent(), I.getSuccessor(1), op0,
                              temp.second);
    }

    /// right argument (r)
    if (Instruction::classof(op1)) {
      /// perform comparison with inverted predicate
      auto temp = ad1->icmp(cmpInst->getInversePredicate(),
                            cmpInst->getType()->getIntegerBitWidth(), *ad0);

      DEBUG_OUTPUT("T-r: " << *temp.first);
      DEBUG_OUTPUT("F-r: " << *temp.second);

      /// true
      bcs.putBranchConditions(I.getParent(), I.getSuccessor(0), op1,
                              temp.first);
      /// false
      bcs.putBranchConditions(I.getParent(), I.getSuccessor(1), op1,
                              temp.second);
    }
  }

  /// continue as it were a simple terminator
  this->visitTerminatorInst(I);
}

void VsaVisitor::visitSwitchInst(SwitchInst &I) {
  auto cond = I.getCondition();
  auto values = newState.getAbstractValue(cond);

  for (auto &kase : I.cases()) {
    bcs.putBranchConditions(I.getParent(), kase.getCaseSuccessor(), cond,
                            shared_ptr<AbstractDomain>(
                                new AD_TYPE(kase.getCaseValue()->getValue())));
  }

  bcs.putBranchConditions(I.getParent(), I.getDefaultDest(), cond, values);

  /// continue as it were a simple terminator
  this->visitTerminatorInst(I);
}

void VsaVisitor::visitLoadInst(LoadInst &I) {
  // not strictly necessary (non-present vars are T ) but good for clearity
  newState.put(I, AD_TYPE::create_top(I.getType()->getIntegerBitWidth()));
}

void VsaVisitor::visitPHINode(PHINode &I) {
  /// bottom as initial value
  auto bs = AD_TYPE::create_bottom(I.getType()->getIntegerBitWidth());

  /// iterator for incoming blocks and values:
  /// we have to handle it seperatly since LLVM seems to save them not togehter
  auto blocks_iterator = I.block_begin();
  auto val_iterator = I.incoming_values().begin();

  // iterate together over incoming blocks and values
  for (; blocks_iterator != I.block_end(); blocks_iterator++, val_iterator++) {

    /// if the basic block where a value comes from is bottom,
    /// the corresponding alternative in the phi node is never taken
    /// the next 20 lines handle all the cases for that
    Value *val = val_iterator->get();

    /// create initial condition for lubs
    auto newValue = AD_TYPE::create_bottom(I.getType()->getIntegerBitWidth());

    /// Check if basic block containing use is bottom
    if (Instruction::classof(val)) {
      /// get incoming block
      auto incomingBlock = *blocks_iterator;

      /// block has not been visited yet -> implicit bottom=
      if (programPoints.find(incomingBlock) == programPoints.end())
        continue;

      /// explicit bottom
      if (programPoints[incomingBlock].isBottom())
        continue;

      bcs.applyCondition(incomingBlock, I.getParent());
      newValue = programPoints[incomingBlock].getAbstractValue(val);
      bcs.unApplyCondition(incomingBlock);
    } else {
      newValue = newState.getAbstractValue(val);
    }

    /// if state of basic block was not bottom, include abstract value
    /// in appropriate block in lub for phi
    bs = bs->leastUpperBound(*newValue);
  }

  assert(!bs->isBottom() &&
         "VsaVisitor::visitPHINode: new value is bottom!");

  /// save new value into state
  newState.put(I, bs);
}

void VsaVisitor::visitAdd(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->add(I.getType()->getIntegerBitWidth(), *ad1,
               I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}

void VsaVisitor::visitSub(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->sub(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}

void VsaVisitor::visitMul(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->mul(I.getType()->getIntegerBitWidth(), *ad1,
               I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}

void VsaVisitor::visitURem(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->urem(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitSRem(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->srem(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitUDiv(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->udiv(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitSDiv(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->sdiv(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitAnd(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->and_(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitOr(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->or_(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitXor(BinaryOperator &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->xor_(I.getType()->getIntegerBitWidth(), *ad1,
                        I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}

void VsaVisitor::visitShl (Instruction &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->shl(I.getType()->getIntegerBitWidth(), *ad1,
                         I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitLShr(Instruction &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->lshr(I.getType()->getIntegerBitWidth(), *ad1,
                         I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}
void VsaVisitor::visitAShr(Instruction &I){
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I,
               ad0->ashr(I.getType()->getIntegerBitWidth(), *ad1,
                         I.hasNoSignedWrap(), I.hasNoUnsignedWrap()));
}


void VsaVisitor::visitBinaryOperator(BinaryOperator &I) {
  STD_OUTPUT("visited binary instruction");
}

void VsaVisitor::visitUnaryInstruction(UnaryInstruction &I) {
  // todo
}

void VsaVisitor::visitInstruction(Instruction &I) {
  DEBUG_OUTPUT("visitInstruction: " << I.getOpcodeName());
}

void VsaVisitor::pushSuccessors(TerminatorInst &I) {
  // put all currently reachable successors into the worklist
  for (auto bb : I.successors()) {
    if (!bcs.isBasicBlockReachable(I.getParent(), bb))
      continue; // do not put it on the worklist now
    DEBUG_OUTPUT("\t-" << bb->getName());
    worklist.push(bb);
  }
}

void VsaVisitor::print() {
  for (auto &pp : programPoints) {
    STD_OUTPUT("VsaVisitor::print():" << pp.first->getName());
    pp.second.print();
  }
}
}
