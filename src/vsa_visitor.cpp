#include "vsa_visitor.h"
#include "llvm/Support/raw_os_ostream.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitBasicBlock(BasicBlock &BB){
    //errs() << "visited basic blocks" << "\n";

    /// empty state represents bottom
    newState = State();
    errs() <<newState.toString() + "\n";

    /// least upper bound with all predecessors
    for(auto pred : predecessors(&BB)){
        auto old = programPoints.find(pred);
        if(old != programPoints.end()){
            newState.leastUpperBound(old->second);
        } /// else: its state is bottom and lub(bottom, x) = x
    }
}

void VsaVisitor::visitTerminationInst(TerminatorInst &I){
    /// something has changed in BB
    auto currentBB = I.getParent();
    auto old = programPoints.find(currentBB);
    if(old != programPoints.end()){
        /// compute lub in place after this old state is updated
        if(old->second.leastUpperBound(newState)){
            /// new state was old state: do not push sucessors
            return;
        }
    } else {
        programPoints[currentBB] = newState;
    }
    pushSuccessors(I);
}

void VsaVisitor::visitPHINode(PHINode &I){
    /// bottom
    std::shared_ptr<AbstractDomain> bs (new BoundedSet(false));
    for(Use& val:I.incoming_values()){
        bs->leastUpperBound(*newState.getAbstractValues(val));
    }
    newState.put(I,bs);
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I){
    errs() << "visited binary instruction \n";
    //auto ad0 = newState.getAbstractValues(I.getOperand(0));
    //auto ad1 = newState.getAbstractValues(I.getOperand(1));
}

void VsaVisitor::visitAdd(BinaryOperator &I) {
    errs() << "visited add instruction \n";
    auto op0 = I.getOperand(0);
    errs() << "after getOperand \n";
    errs() << newState.toString() + " bla  \n";
    auto ad0 = newState.getAbstractValues(op0);
    //auto ad1 = newState.getAbstractValues(I.getOperand(1));

    //newState.put(I, ad0->add(*ad1));
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
    errs() << I.getOpcodeName() << ": " << I.getValueID() << "\n";
}

void VsaVisitor::pushSuccessors(TerminatorInst &I){
    for(auto bb : I.successors())
        worklist.push(bb);

}

}

