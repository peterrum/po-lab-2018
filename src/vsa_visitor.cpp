#include "vsa_visitor.h"

using namespace llvm;

namespace pcpo {

void VsaVisitor::visitInstruction(Instruction &I){
    errs() << I.getOpcodeName() << ": " << I.getValueID() << "\n";
}

void VsaVisitor::visitPHINode(PHINode &I) {
    /// todo: merge
    if(true){
        this->pushInstUsers(I);
    }
}

void VsaVisitor::visitBinaryOperator(BinaryOperator &I) {

    errs() << "visited binary Operator"<<"\n";
    if(true)
        this->pushInstUsers(I);

}

void VsaVisitor::pushInstUsers(Instruction &I) {
    for(auto values: I.users()){
        if(Instruction::classof(values)) {
            Instruction *v = reinterpret_cast<Instruction *>(values);
            worklist.push(v);
        }
    }
}

}

