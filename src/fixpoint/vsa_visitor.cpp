#include "vsa_visitor.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitBasicBlock(BasicBlock &BB) {
  /// create new empty BOTTOM state
  newState = State();
  DEBUG_OUTPUT("visitBasicBlock: entered " << BB.getName());

  /// bb has no predecessors: return empty state which is not bottom!
  /// and insert values s.t. arg -> T
  const int numPreds = std::distance(pred_begin(&BB), pred_end(&BB));
  if (numPreds == 0) {
    /// mark state such that it is not bottom any more
    newState.markVisited();

    for (auto &arg : BB.getParent()->args()) {
      /// put top for all arguments
      if (arg.getType()->isIntegerTy())
        newState.put(arg,
                     AD_TYPE::create_top(arg.getType()->getIntegerBitWidth()));
    }

    return;
  }

  /// least upper bound with all predecessors
  for (const auto pred : predecessors(&BB)) {
    DEBUG_OUTPUT("visitBasicBlock: pred " << pred->getName() << " found");
    const auto incoming = programPoints.find(pred);
    if (incoming != programPoints.end()) {
      if (incoming->second.isBottom()) /// case 1: lub(x, bottom) = x
        continue;
      /// else state is not bottom
      DEBUG_OUTPUT("visitBasicBlock: state for " << pred->getName()
                                                 << " found");

      /// apply condition and check if basic block is reachable from previous
      /// block under application of the condition
      if(bcs.applyCondition(pred, &BB))
          newState.leastUpperBound(incoming->second);
      // else : not reachable -> do not take lub
      bcs.unApplyCondition(pred);
    }
  }

  /// prune state with predominator (to get rid of mappings that are NOT
  /// guaranteed to be in the state)
  if(numPreds>1)
    newState.prune(programPoints[DT.getNode(&BB)->getIDom()->getBlock()]);

  /// visited and still bottom: something is wrong...
  /// none of the preceeding basic blocks has been visited!?
  assert(!newState.isBottom() &&
         "VsaVisitor::visitBasicBlock: newState is still bottom!");
}

void VsaVisitor::visitTerminatorInst(TerminatorInst &I) {
  DEBUG_OUTPUT("visitTerminationInst: entered");
  const auto currentBB = I.getParent();
  const auto oldState = programPoints.find(currentBB);
  if (oldState != programPoints.end()) {
    DEBUG_OUTPUT("visitTerminationInst: old state found");

    assert(!oldState->second.isBottom() && "Pruning with bottom!");

    /// From the merge of states, there are values in the map that are in
    /// reality only defined for certain paths.
    /// All values actually defined are also defined after the first pass.
    /// Therefore remove all values that were not defined in the previous state
    newState.prune(oldState->second);

    /// compute lub in place after this old state is updated
    if (newState.lessOrEqual(oldState->second)) {
      /// new state was old state: do not push successors
      DEBUG_OUTPUT("visitTerminationInst: state has not been changed");
      DEBUG_OUTPUT("visitTerminationInst: new state equals old state in "
                   << currentBB->getName());
      return;
    } /// else: state has changed
  }
  programPoints[currentBB] = newState;

  DEBUG_OUTPUT(
      "visitTerminationInst: state has been changed -> push successors");
  DEBUG_OUTPUT("visitTerminationInst: new state in bb "
               << currentBB->getName());

  /// push currently reachable successors
  pushSuccessors(I);
}

void VsaVisitor::visitBranchInst(BranchInst &I) {
  const auto cond = I.getOperand(0);

  DEBUG_OUTPUT("CONDITIONAL BRANCHES: TEST");
  if (ICmpInst::classof(cond)) {
    const auto cmpInst = reinterpret_cast<ICmpInst *>(cond);

    const auto op0 = cmpInst->getOperand(0);
    const auto ad0 = newState.getAbstractValue(op0);

    const auto op1 = cmpInst->getOperand(1);
    const auto ad1 = newState.getAbstractValue(op1);

    DEBUG_OUTPUT("CONDITIONAL BRANCHES: ");
    DEBUG_OUTPUT("     " << *ad0);
    DEBUG_OUTPUT("     " << *ad1);

    /// left argument (l)
    if (Instruction::classof(op0)) {
      /// perform comparison
      auto temp = ad0->icmp(cmpInst->getPredicate(),
                            cmpInst->getType()->getIntegerBitWidth(), *ad1);

      putBothBranchConditions(I, op0, temp);
    }

    /// right argument (r)
    if (Instruction::classof(op1)) {
      /// perform comparison with swapped predicate
      auto temp = ad1->icmp(cmpInst->getSwappedPredicate(),
                            cmpInst->getType()->getIntegerBitWidth(), *ad0);

      putBothBranchConditions(I, op1, temp);
    }
  }

  /// continue as if it were a simple terminator
  visitTerminatorInst(I);
}

void VsaVisitor::putBothBranchConditions(BranchInst& I, Value* op,
  std::pair<shared_ptr<AbstractDomain>, shared_ptr<AbstractDomain>> &valuePair) {
    DEBUG_OUTPUT("T-r: " << *valuePair.first);
    DEBUG_OUTPUT("F-r: " << *valuePair.second);

    /// true
    bcs.putBranchConditions(I.getParent(), I.getSuccessor(0), op,
                            valuePair.first);
    /// false
    bcs.putBranchConditions(I.getParent(), I.getSuccessor(1), op,
                            valuePair.second);
}

void VsaVisitor::visitSwitchInst(SwitchInst &I) {
  const auto cond = I.getCondition();

  /// this will be the values for default. We (potentially) shrink this with
  /// every case that we visit
  auto values = newState.getAbstractValue(cond);

  std::map<BasicBlock*, std::shared_ptr<AbstractDomain>> tempConditions;

  for (const auto &kase : I.cases()) {

    AD_TYPE kaseConst(kase.getCaseValue()->getValue());

    /// split values into those take compare == with the constant (none or one value) and those that
    /// do not (the rest)
    const auto kaseVals = values->icmp(CmpInst::Predicate::ICMP_EQ,
                          kase.getCaseValue()->getType()->getIntegerBitWidth(),
                          kaseConst);

    /// put branch condition in place
    /// since several cases might jump to the same BB, we need to build it first and set it in another loop
    auto successor = kase.getCaseSuccessor();
    if(tempConditions.find(successor) != tempConditions.end()) {
      // value present -> compute LUB
      tempConditions[successor] = tempConditions[successor]->leastUpperBound(*kaseVals.first);
    }
    else {
      // no value present -> put
      tempConditions[successor] = kaseVals.first;
    }

    /// use information to constrain default case
    values = kaseVals.second;
  }

  // put default condition
  auto defaultSuccessor = I.getDefaultDest();

  if(tempConditions.find(defaultSuccessor) != tempConditions.end()) {
      // value present -> compute LUB
    tempConditions[defaultSuccessor] = tempConditions[defaultSuccessor]->leastUpperBound(*values);
  }
  else {
    // no value present -> put
    tempConditions[defaultSuccessor] = values;
  }


  for (const auto &kase : I.cases()) {
    /// put branch condition in place
    bcs.putBranchConditions(I.getParent(), kase.getCaseSuccessor(), cond,
                            tempConditions[kase.getCaseSuccessor()]);
  }

  // put default condition
  bcs.putBranchConditions(I.getParent(), I.getDefaultDest(), cond, tempConditions[I.getDefaultDest()]);


  /// continue as if it were a simple terminator
  visitTerminatorInst(I);
}

void VsaVisitor::visitLoadInst(LoadInst &I) {
  // not strictly necessary (non-present vars are T ) but good for clearity
  newState.put(I, AD_TYPE::create_top(I.getType()->getIntegerBitWidth()));
}

void VsaVisitor::visitPHINode(PHINode &I) {
  /// bottom as initial value
  auto bs = AD_TYPE::create_bottom(I.getType()->getIntegerBitWidth());

  /// iterator for incoming blocks and values:
  /// we have to handle it seperatly since LLVM seems to not save them together
  auto blocks_iterator = I.block_begin();
  auto val_iterator = I.incoming_values().begin();

  // iterate together over incoming blocks and values
  for (; blocks_iterator != I.block_end(); blocks_iterator++, val_iterator++) {
    Value *val = val_iterator->get();

    /// create initial condition for lubs
    auto newValue = AD_TYPE::create_bottom(I.getType()->getIntegerBitWidth());

    /// Check if this is an instruction
    if (Instruction::classof(val)) {
      /// if the basic block where a value comes from is bottom,
      /// the corresponding alternative in the phi node is never taken
      /// we handle this here

      /// get incoming block
      const auto incomingBlock = *blocks_iterator;

      /// block has not been visited yet -> implicit bottom -> continue
      if (programPoints.find(incomingBlock) == programPoints.end())
        continue;

      /// explicit bottom -> continue
      if (programPoints[incomingBlock].isBottom())
        continue;

      /// apply the conditions that we have for reaching this basic block
      /// from the basic block containing the instruction
      bcs.applyCondition(incomingBlock, I.getParent());

      newValue = programPoints[incomingBlock].getAbstractValue(val);

      /// reset the condition cache
      bcs.unApplyCondition(incomingBlock);
    } else {
      /// val is not an instruction but a constant etc., so we do not need to
      /// go to its basic block but can get it directly
      newValue = newState.getAbstractValue(val);
    }

    /// if state of basic block was not bottom, include abstract value
    /// in appropriate block in LUB for phi
    bs = bs->leastUpperBound(*newValue);
  }

  /// this should not happen as we only put basicBlocks on the worklist if they
  /// are reachable
  assert(!bs->isBottom() && "VsaVisitor::visitPHINode: new value is bottom!");

  /// save new value into state
  newState.put(I, bs);
}

void VsaVisitor::visitAdd(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->add(I.getType()->getIntegerBitWidth(), *ad1,
                           I.hasNoUnsignedWrap(), I.hasNoSignedWrap()));
}

void VsaVisitor::visitSub(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->sub(I.getType()->getIntegerBitWidth(), *ad1,
                           I.hasNoUnsignedWrap(), I.hasNoSignedWrap()));
}

void VsaVisitor::visitMul(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->mul(I.getType()->getIntegerBitWidth(), *ad1,
                           I.hasNoUnsignedWrap(), I.hasNoSignedWrap()));
}

void VsaVisitor::visitURem(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->urem(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitSRem(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->srem(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitUDiv(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->udiv(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitSDiv(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->sdiv(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitAnd(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->and_(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitOr(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->or_(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitXor(BinaryOperator &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->xor_(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitShl(Instruction &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->shl(I.getType()->getIntegerBitWidth(), *ad1,
                           I.hasNoUnsignedWrap(), I.hasNoSignedWrap()));
}

void VsaVisitor::visitLShr(Instruction &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->lshr(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitAShr(Instruction &I) {
  auto ad0 = newState.getAbstractValue(I.getOperand(0));
  auto ad1 = newState.getAbstractValue(I.getOperand(1));

  newState.put(I, ad0->ashr(I.getType()->getIntegerBitWidth(), *ad1));
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I) {
  STD_OUTPUT("visited binary instruction");
}

void VsaVisitor::visitUnaryInstruction(UnaryInstruction &I) {
  /// interesting ones here would be the ext/trunc instructions, i.e.
  /// sext, zext, trunc
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

void VsaVisitor::print() const {
  for (const auto &pp : programPoints) {
    STD_OUTPUT("VsaVisitor::print():" << pp.first->getName());
    pp.second.print();
  }
}

std::map<BasicBlock *, State> &VsaVisitor::getProgramPoints() {
  return programPoints;
}
} // namespace pcpo
