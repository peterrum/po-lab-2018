#include "vsa_visitor.h"
#include "llvm/Support/raw_os_ostream.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitBasicBlock(BasicBlock &BB){
    /// empty state represents bottom
    newState = State();
    DEBUG_OUTPUT("visitBasicBlock: entered");
    /// least upper bound with all predecessors
    for(auto pred : predecessors(&BB)){
        DEBUG_OUTPUT("visitBasicBlock: pred" << pred->getName() << " found");
        auto old = programPoints.find(pred);
        if(old != programPoints.end()){
            DEBUG_OUTPUT("visitBasicBlock: state for" << pred->getName() << " found");
            newState.leastUpperBound(old->second);
        } /// else: its state is bottom and lub(bottom, x) = x
    }
}

void VsaVisitor::visitTerminatorInst(TerminatorInst &I){
    /// something has changed in BB
    DEBUG_OUTPUT("visitTerminationInst: entered");
    auto currentBB = I.getParent();
    auto old = programPoints.find(currentBB);
    if(old != programPoints.end()){
        DEBUG_OUTPUT("visitTerminationInst: old state found");
        /// compute lub in place after this old state is updated
        if(old->second.leastUpperBound(newState)){
            /// new state was old state: do not push sucessors
            return;
        }
    } else {
        DEBUG_OUTPUT("visitTerminationInst: old state not found");
        programPoints[currentBB] = newState;
    }
    pushSuccessors(I);
}

void VsaVisitor::visitPHINode(PHINode &I){
    /// bottom
    std::shared_ptr<AbstractDomain> bs (new BoundedSet(false));
    for(Use& val:I.incoming_values()){
        //newState.getAbstractValues(val)->printOut();
        bs = bs->leastUpperBound(*newState.getAbstractValues(val));
    }
    
    bs->printOut();
    
    newState.put(I,bs);
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I){
    STD_OUTPUT("visited binary instruction");
}

void VsaVisitor::visitAdd(BinaryOperator &I) {
    auto ad0 = newState.getAbstractValues(I.getOperand(0));
    auto ad1 = newState.getAbstractValues(I.getOperand(1));
    
    newState.put(I, ad0->add(*ad1));
}

void VsaVisitor::visitMul(BinaryOperator& I) {
    //auto ad0 = newState.getAbstractValues(I.getOperand(0));
    //auto ad1 = newState.getAbstractValues(I.getOperand(1));
    //newState.put(I, ad0->mul(*ad1));

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

}

