#include "vsa_visitor.h"
#include "llvm/Support/raw_os_ostream.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitBasicBlock(BasicBlock &BB){
    /// empty state represents bottom
    newState = State();
    newState.setNotBottom();
    DEBUG_OUTPUT("visitBasicBlock: entered");
    /// least upper bound with all predecessors
    for(auto pred : predecessors(&BB)){
        DEBUG_OUTPUT("visitBasicBlock: pred" << pred->getName() << " found");
        auto incoming = programPoints.find(pred);
        if(incoming != programPoints.end()){
            if(incoming->second.isBottom()) continue;
            // else state is not bottom
            DEBUG_OUTPUT("visitBasicBlock: state for" << pred->getName() << " found");
            
            incoming->second.applyCondition(&BB);
            newState.leastUpperBound(incoming->second);
            incoming->second.unApplyCondition();
        } /// else: its state is implicit bottom and lub(bottom, x) = x
    }
}

void VsaVisitor::visitTerminatorInst(TerminatorInst &I){
    /// something has changed in BB
    DEBUG_OUTPUT("visitTerminationInst: entered");
    auto currentBB = I.getParent();
    auto old = programPoints.find(currentBB);
    if(old != programPoints.end()){
        DEBUG_OUTPUT("visitTerminationInst: old state found");
        
        // TODO: revert
        //assert(!old->second.isBottom() && "Pruning with bottom!");
        
        // From the merge of states, there are values in the map that are in
        // reality only defined for certain paths.
        // All values actually defined are also defined after the first pass.
        // Therefore remove all values that were not defined in the previous state
        if(!old->second.isBottom())
            newState.prune(old->second);
        
        /// compute lub in place after this old state is updated
        if(!old->second.leastUpperBound(newState)){
            /// new state was old state: do not push successors
            DEBUG_OUTPUT("visitTerminationInst: state has not been changed");
            DEBUG_OUTPUT("visitTerminationInst: new state equals old state in " << currentBB->getName());
            newState.print();
            return;
        }
        old->second.transferBranchConditions(newState);
        old->second.transferBottomness(newState);
    } else {
        DEBUG_OUTPUT("visitTerminationInst: old state not found");
        programPoints[currentBB] = newState;
    }
    
    DEBUG_OUTPUT("visitTerminationInst: state has been changed -> push successors");
    DEBUG_OUTPUT("visitTerminationInst: new state in bb " << currentBB->getName());
    programPoints[currentBB].print();
    newState.print();
    pushSuccessors(I);
}

void VsaVisitor::visitBranchInst(BranchInst &I){
    auto cond = I.getOperand(0);
    
    
    DEBUG_OUTPUT("CONDITIONAL BRANCHES: TEST");
    if(ICmpInst::classof(cond)){
        auto cmpInst = reinterpret_cast<ICmpInst*>(cond);
        auto ad0 = newState.getAbstractValue(cmpInst->getOperand(0));
        auto ad1 = newState.getAbstractValue(cmpInst->getOperand(1));

        // TODO: we assume that only the first argument is a variable
        auto temp = ad0->icmp(cmpInst->getPredicate(), cmpInst->getType()->getIntegerBitWidth(), *ad1);

        DEBUG_OUTPUT("CONDITIONAL BRANCHES: ");
        DEBUG_OUTPUT("   " << *ad0);
        DEBUG_OUTPUT("   " << *ad1);
        DEBUG_OUTPUT("T: " << *temp.first);
        DEBUG_OUTPUT("F: " << *temp.second);
        
        // TODO: visit only if not bottom
        newState.putBranchConditions(I.getSuccessor(0), cmpInst->getOperand(0), temp.first);
        newState.putBranchConditions(I.getSuccessor(1), cmpInst->getOperand(0), temp.second);
        
    }
    
    this->visitTerminatorInst(I);
    DEBUG_OUTPUT("blub");
}

void VsaVisitor::visitPHINode(PHINode &I){
    /// bottom
    auto bs = BoundedSet::create_bottom();
    for(Use& val:I.incoming_values()){
        //newState.getAbstractValue(val)->printOut();
            
        /// if the basic block where a value comes from is bottom,
        /// the corresponding alternative in the phi node is never taken
        /// the next 20 lines handle all the cases for that
        
        /// Check if basic block containing use is bottom
        if(Instruction::classof(val)){
            auto incomingBlock = reinterpret_cast<Instruction*>(&val)->getParent();
            
            /// block has not been visited yet -> implicit bottom
            if(programPoints.find(incomingBlock) == programPoints.end()) continue;
            
            /// explicit bottom
            if(programPoints[incomingBlock].isBottom()) continue;
        }
        
        /// if state of basic block was not bottom, include abstract value
        /// in appropriate block in lub for phi
        bs = bs->leastUpperBound(*newState.getAbstractValue(val));
    }
    
    //bs->printOut();
    
    newState.put(I,bs);
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I){
    STD_OUTPUT("visited binary instruction");
}

void VsaVisitor::visitAdd(BinaryOperator &I) {
    auto ad0 = newState.getAbstractValue(I.getOperand(0));
    auto ad1 = newState.getAbstractValue(I.getOperand(1));
    
    // TODO: meaning of arguments?
    newState.put(I, ad0->add(I.getType()->getIntegerBitWidth(),*ad1, false, false));
}

void VsaVisitor::visitMul(BinaryOperator& I) {
    auto ad0 = newState.getAbstractValue(I.getOperand(0));
    auto ad1 = newState.getAbstractValue(I.getOperand(1));
    
    // TODO: meaning of arguments?
    newState.put(I, ad0->mul(I.getType()->getIntegerBitWidth(),*ad1, false, false));

}

void VsaVisitor::visitUnaryInstruction(UnaryInstruction &I){
    // todo
}

void VsaVisitor::visitInstruction(Instruction &I){
    // todo: top or exception
    STD_OUTPUT("visitInstruction: " <<I.getOpcodeName());
}

void VsaVisitor::pushSuccessors(TerminatorInst &I){
    for(auto bb : I.successors()){
        worklist.push(bb);
    }

}

void VsaVisitor::print(){
    for(auto & pp : programPoints){
        DEBUG_OUTPUT("VsaVisitor::print():" << pp.first->getName());
        pp.second.print();
    }
}

}

